#!/bin/bash

# Print header
printf "HTTP/1.1 200 OK\r\n"
printf "Content-Type: text/html\r\n"

# Build body HTML
body="<html><head><title>Environment Variables</title></head>"
body+="<body>"

# Loop through environment variables
for var in $(env); do
    IFS='=' read -r -a array <<< "$var"
    body+="${array[0]}: ${array[1]}<br>"
done

body+="</body></html>"

# Calculate and print content length
content_length=${#body}
printf "Content-Length: $content_length\r\n"

# End headers
printf "\r\n"

# Print body
printf "$body"
