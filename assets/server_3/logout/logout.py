
import os

secret_key = "my_super_secret_key_1337"

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
        return "<h1>Invalid cookie</h1>"
    
    global cookie_header
    cookie_header = remove_cookie(cookie)
    response_body = "<script>setTimeout(() => { window.location.href = '/login/'; }, 200);</script>"
    return response_body

def main():
    """Main function simulating server behavior."""

    method = os.environ.get("REQUEST_METHOD", "")
    cookies = os.environ.get("HTTP_COOKIE", "")
    if not cookies:
        return send_response("200 OK", "<h1>Not logged in</h1>")

    if method != 'GET':
        return send_response("405 Method Not Allowed", "<h1>Method not allowed</h1>")

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