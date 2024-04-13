import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import get_error_body


def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

cookie_header = ""

def handle_request(cookies):
    global cookie_header

    for cookie in cookies:
        if cookie.startswith("theme"):
            value = cookie.split("=")[1]
            if value == "dark":
                cookie_header = f"Set-Cookie: theme=light; Max-Age=3600; path=/"
            elif value == "light":
                cookie_header = f"Set-Cookie: theme=dark; Max-Age=3600; path=/"
            return
    
    cookie_header = f"Set-Cookie: theme=light; Max-Age=3600; path=/"

def main():
    method = os.environ.get("REQUEST_METHOD", "")

    if method != 'GET':
        send_response("405 Method Not Allowed", get_error_body("Method not allowed"))
        exit(0)

    cookies = []
    for item in os.environ:
        if item.startswith("HTTP_COOKIE"):
            head = item.replace("HTTP_COOKIE_", "")
            cookies.append(f"{head}={os.environ[item]}")

    handle_request(cookies)

    print("HTTP/1.1 302 Found\r\n", end="")
    print("Location: /\r\n", end="")
    print("Content-Length: 0\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print(cookie_header + "\r\n", end="")
    print("\r\n", end="")

if __name__ == "__main__":
    main()