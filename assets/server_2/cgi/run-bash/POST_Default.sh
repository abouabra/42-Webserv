#!/bin/bash

# Function to handle errors
function send_error_response() {
  error_line="$1"
  response_body=$(cat <<EOF
<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>Error</title>
        <link rel="stylesheet" href="/cgi/style.css" />
    </head>
    <body>
        <div class="error_page">
            <h1 class="error_line">$error_line</h1>
            <a href="/"><button class="button">Go to Main Page</button></a>
        </div>
    </body>
</html>
EOF
)

  cat <<EOF
HTTP/1.1 400 Bad Request
Content-Type: text/html
Content-Length: ${#response_body}

$response_body
EOF
  exit 1
}

# Function to send a successful response with the given body
function send_response() {
  code="$1"
  body="$2"
  len=$(printf "%s" "$body" | wc -c)
  cat <<EOF
HTTP/1.1 $code
Content-Type: text/html
Content-Length: $len
Connection: keep-alive

$body
EOF
}

# Function to handle the request
function handle_request() {
    method="$REQUEST_METHOD"
    method="${method#*=}"

	if [[ $method != "POST" ]]; then  # Use != for comparison
		send_error_response "Method not allowed"
	fi

    size=${CONTENT_LENGTH:-0}
    if [[ $size -eq 0 ]]; then
        send_error_response "No data received"
    fi

    read -n "$size" data

    fname="${data#*&fname=}"
    fname="${fname%%&*}"
    fname="${fname#*=}"
    
    lname="${data#*&lname=}"
    lname="${lname%%&*}"
    lname="${lname#*=}"

    msg="${data#*&msg=}"
    msg="${msg%%&*}"
    msg="${msg#*=}"



  if [[ -z $fname || -z $lname || -z $msg ]]; then  # Use -z to check for empty variables
    send_error_response "Invalid query string"
  fi



  fname="${fname//+/ }"
  lname="${lname//+/ }"
  msg="${msg//+/ }"

  response_body=$(cat <<EOF
<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>BASH CGI with POST</title>
        <link rel="stylesheet" href="/cgi/style.css" />
    </head>
    <body>
        <div class="error_page">
            <h1>BASH CGI with POST Method</h1>
            <h1>First Name: $fname</h1>
            <h1>Last Name: $lname</h1>
            <h1>Message: $msg</h1>
        </div>
    </body>
</html>
EOF
)

  send_response "200 OK" "$response_body"
}

# Main function
function main() {
  handle_request
}

main
