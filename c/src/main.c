#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"


#define MAX_PRIMES 1024

iMX prime_list[MAX_PRIMES] = {0};

iMX public_key = 0;
iMX private_key = 0;
iMX n = 0;


iMX gcd(iMX a, iMX b) {
    while (b != 0) {
        iMX temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


iMX modular_inverse(iMX a, iMX b) 
{
    iMX phi = b;

    // Calculate the GCD (extended euclidean)
    iMX x0 = 1;
    iMX x1 = 0;
    iMX y0 = 0;
    iMX y1 = 1;

    iMX q = 0;

    iMX temp = 0;

    while (b != 0) {
        q = a / b;

        // equals to a, b = b, a % b in Python
        temp = b;
        b = a % b;
        a = temp;

        // equals to x0, x1 = x1, x0 - q * x1 in Python
        temp = x1;
        x1 = x0 - q * x1;
        x0 = temp;

        // equals to y0, y1 = y1, y0 - q * y1 in Python
        temp = y1;
        y1 = y0 - q * y1;
        y0 = temp;
    }

    // the GCD (a.k.a. variable 'a') needs to be 1
    if (a != 1) {
        return 0;
    }

    // Else, return the modular inverse (ensuring that is positive)
    return (x0 + phi) % phi;
}


/* 
 * This function would be way more optimized if 
 * each element of the seive stored the state of
 * more than one number (like 8 if chars are used)
 */
void fill_prime_list() 
{
    u8 seive[MAX_PRIMES] = {0};
    for (iMX i = 0; i < MAX_PRIMES; i++) {
        seive[i] = 1;
    }

    seive[0] = 0;
    seive[1] = 0;

    for (iMX i = 2; i * i < MAX_PRIMES; i++) {
        if (seive[i]) {
            for (iMX j = i * i; j < MAX_PRIMES; j += i) {
                seive[j] = 0;
            }
        }
    }

    iMX last_index = 0;
    for (iMX i = 0; i < MAX_PRIMES; i++) {
        if (seive[i] != 0) {
            prime_list[last_index] = i;
            last_index++;
        }
    }
}


iMX pick_random_prime() 
{
    iMX prime_index = 0;
    iMX prime_value = 0;

    srand(time(NULL) + 3.14159265359);
    while (prime_value == 0){
        prime_index = rand() % MAX_PRIMES;
        prime_value = prime_list[prime_index];
    }
    prime_list[prime_index] = 0;

    return prime_value;
}


iMX find_e(iMX phi) 
{
    iMX e = 65537;

    while (1) {
        if (gcd(e, phi) == 1) {
            return e;
        }
        // e = random.randrange(3, phi, 2) in Python, I guess
        e = 3 + 2 * (rand() % ((phi - 3) / 2));
    }
}


void set_keys() 
{
    iMX prime1 = pick_random_prime();
    iMX prime2 = pick_random_prime();

    // This can absolutely have an overflow
    // Guess that this can improve security(?) Probably not lol
    n = prime1 * prime2;
    
    iMX phi = (prime1 - 1) * (prime2 - 1);

    iMX e = find_e(phi);

    // d = (k*Φ(n) + 1) / e for some integer k
    public_key = e;
    private_key = modular_inverse(e, phi);
}


iMX encrypt(i8 message) {
    iMX e = public_key;
    iMX encrypted_text = 1;

    while (e > 0) {
        encrypted_text *= message;
        encrypted_text %= n;
        e--;
    }

    return encrypted_text;
}


i8 decrypt(iMX encrypted_text) {
    iMX d = private_key;
    iMX decrypted_text = 1;

    while (d > 0) {
        decrypted_text *= encrypted_text;
        decrypted_text %= n;
        d--;
    }
    
    return decrypted_text;
}


iMX* encoder(const i8* message, iMX* encoded_size) {
    // Calculate the size of the input message
    *encoded_size = (iMX) strlen(message);

    // Allocate memory for the encoded array
    iMX* encoded = calloc(*encoded_size, sizeof(iMX));
    if (encoded == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    for (iMX i = 0; i < *encoded_size; i++) {
        encoded[i] = encrypt(message[i]);
    }

    return encoded;
}


i8* decoder(const iMX* encoded, size_t encoded_size) {
    // Allocate memory for the decoded string (including the null terminator)
    i8* decoded = calloc((encoded_size + 1), sizeof(i8));
    if (decoded == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    for (size_t i = 0; i < encoded_size; i++) {
        decoded[i] = decrypt(encoded[i]);
    }

    // Null-terminate the decoded string
    decoded[encoded_size] = '\0';

    return decoded;
}


i32 main() 
{
    fill_prime_list();
    set_keys();
    i8 message[] = "Hello, Papai Noel!";
    iMX message_len = 0;

    iMX* encoded = encoder(message, &message_len);

    printf("Encoded message:\n");
    for (i32 i = 0; i < message_len; i++) {
        printf("%llx.", encoded[i]);
    }
    printf("\n");

    i8* decoded = decoder(encoded, message_len);

    printf("Decoded encoded message:\n%s\n", decoded);

    free(encoded);
    free(decoded);

    return 0;
}
