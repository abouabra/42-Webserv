import os
import re
import sys
import uuid


def parse_filename(content):
    # search for the filename
    filename_pattern = r'filename="([^"]+)"'
    match = re.search(filename_pattern, content, re.IGNORECASE)
    return match.group(1).strip() if match else ""

def handle_upload(request_body, upload_dir):
    boundary = os.environ.get("CONTENT_TYPE", "")
    if boundary.find("boundary=") != -1:
        boundary = boundary.split("boundary=")[1]
    else:
        boundary = None

    filename = None
    if boundary:
        part = request_body.split(boundary.encode())[1:-1][0]
        header, data = part.split(b'\r\n\r\n', 1)

        filename = parse_filename(header.decode())
    else:
        data = request_body

    if not filename:
        filename = str(uuid.uuid4())
    else:
        filename, file_extension = os.path.splitext(filename)
        filename = f"{filename}-{str(uuid.uuid4())}{file_extension}"
    
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    file_path = os.path.join(upload_dir,filename)

    try:
        with open(file_path, "wb") as f:
            if boundary:
                data = data.split(b'\r\n--', 1)[0]
            f.write(data)
        return f"<h1>201 - Created</h1><h3>{filename} has been uploaded successfully!</h3><h3>Go to upload directory to see your file</h3>"
    except Exception as e:
        return f"<h1>Error during upload: {str(e)}</h1>"

    return "<h1>No file found in upload data</h1>"

def main():
    method = os.environ.get("REQUEST_METHOD", "")
    if method != "POST":
        exit(1)

    content_length = os.environ.get("CONTENT_LENGTH", "")
    if not content_length:
        print("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 22\r\nConnection: keep-alive\r\n\r\nError: Invalid request")
        exit(0)

    upload_dir = os.environ.get("UPLOAD_DIR", "")
    if not upload_dir:
        print("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 31\r\nConnection: keep-alive\r\n\r\nError: Upload directory not set")
        exit(0)

    # Read the entire request body
    try:
        request_body = sys.stdin.buffer.read()
    except Exception as e:
        exit(1)

    # Generate HTTP response body
    response_body = "<!DOCTYPE html><html><head><title>Upload</title><meta charset=\"UTF-8\" />"
    response_body += "<style>body { font-family: sans-serif; background-color: #141615; color: #317aed; font-size: 2em; text-align: center; }</style>"
    response_body += "</head><body>"
    data = handle_upload(request_body, upload_dir)
    if data[:7] == "<h1>201":
        status = "201 Created"
    else:
        status = "400 Bad Request"
    response_body += data
    response_body += "</body></html>"

    # Generate HTTP response headers
    print(f"HTTP/1.1 {status}\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print("Content-Length: " + str(len(response_body)) + "\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print("\r\n", end="")
    print(response_body)

if __name__ == "__main__":
    main()
