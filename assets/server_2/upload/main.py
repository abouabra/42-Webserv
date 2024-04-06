import os
import re
import sys

def parse_filename(content):
    # search for the filename
    filename_pattern = r'filename="([^"]+)"'  # Regular expression for filename
    match = re.search(filename_pattern, content, re.IGNORECASE)
    return match.group(1).strip() if match else ""

def handle_upload(request_body):
    upload_dir = "upload_dir"  # Modify this to your desired upload directory
    boundary = os.environ.get("CONTENT_TYPE", "").split("boundary=")[-1]

    if not boundary:
        return "<h1>Error: Could not find boundary in Content-Type</h1>"

    parts = request_body.split(boundary.encode())[1:-1]

    for part in parts:
        header, data = part.split(b'\r\n\r\n', 1)

        filename = parse_filename(header.decode())
        if not filename:
            continue
        
        script_path = os.environ.get("PATH_INFO")
        if not script_path:
            return "<h1>Error: Could not determine script path</h1>"
        script_path = script_path.rsplit("/", 1)[0]
        file_path = os.path.join(script_path, upload_dir, filename)

        try:
            with open(file_path, "wb") as f:
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

    # Read the entire request body
    try:
        request_body = sys.stdin.buffer.read()
    except Exception as e:
        print(f"Error reading request body: {str(e)}", file=sys.stderr)
        exit(1)

    response_body = "<!DOCTYPE html><html><head><title>Upload</title><meta charset=\"UTF-8\" />"
    response_body += "<style>body { font-family: sans-serif; background-color: #141615; color: #317aed; font-size: 2em; text-align: center; }</style>"
    response_body += "</head><body>"
    response_body += handle_upload(request_body)
    response_body += "</body></html>"

    # Generate HTTP response headers
    print("HTTP/1.1 201 Created")
    print("Content-Type: text/html")
    print("Content-Length: " + str(len(response_body)))
    print()
    print(response_body)

if __name__ == "__main__":
    main()