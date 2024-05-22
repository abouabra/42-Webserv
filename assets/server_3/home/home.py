import os
from hashlib import sha256
from urllib.parse import parse_qs
import sys
import csv

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import home_page_body, get_error_body, accounts_db_file ,session_db_file, check_session_timeout

def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

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

def handle_request(cookies):
    """Handles GET requests for the home page."""

    cookie_username = ""
    theme = "dark"
    for cookie in cookies:
        if cookie.startswith("SESSION_ID="):
            cookie_username = is_session_valid(cookie)
        if cookie.startswith("theme"):
            theme = cookie.split("=")[1]

    if not cookie_username:
        print("HTTP/1.1 302 Found\r\nLocation: /login/\r\nContent-Length: 0\r\n\r\n")
        exit(0)

    response_body = home_page_body(cookie_username, theme)
    return response_body

   

def main():
    """Main function for home.py"""

    method = os.environ.get("REQUEST_METHOD", "")
    cookies = []
    for item in os.environ:
        if item.startswith("HTTP_COOKIE"):
            head = item.replace("HTTP_COOKIE_", "")
            cookies.append(f"{head}={os.environ[item]}")

    check_session_timeout()

    if method != 'GET':
        return send_response("405 Method Not Allowed", get_error_body("Method not allowed"))

    response_body = handle_request(cookies)

    send_response("200 OK", response_body)


if __name__ == "__main__":
    main()