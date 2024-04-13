<?php

// Function to handle errors
function send_error_response($error_line) {
    $response_body = <<<EOF
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
EOF;

    echo "HTTP/1.1 400 Bad Request\r\n";
    echo "Content-Type: text/html\r\n";
    echo "Content-Length: " . strlen($response_body) . "\r\n\r\n";
    echo $response_body;
    exit();
}

// Function to send a successful response with the given body
function send_response($code, $body) {
    echo "HTTP/1.1 $code\r\n";
    echo "Content-Type: text/html\r\n";
    echo "Content-Length: " . strlen($body) . "\r\n";
    echo "Connection: keep-alive\r\n";
    echo "\r\n";
    echo $body;
}

// Function to handle the request
function handle_request() {
    $method = $_SERVER['REQUEST_METHOD'];
	if ($method !== "GET") {
		send_error_response("Method not allowed");
	}

    $query_string = $_SERVER['QUERY_STRING'];
    
    $fname = explode("&", $query_string)[2];
    $fname = explode("=", $fname)[1];

    $lname = explode("&", $query_string)[3];
    $lname = explode("=", $lname)[1];
    
    $msg = explode("&", $query_string)[4];
    $msg = explode("=", $msg)[1];

   

    if (!$fname || !$lname || !$msg) {
        send_error_response("Invalid query string");
    }

    // Decode URL-encoded values
    $fname = urldecode($fname);
    $lname = urldecode($lname);
    $msg = urldecode($msg);

    $response_body = <<<EOF
<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>PHP CGI with GET</title>
        <link rel="stylesheet" href="/cgi/style.css" />
    </head>
    <body>
        <div class="error_page">
            <h1>PHP CGI with GET Method</h1>
            <h1>First Name: $fname</h1>
            <h1>Last Name: $lname</h1>
            <h1>Message: $msg</h1>
        </div>
    </body>
</html>
EOF;

    send_response("200 OK", $response_body);
}

// Main function
handle_request();

?>
