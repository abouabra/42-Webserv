import os
import re
import sys

def parse_filename(content):

    # seach for this the line containing this phrase then print it
    # Content-Disposition:
    # content_disposition_pattern = r'Content-Disposition:.*'  # Regular expression for Content-Disposition
    # match = re.search(content_disposition_pattern, content, re.IGNORECASE)
    # print(match.group(0), file=sys.stderr)




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
        print(f"Uploading file to: {file_path}", file=sys.stderr)

        # Remove trailing boundary and remove one more line
        # data = data.rsplit(b'--', 1)[0].rsplit(b'\r\n', 1)[0]



        try:
            with open(file_path, "wb") as f:
                data = data.split(b'\r\n--', 1)[0]
                f.write(data)
            return f"<h1>File uploaded successfully: {filename}</h1>"
        except Exception as e:
            return f"<h1>Error during upload: {str(e)}</h1>"

    return "<h1>No file found in upload data</h1>"

def main():

    # print("Reached Here", file=sys.stderr)


    try:
        # Read the entire request body
        request_body = sys.stdin.buffer.read()
    except Exception as e:
        print(f"Error reading request body: {str(e)}", file=sys.stderr)
        exit(1)
    # print(request_body, file=sys.stderr)
    response_body = handle_upload(request_body)

    # Generate HTTP response headers
    print("HTTP/1.1 201 Created")
    print("Content-Type: text/html")
    print("Content-Length: " + str(len(response_body)))
    print()
    print(response_body)

if __name__ == "__main__":
    main()