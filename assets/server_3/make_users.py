from hashlib import sha256

def get_hashed_password(password):
    return sha256(password.encode()).hexdigest()

password = "mbaanni"
print(get_hashed_password(password))