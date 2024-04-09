import os
from urllib.parse import unquote

def get_error_body(error_line):
    return  f"""
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <title>Error</title>
                <link rel="stylesheet" href="/cgi/style.css" />
            </head>
            <body>
                <div class="error_page">
                    <h1 class="error_line">{error_line}</h1>
                    <a href="/"><button class="button">Go to Main Page</button></a>
                </div>
            </body>
        </html>
    """

def get_main_body(fname, lname, msg):
    return  f"""
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8">
                <title>Python CGI with GET</title>
                <link rel="stylesheet" href="/cgi/style.css" />
            </head>
            <body>
                <div class="error_page">
                    <h1>Python CGI with GET Method</h1>
                    <h1>First Name: {fname}</h1>
                    <h1>Last Name: {lname}</h1>
                    <h1>Message: {msg}</h1>
                </div>
            </body>
        </html>
    """


def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

def handle_request(query_string):
    method, language, fname, lname, msg = query_string.split("&")
    if not fname.startswith("fname=") or not lname.startswith("lname=") or not msg.startswith("msg="):
        send_response("400 Bad Request", get_error_body("Invalid query string"))
        exit(1)
    

    fname = fname.split("=")[1]
    lname = lname.split("=")[1]
    msg = msg.split("=")[1]

    fname = unquote(fname).replace("+", " ")
    lname = unquote(lname).replace("+", " ")
    msg = unquote(msg).replace("+", " ")

    return get_main_body(fname, lname, msg)

def main():
    """Main function for home.py"""

    method = os.environ.get("REQUEST_METHOD", "")
    query_string = os.environ.get("QUERY_STRING", "")

    if method != 'GET':
        return send_response("405 Method Not Allowed", get_error_body("Method not allowed"))

    response_body = handle_request(query_string)

    send_response("200 OK", response_body)

if __name__ == "__main__":
    main()