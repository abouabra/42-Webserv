#!/bin/bash

# Print HTTP header
printf "HTTP/1.1 200 OK\r\n"
printf "Content-Type: text/html\r\n"

# Start HTML body
body="<html><head><title>POST Data</title></head>"
body+="<body>"
body+="<h1>POST Data</h1>"

# Get size of POST data
size=${CONTENT_LENGTH:-0}

# Read POST data
read -n "$size" data

# Add POST data to body
body+="<p>$data</p>"

# End HTML body
body+="</body></html>"

# Print Content-Length header
printf "Content-Length: ${#body}\r\n"

# Print empty line to separate header and body
printf "\r\n"

# Print HTML body
printf "$body"
