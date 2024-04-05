# this is the page for the POST request it will just print the data that was sent to it
import os

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")

body = "<html><head><title>POST Data</title></head>"
body += "<body>"
body += "<h1>POST Data</h1>"

# read all the data from the POST request
size = os.environ["CONTENT_LENGTH"]
size = int(size)
data = os.read(0, size)
data = data.decode("utf-8")
body += f"<p>{data}</p>"
body += "</body></html>"
print("Content-Length: " + str(len(body)))
print()
print(body)