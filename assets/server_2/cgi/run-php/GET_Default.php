<?php

// Print header
echo "HTTP/1.1 200 OK\r\n";
echo "Content-Type: text/html\r\n";

// Build body HTML
$body = "<html><head><title>Environment Variables</title></head>";
$body .= "<body>";

// Print environment variables from system
$body .= "</body></html>";
foreach ($_SERVER as $key => $value) {
    if (is_array($value)) {
        $value = implode(", ", $value);
    }
    $body .= "$key: $value<br/>";
}
// Calculate content length
$content_length = strlen($body);

// Print content length header
echo "Content-Length: $content_length\r\n";

// print new line
echo "\r\n";

// Print body
echo $body;

?>
