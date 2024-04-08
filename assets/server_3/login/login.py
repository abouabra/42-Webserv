import os
from hashlib import sha256
from urllib.parse import parse_qs
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import secret_key, username, password_hash, get_error_body

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
    """Validates provided credentials against hard-coded credentials."""
    return (
        form_username == username
        and get_hashed_password(form_password) == password_hash
    )


def generate_cookie(username, value):
    """Generates a secure cookie with hashed value."""
    hashed_value = sha256(f"{username}{value}{secret_key}".encode()).hexdigest()
    return f"auth_session={username}-{value}-{hashed_value}"


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
            if validate_credentials(username, password):
                cookie = generate_cookie(username, 'logged_in')
                if remember_me:
                    global cookie_header
                    cookie_header = f"Set-Cookie: {cookie}; Max-Age=3600; Path=/"
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


def main():
    """Main function for login.py"""

    method = os.environ.get("REQUEST_METHOD", "")
    size = os.environ.get("CONTENT_LENGTH", 0)

    if int(size) == 0 and method == 'POST':
        print("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n")
        exit(0)
    
    if method != 'GET' and method != 'POST':
        send_response("405 Method Not Allowed", get_error_body("Method not allowed"))
        exit(0)

    response_body = handle_request(method, size)

    print("HTTP/1.1 200 OK\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print("Content-Length: " + str(len(response_body)) + "\r\n", end="")
    if cookie_header:
        print(cookie_header + "\r\n", end="")
    print("\r\n", end="")
    print(response_body)


if __name__ == "__main__":
    main()