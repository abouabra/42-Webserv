
from hashlib import sha256

secret_key = "my_super_secret_key_1337"
username = "admin"
plain_password = "admin"
password_hash = sha256(plain_password.encode()).hexdigest()

def home_page_body(cookie_username, theme):
    return f"""
    <!DOCTYPE html>
    <html lang="en">
        <head>
            <meta charset="UTF-8" />
            <meta name="viewport" content="width=device-width, initial-scale=1.0" />
            <link rel="icon" href="favicon.ico" />
            <title>Server 3</title>
            <style>
                body {{
                    background-color: {theme == "dark" and "#141615" or "#f0f0f0"};
                }}
            </style>
            <link rel="stylesheet" href="style.css" />
        </head>
        <body>
            <div class="container">
                <div class="left-side">
                    <h1>Server 3</h1>
                    <div>
                        <h3>Welcome <span class="highlight_user">{cookie_username}</span> to your Home Page</h3>
                        <h3>This is a proof of concept page</h3>
                        <h3>for cookies and session management</h3>
                    </div>
                    <div class="button-container">
                        <h3>Here you can Log Out</h3>
                        <button
                            class="button"
                            role="button"
                            onclick="location.href='/logout/'">
                            Log Out
                        </button>
                    </div>
                    <div class="button-container">
                        <h3>Here you can switch theme</h3>
                        <button
                            class="button"
                            role="button"
                            onclick="location.href='/switch/'">
                            Switch
                        </button>
                    </div>
                </div>
                <div class="right-side"></div>
            </div>
        </body>
    </html>
    """

def get_error_body(error_line):
    return  f"""
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8" />
                <title>Error</title>
                <link rel="stylesheet" href="/style.css" />
            </head>
            <body>
                <div class="error_page">
                    <h1 class="error_line">{error_line}</h1>
                    <a href="/"><button class="button">Go to Main Page</button></a>
                </div>
            </body>
        </html>
    """

def get_logout_body():
    return  """
        <!DOCTYPE html>
        <html>
            <head>
                <meta charset="UTF-8" />
                <title>Logout</title>
                <link rel="stylesheet" href="/style.css" />
            </head>
            <body>
                <div class="error_page">
                    <h1>Logging out...</h1>
                </div>
                <script>setTimeout(() => { window.location.href = '/login/'; }, 500);</script>
            </body>
        </html>
    """

def main():
    """Send a 403 Forbidden response."""
    
    response_body = """
    <!DOCTYPE html>
    <html>
        <head>
            <meta charset="UTF-8" />
            <title>Forbidden</title>
        </head>
        <body>
            <h1>403 Forbidden</h1>
            <p>You don't have permission to access this resource.</p>
        </body>
    </html>
    """

    print(f"HTTP/1.1 403 Forbidden\r\n", end="")
    print("Content-Type: text/html\r\n", end="")
    print(f"Content-Length: {len(response_body)}\r\n", end="")
    print("Connection: keep-alive\r\n", end="")
    print("\r\n", end="")
    print(response_body, end="")


if __name__ == "__main__":
    main()
