<?php
// Print HTTP header
echo "HTTP/1.1 200 OK\r\n";
echo "Content-Type: text/html\r\n";

// Start HTML body
$body = "<html><head><title>POST Data</title></head>";
$body .= "<body>";
$body .= "<h1>POST Data</h1>";


// get Content-Length header
$content_length = (int) $_SERVER["CONTENT_LENGTH"];

// Read POST data
$post_data = "";
if ($content_length > 0) {
    $post_data = fread(STDIN, $content_length); // Use STDIN directly for clarity
}
// Add POST data to body
$body .= "<p>" . htmlspecialchars($post_data) . "</p>";

// End HTML body
$body .= "</body></html>";

// Print Content-Length header
echo "Content-Length: " . strlen($body) . "\r\n";

// Print new line
echo "\r\n";

// Print HTML body
echo $body;
?>
