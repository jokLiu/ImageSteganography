//
// Created by jokubas on 12/12/17.
//
#include <string>
#include <assert.h>
#include <iostream>
#include <functional>
#include <vector>
#include "steganography.h"
#include "CImg.h"

using namespace cimg_library;


namespace steg {


    std::vector<int64_t> primes(int64_t limit);

    void encode_length_primes(const uint64_t msg_length,
                              CImg<unsigned char> &image,
                              const std::vector<int64_t> &primes);

    uint64_t LSB_decode_length(CImg<unsigned char> &image,
                               const std::vector<int64_t> &primes);

    int64_t encode_single_byte(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            int64_t elem,
            const std::vector<int64_t> &primes);

    char LSB_decode_single_byte_odd(CImg<unsigned char> &image,
                                    int64_t elem, const std::vector<int64_t> &primes);

    void StegCoding::LSB_encode_prime(std::string name, std::string message) {
        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        assert(src.width() >= 64 &&
               src.height() >= 64 &&
               src.width() * src.height() / BIT_TO_BYTE >=
               ENCODE_SIZE + msg_length);

        auto prime_n = primes(src.width() * src.height());

        assert(prime_n.size() > msg_length * BIT_TO_BYTE + ENCODE_SIZE);

        // encode length of the text
        encode_length_primes(msg_length, src, prime_n);

        // encoding the message
        int64_t elem = ENCODE_SIZE;
        for (char c : message) {
            elem = encode_single_byte(c, src, elem, prime_n);
        }

        src.save(name.c_str());
    }


    void encode_length_primes(const uint64_t msg_length,
                              CImg<unsigned char> &image,
                              const std::vector<int64_t> &primes) {

        int bit, w, h, prime;
        int width = image.width();
        int total = image.height() * width;

        // encoding the size to the picture
        for (int i = 0, j = ENCODE_SIZE - 1;
             i < ENCODE_SIZE && j >= 0; i++, j--) {

            bit = (msg_length >> j) & 1U; // get the bit at the position

            // changing blue value of the message
            prime = primes[i];
            w = prime % width; // width
            h = prime / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
    }

    int64_t encode_single_byte(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            int64_t elem,
            const std::vector<int64_t> &primes) {
        int shift_count = BIT_TO_BYTE - 1;
        int bit, i, w, h, prime;
        int width = image.width();
        // encoding the size to the picture
        for (i = elem; shift_count >= 0; i++, shift_count--) {
            bit = (to_encode >> shift_count) & 1U;

            prime = primes[i];
            w = prime % width; // width
            h = prime / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
        return i;
    }

    char LSB_decode_single_byte_odd(CImg<unsigned char> &image,
                                    int64_t elem, const std::vector<int64_t> &primes) {
        int bit = 0, w, h, prime;
        int width = image.width();
        uint8_t to_decode = 0;

        for (int i = elem; i < elem + BIT_TO_BYTE; i++) {
            prime = primes[i];
            w = prime % width; // width
            h = prime / width; // height
            bit = image(w, h, BLUE) & 1U;
            to_decode <<= 1;
            to_decode ^= (-bit ^ to_decode) & 1U;
        }

        return to_decode;
    }


    uint64_t LSB_decode_length(CImg<unsigned char> &image,
                               const std::vector<int64_t> &primes) {

        uint64_t msg_length = 0;
        int width = image.width();
        int total = image.height() * width;
        int bit, w, h, prime;
        for (int i = 0; i < ENCODE_SIZE; i++) {
            msg_length <<= 1;

            prime = primes[i];
            w = prime % width; // width
            h = prime / width; // height

            bit = image(w, h, BLUE) & 1U;
            msg_length ^= (-bit ^ msg_length) & 1U;
        }
        return msg_length;
    }


    std::string StegCoding::LSB_decode_prime(std::string name) {
        CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length;

        auto prime_n = primes(src.width() * src.height());

        // decode length and translate it to bits
        msg_length = LSB_decode_length(src, prime_n) * BIT_TO_BYTE + ENCODE_SIZE;

        // decode message
        int height = src.height();
        int width = src.width();
        int total_pixels = width * height;

        for (int nr = ENCODE_SIZE; nr < total_pixels && nr < msg_length; nr += BIT_TO_BYTE) {
            message += LSB_decode_single_byte_odd(src, nr, prime_n);
        }
        return message;
    }

    // computing primes using the modified implementation of Sieve of Atkin
    // from http://www.geeksforgeeks.org/sieve-of-atkin/
    // more about the algorithm can be found in
    // https://en.wikipedia.org/wiki/Sieve_of_Atkin
    std::vector<int64_t> primes(int64_t limit) {

        std::vector<int64_t> primes = {2, 3, 5};

        // Initialise the sieve array with false values
        bool sieve[limit] = {false};

        /* Mark siev[n] is true if one of the following is true:
         a) n = (4*x*x)+(y*y) has odd number of solutions, i.e., there exist
            odd number of distinct pairs (x, y) that satisfy the equation and
            n % 12 = 1 or n % 12 = 5.
         b) n = (3*x*x)+(y*y) has odd number of solutions and n % 12 = 7
         c) n = (3*x*x)-(y*y) has odd number of solutions, x > y and n % 12 = 11 */
        for (int x = 1; x * x < limit; x++) {
            for (int y = 1; y * y < limit; y++) {
                // Main part of Sieve of Atkin
                int n = (4 * x * x) + (y * y);
                if (n <= limit && (n % 12 == 1 || n % 12 == 5))
                    sieve[n] ^= true;

                n = (3 * x * x) + (y * y);
                if (n <= limit && n % 12 == 7)
                    sieve[n] ^= true;

                n = (3 * x * x) - (y * y);
                if (x > y && n <= limit && n % 12 == 11)
                    sieve[n] ^= true;
            }
        }

        // Mark all multiples of squares as non-prime
        for (int r = 5; r * r < limit; r++) {
            if (sieve[r]) {
                for (int i = r * r; i < limit; i += r * r)
                    sieve[i] = false;
            }
        }

        // Print primes using sieve[]
        for (int i = 5; i < limit; i++) {
            if (sieve[i])
                primes.push_back(i);
        }

        return primes;
    }
}