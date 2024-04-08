import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import get_logout_body, get_error_body
cookie_header = ""

def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

def remove_cookie(cookie):
    """Removes the cookie from the client's browser."""
    return f"Set-Cookie: {cookie}; Max-Age=0; path=/"

def handle_request(cookie):
    """Handles the incoming HTTP request."""

    cookie_name, cookie_value = cookie.split("=")
    if cookie_name != "auth_session":
        return get_error_body("Invalid cookie")
    
    global cookie_header
    cookie_header = remove_cookie(cookie)
    return get_logout_body()

def main():
    """Main function simulating server behavior."""

    method = os.environ.get("REQUEST_METHOD", "")
    cookies = os.environ.get("HTTP_COOKIE", "")
    if not cookies:
        return send_response("200 OK", get_error_body("Not logged in"))

    if method != 'GET':
        return send_response("405 Method Not Allowed", get_error_body("Method not allowed"))

    response_body = handle_request(cookies)
   
    print("HTTP/1.1 200 OK\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print("Content-Length: " + str(len(response_body)) + "\r\n", end="")

    if cookie_header:
        print(cookie_header + "\r\n", end="")
    print("\r\n", end="")
    print(response_body)
    
if __name__ == "__main__":
    main()