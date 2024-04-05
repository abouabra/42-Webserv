#! /usr/bin/python3
# loop over the environment variables and print them
import os

print("HTTP/1.1 200 OK\r\n", end="")
print("Content-Type: text/html\r\n", end="")

body = "<html><head><title>Environment Variables</title></head>"
body += "<body>"
for key, value in os.environ.items():
    body += f"{key}: {value}<br>"
body += "</body></html>"

print("Content-Length: " + str(len(body)) + "\r\n", end="")
print("\r\n", end="")
print(body)