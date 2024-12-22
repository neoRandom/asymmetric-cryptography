import random
import math

# A set will be the collection of prime numbers,
# where we can select random primes p and q
prime = []

public_key = None
private_key = None
n = None


# Extended Euclidean Algorithm
def extended_euclidean(a, b):
    x0, x1, y0, y1 = 1, 0, 0, 1  # Initialize coefficients
    while b != 0:
        q = a // b  # Quotient
        a, b = b, a % b  # Update a and b
        x0, x1 = x1, x0 - q * x1  # Update x coefficients
        y0, y1 = y1, y0 - q * y1  # Update y coefficients

    return a, x0, y0


# Function to compute the modular inverse
def modular_inverse(e, phi):
    gcd, x, _ = extended_euclidean(e, phi)
    if gcd != 1:
        raise ValueError("No modular inverse exists for the given inputs")
    return x % phi  # Ensure the result is positive


# We will run the function only once to fill the set of
# prime numbers
def primefiller():
    # Method used to fill the primes set is Sieve of
    # Eratosthenes (a method to collect prime numbers)
    max_primes = 1024
    seive = [True] * max_primes
    seive[0] = False
    seive[1] = False
    for i in range(2, max_primes):
        for j in range(i * 2, max_primes, i):
            seive[j] = False

    # Filling the prime numbers
    for i in range(len(seive)):
        if seive[i]:
            prime.append(i)


# Picking a random prime number and erasing that prime
# number from list because p!=q
def pickrandomprime():
    global prime

    ret = random.choice(prime)
    prime.remove(ret)
    return ret


def find_e(phi):
    # Commonly used secure value for e
    common_e = 2 ** 16 + 1 # 65537
    if math.gcd(common_e, phi) == 1:
        return common_e
    
    # Randomized selection of e
    while True:
        e = random.randrange(3, phi, 2)  # Random odd number between 3 and phi
        if math.gcd(e, phi) == 1:
            return e


def setkeys():
    global public_key, private_key, n
    prime1 = pickrandomprime()  # First prime number
    prime2 = pickrandomprime()  # Second prime number

    n = prime1 * prime2
    phi = (prime1 - 1) * (prime2 - 1)

    e = find_e(phi)

    # d = (k*Φ(n) + 1) / e for some integer k
    public_key = e

    private_key = modular_inverse(e, phi)


# To encrypt the given number
def encrypt(message):
    global public_key, n
    e = public_key
    encrypted_text = 1
    while e > 0:
        encrypted_text *= message
        encrypted_text %= n
        e -= 1
    return encrypted_text


# To decrypt the given number
def decrypt(encrypted_text):
    global private_key, n
    d = private_key
    decrypted = 1
    while d > 0:
        decrypted *= encrypted_text
        decrypted %= n
        d -= 1
    return decrypted


# First converting each character to its ASCII value and
# then encoding it then decoding the number to get the
# ASCII and converting it to character
def encoder(message):
    encoded = []
    # Calling the encrypting function in encoding function
    for letter in message:
        encoded.append(encrypt(ord(letter)))
    return encoded


def decoder(encoded):
    s = ''
    # Calling the decrypting function decoding function
    for num in encoded:
        s += chr(decrypt(num))
    return s


if __name__ == '__main__':
    primefiller()
    setkeys()
    message = input("Message: ")
    # Uncomment below for manual input
    # message = input("Enter the message\n")
    # Calling the encoding function
    coded = encoder(message)

    print("Initial message:")
    print(message)
    print("The encoded message(encrypted by public key)")
    print('.'.join(f"{p:x}" for p in coded))
    print('.'.join(str(p) for p in coded))
    print("The decoded message(decrypted by public key)")
    print(''.join(str(p) for p in decoder(coded)))
