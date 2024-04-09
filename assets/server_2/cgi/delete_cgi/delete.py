import os
from datetime import datetime

def send_response(code, body):
    """Sends an HTTP response with the provided code and body."""
    print(f"HTTP/1.1 {code}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(body)}\r\n", end="")
    print("\r\n", end="")
    print(body, end="")

# change permissions of the file such that std::remove() will fail to delete it
# chmod 444 data.db
# chwon root:root data.db

def get_error_body(error_line):
    return  f"""
        <!DOCTYPE html>
        <html>
            <head>
                <title>Error</title>
                <link rel="stylesheet" href="/cgi/delete_cgi/style.css" />
            </head>
            <body>
                <div class="error_page">
                    <h1>{error_line}</h1>
                    <a href="/"><button class="button">Go to Main Page</button></a>
                </div>
            </body>
        </html>
    """

def get_main_body(line):
    return  f"""
        <!DOCTYPE html>
        <html>
            <head>
                <title>DELETE CGI</title>
                <link rel="stylesheet" href="/cgi/delete_cgi/style.css" />
            </head>
            <body>
                <div class="error_page">
                    <h1>{line}</h1>
                </div>
            </body>
        </html>
    """

def handle_request(method):

    if method == 'GET':
        path = os.path.join(os.path.dirname(__file__), "data.db")
        with open(path, "r") as f:
            count = len(f.readlines())
        return get_main_body(f"DELETE request received {count} times.")



    elif method == 'DELETE':
        date = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        line = f"DELETE request received at {date}\n"
        path = os.path.join(os.path.dirname(__file__), "data.db")
        with open(path, "a") as f:
            f.write(line)
        return get_main_body("Request received and logged.")

def main():

    method = os.environ.get("REQUEST_METHOD", "")
    if method not in ["GET", "DELETE"]:
        return send_response("405 Method Not Allowed", get_error_body("Method not allowed"))

    response_body = handle_request(method)

    print("HTTP/1.1 200 OK\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print("Content-Length: " + str(len(response_body)) + "\r\n", end="")
    print("\r\n", end="")
    print(response_body)

if __name__ == "__main__":
    main()