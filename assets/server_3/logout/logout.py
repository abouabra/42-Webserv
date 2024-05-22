import os
import sys
import csv


sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from config import get_logout_body, get_error_body, session_db_file, check_session_timeout
cookie_header = ""

def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

def remove_cookie(cookie):
    value = cookie.split("=")[1]
    session_db_read = csv.reader(open(session_db_file, "r"), delimiter=",")

    data = [row for row in session_db_read]
    for row in data:
        if value == row[1]:
            data.remove(row)
            break

    session_db_write = csv.writer(open(session_db_file, "w"), delimiter=",")
    for row in data:
        session_db_write.writerow(row)
    return f"Set-Cookie: {cookie}; Max-Age=0; path=/"

def handle_request(cookies):
    """Handles the incoming HTTP request."""
    counter = 0
    global cookie_header
    for cookie in cookies:
        if cookie.startswith("SESSION_ID="):
            header = remove_cookie(cookie)
            if(counter > 0):
                cookie_header += "\r\n" + header
            else:
                cookie_header = header
            counter += 1
        if cookie.startswith("theme"):
            header = f"Set-Cookie: {cookie}; Max-Age=0; path=/"
            if(counter > 0):
                cookie_header += "\r\n" + header
            else:
                cookie_header = header
            counter += 1
    print(cookie_header, file=sys.stderr)

    if counter > 0:
        return get_logout_body()
    return get_error_body("Not logged in")

def main():
    """Main function simulating server behavior."""

    method = os.environ.get("REQUEST_METHOD", "")
    cookies = []
    for item in os.environ:
        if item.startswith("HTTP_COOKIE"):
            head = item.replace("HTTP_COOKIE_", "")
            cookies.append(f"{head}={os.environ[item]}")

    check_session_timeout()
    if not cookies:
        return send_response("200 OK", get_error_body("Not logged in"))

    if method != 'GET':
        return send_response("405 Method Not Allowed", get_error_body("Method not allowed"))

    response_body = handle_request(cookies)
   
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