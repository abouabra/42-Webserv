import os
from hashlib import sha256
from urllib.parse import parse_qs
import sys
import csv
import uuid
import time


sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import get_error_body, accounts_db_file, session_db_file, check_session_timeout

cookie_header = ""

def read_file(file_name):
    """Reads the content of the provided file."""

    path = os.path.join(os.path.dirname(__file__), file_name)
    with open(path, "r") as f:
        return f.read()


def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print("\r\n", end="")
    print(body, end="")


def parse_login_form(post_body):
    """Extracts username, password, and 'remember_me' flag from POST data."""
    try:
        form_data = parse_qs(post_body)
        return (
            form_data.get("username", [None])[0],
            form_data.get("password", [None])[0],
            "on" in form_data.get("remember_me", []),
        )

    except Exception as e:
        return None, None, None

def get_hashed_password(password):
    """Hashes a password using SHA-256."""
    return sha256(password.encode()).hexdigest()


def validate_credentials(form_username, form_password):
    accounts_db = csv.reader(open(accounts_db_file, "r"), delimiter=",")
    for row in accounts_db:
        if form_username == row[1] and get_hashed_password(form_password) == row[2]:
            return row[0]
    return -1


def generate_session(user_id):
    session_db_read = csv.reader(open(session_db_file, "r"), delimiter=",")
    session_db_write = csv.writer(open(session_db_file, "a"), delimiter=",") 
    next(session_db_read)

    last_id = 0
    for row in session_db_read:
        last_id = int(row[0])
    new_id = last_id + 1
    session_id = uuid.uuid4().hex
    session_creation_time = str(int(time.time()))
    session_db_write.writerow([new_id, session_id, user_id, session_creation_time])
    return session_id

def handle_request(method, size):
    """Handles GET and POST requests for login functionality."""

    if method == 'GET':
        response_body = read_file("login.html")
        return response_body

    elif method == 'POST':
        post_body = ""
        post_body = os.read(0, int(size))
        post_body = post_body.decode("utf-8")

        username, password, remember_me = parse_login_form(post_body)
        if username and password:
            user_id = validate_credentials(username, password)
            if user_id != -1:
                session_id = generate_session(user_id)
                cookie = f"SESSION_ID={session_id}"
                if remember_me:
                    global cookie_header
                    cookie_header = f"Set-Cookie: {cookie}; Max-Age=86400; Path=/"
                else:
                    cookie_header = f"Set-Cookie: {cookie}; Path=/"

                print (f"HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Length: 0\r\n{cookie_header}\r\n\r\n")
                exit(0)
            else:
                send_response("401 Unauthorized", get_error_body("Invalid credentials"))
                exit(0)
        else:
            send_response("400 Bad Request", get_error_body("Invalid form data"))
            exit(0)


def is_session_valid(cookie):
    session_db = csv.reader(open(session_db_file, "r"), delimiter=",")
    accounts_db = csv.reader(open(accounts_db_file, "r"), delimiter=",")
    value = cookie.split("=")[1]
    for row in session_db:
        if value == row[1]:
            for account in accounts_db:
                if row[2] == account[0]:
                    return account[1]
    return None

def main():
    """Main function for login.py"""

    method = os.environ.get("REQUEST_METHOD", "")
    size = os.environ.get("CONTENT_LENGTH", 0)

    check_session_timeout()
    if int(size) == 0 and method == 'POST':
        print("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n")
        exit(0)
    
    if method != 'GET' and method != 'POST':
        send_response("405 Method Not Allowed", get_error_body("Method not allowed"))
        exit(0)

    cookies = []
    for item in os.environ:
        if item.startswith("HTTP_COOKIE"):
            head = item.replace("HTTP_COOKIE_", "")
            cookies.append(f"{head}={os.environ[item]}")

    for cookie in cookies:
        if cookie.startswith("SESSION_ID="):
            global cookie_header
            cookie_username = is_session_valid(cookie)
            if cookie_username:
                print("HTTP/1.1 302 Found\r\nLocation: /\r\nContent-Length: 0\r\n\r\n")
                exit(0)

    response_body = handle_request(method, size)

    print("HTTP/1.1 200 OK\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print("Content-Length: " + str(len(response_body)) + "\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    if cookie_header:
        print(cookie_header + "\r\n", end="")
    print("\r\n", end="")
    print(response_body)


if __name__ == "__main__":
    main()