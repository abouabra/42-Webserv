import os
from hashlib import sha256
from urllib.parse import parse_qs
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import secret_key, username, home_page_body, get_error_body

def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

def generate_cookie(username, value):
    """Generates a secure cookie with hashed value."""
    hashed_value = sha256(f"{username}{value}{secret_key}".encode()).hexdigest()
    return f"auth_session={username}-{value}-{hashed_value}"


def is_logged_in(cookies):
    """Checks if the user is logged in based on valid cookies."""
    if not cookies:
        return False

    try:
        cookie_name, cookie_value = cookies.split("=")
        if cookie_name != "auth_session":
            return False

        username_from_cookie, value, hashed_value = cookie_value.split("-")
        return (
            cookies == generate_cookie(username_from_cookie, value)
            and hashed_value == sha256(f"{username_from_cookie}{value}{secret_key}".encode()).hexdigest()
            and username_from_cookie == username
        )

    except ValueError:
        return False


def handle_request(cookies):
    """Handles GET requests for the home page."""

    if is_logged_in(cookies):
        cookie_username = cookies.split("=")[1].split("-")[0]
        response_body = home_page_body(cookie_username)
        return response_body
    else:
        print("HTTP/1.1 302 Found\r\nLocation: /login/\r\nContent-Length: 0\r\n\r\n")
        exit(0)

def main():
    """Main function for home.py"""

    method = os.environ.get("REQUEST_METHOD", "")
    cookies = os.environ.get("HTTP_COOKIE", "")

    if method != 'GET':
        return send_response("405 Method Not Allowed", get_error_body("Method not allowed"))

    response_body = handle_request(cookies)

    print("HTTP/1.1 200 OK\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print("Content-Length: " + str(len(response_body)) + "\r\n", end="")
    print("\r\n", end="")
    print(response_body)

if __name__ == "__main__":
    main()