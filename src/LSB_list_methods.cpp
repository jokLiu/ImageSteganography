//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Jokubas Liutkus
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

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

    std::vector<int64_t> compute_spiral_matrix(int64_t size);

    std::vector<int64_t> compute_magic_sq_matrix(int64_t size);

    void encode_length_generic(const uint64_t msg_length,
                               CImg<unsigned char> &image,
                               const std::vector<int64_t> &list);

    uint64_t decode_length_generic(CImg<unsigned char> &image,
                                   const std::vector<int64_t> &list);

    int64_t encode_single_byte(uint8_t to_encode,
                               CImg<unsigned char> &image,
                               int64_t elem,
                               const std::vector<int64_t> &list);

    char decode_single_byte(CImg<unsigned char> &image,
                            int64_t elem, const std::vector<int64_t> &list);

    static void encode_list_generic(std::string name,
                                    std::string message,
                                    const std::function<std::vector<int64_t>(int64_t)> &f);


    static std::string decode_list_generic(std::string name,
                                           const std::function<std::vector<int64_t>(int64_t)> &f);


    //*****************************************************************
    //*****************************************************************
    //*****************************************************************

    void StegCoding::LSB_encode_prime(const std::string& name, const std::string& message) {
        encode_list_generic(name, message, primes);
    }

    std::string StegCoding::LSB_decode_prime(const std::string& name) {
        return decode_list_generic(name, primes);
    }

    void StegCoding::LSB_encode_spiral(const std::string& name, const std::string& message) {
        encode_list_generic(name, message, compute_spiral_matrix);
    }

    std::string StegCoding::LSB_decode_spiral(const std::string& name) {
        return decode_list_generic(name, compute_spiral_matrix);
    }

    void StegCoding::LSB_encode_magic_sq(const std::string& name, const std::string& message) {
        encode_list_generic(name, message, compute_magic_sq_matrix);
    }

    std::string StegCoding::LSB_decode_magic_sq(const std::string& name) {
        return decode_list_generic(name, compute_magic_sq_matrix);
    }

    static std::string decode_list_generic(std::string name,
                                           const std::function<std::vector<int64_t>(int64_t)> &f) {

        CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length;

        auto prime_n = f(src.width() * src.height());

        // decode length and translate it to bits
        msg_length = decode_length_generic(src, prime_n) * BIT_TO_BYTE + ENCODE_SIZE;

        // decode message
        int height = src.height();
        int width = src.width();
        int total_pixels = width * height;

        for (int nr = ENCODE_SIZE; nr < total_pixels && nr < msg_length; nr += BIT_TO_BYTE) {
            message += decode_single_byte(src, nr, prime_n);
        }
        return message;
    }


    static void encode_list_generic(std::string name,
                                    std::string message,
                                    const std::function<std::vector<int64_t>(int64_t)> &f) {

        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        assert(src.width() >= 64 &&
               src.height() >= 64);

        auto prime_n = f(src.width() * src.height());

        // encode length of the text
        encode_length_generic(msg_length, src, prime_n);

        // encoding the message
        int64_t elem = ENCODE_SIZE;
        for (char c : message) {
            elem = encode_single_byte(c, src, elem, prime_n);
        }

        src.save(name.c_str());
    }


    void encode_length_generic(const uint64_t msg_length,
                               CImg<unsigned char> &image,
                               const std::vector<int64_t> &list) {

        int bit, w, h, elem;
        int width = image.width();

        // encoding the size to the picture
        for (int i = 0, j = ENCODE_SIZE - 1;
             i < ENCODE_SIZE && j >= 0; i++, j--) {

            bit = (msg_length >> j) & 1U; // get the bit at the position

            // changing blue value of the message
            elem = list[i];
            w = elem % width; // width
            h = elem / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
    }

    int64_t encode_single_byte(uint8_t to_encode,
                               CImg<unsigned char> &image,
                               int64_t elem,
                               const std::vector<int64_t> &list) {

        int shift_count = BIT_TO_BYTE - 1;
        int bit, i, w, h, pos;
        int width = image.width();
        // encoding the size to the picture
        for (i = elem; shift_count >= 0; i++, shift_count--) {
            bit = (to_encode >> shift_count) & 1U;

            pos = list[i];
            w = pos % width; // width
            h = pos / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
        return i;
    }

    char decode_single_byte(CImg<unsigned char> &image,
                            int64_t elem, const std::vector<int64_t> &list) {
        int bit = 0, w, h, pos;
        int width = image.width();
        uint8_t to_decode = 0;

        for (int i = elem; i < elem + BIT_TO_BYTE; i++) {
            pos = list[i];
            w = pos % width; // width
            h = pos / width; // height
            bit = image(w, h, BLUE) & 1U;
            to_decode <<= 1;
            to_decode ^= (-bit ^ to_decode) & 1U;
        }

        return to_decode;
    }


    uint64_t decode_length_generic(CImg<unsigned char> &image,
                                   const std::vector<int64_t> &list) {

        uint64_t msg_length = 0;
        int width = image.width();
        int total = image.height() * width;
        int bit, w, h, elem;
        for (int i = 0; i < ENCODE_SIZE; i++) {
            msg_length <<= 1;

            elem = list[i];
            w = elem % width; // width
            h = elem / width; // height

            bit = image(w, h, BLUE) & 1U;
            msg_length ^= (-bit ^ msg_length) & 1U;
        }
        return msg_length;
    }

    /*****************************************************************
     *
     *                     PRIME NUMBERS SECTION
     *
     *****************************************************************/

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

        // mark all multiples of squares as non-prime
        for (int r = 5; r * r < limit; r++) {
            if (sieve[r]) {
                for (int i = r * r; i < limit; i += r * r)
                    sieve[i] = false;
            }
        }

        // add primes to the list
        for (int i = 5; i < limit; i++) {
            if (sieve[i])
                primes.push_back(i);
        }

        return primes;
    }


    /*****************************************************************
     *
     *                     SPIRAL MATRIX SECTION
     *
     *****************************************************************/

    enum direction {
        RIGHT,
        DOWN,
        LEFT,
        UP
    };

    struct params {
        direction dir;
        std::pair<int, int> coord;
        int steps;
        int counter;
    };

    void compute_spiral_line(params &par, int **matrix);

    // computing a sprial matrix, and then collecting the
    // matrix values into the vector for a further use.
    //
    // Example:  
    //
    //         36 17 18 19 20 21
    //         35 16  5 6   7 22
    //         34 15  4 1   8 23
    //         33 14  3 2   9 24
    //         32 13 12 11 10 25
    //         31 30 29 28 27 26
    //
    // and the list collected will be:
    // 31 32 30 33 13 29 34 14 12 28 35 15 3 11 27 36 16 4
    // 2 10 26 17 5 1 9 25 19 7 23 20 22 21 (pattern of collecting 
    // the elements can be seen from the example)
    //
    std::vector<int64_t> compute_spiral_matrix(int64_t size) {
        const int sz = sqrt(size);

        // alloc
        int **matrix = new int *[sz];
        for (int i = 0; i < sz; i++)
            matrix[i] = new int[sz];

        std::vector<int64_t> list{};
        int x = sz >> 1, y = sz >> 1;

        int inc = 0;
        int steps = 1;
        params par{direction::RIGHT, std::make_pair(x, y), steps, 0};

        while (x >= 0 && y >= 0 && x < sz && y < sz) {
            compute_spiral_line(par, matrix);
            if (++inc == 2) {
                inc = 0;
                steps++;
            }
            par.steps = steps;
            x = par.coord.first;
            y = par.coord.second;
        }

        int row, temp;
        for (int r = sz - 1; r > -sz; --r) {
            temp = abs(r);
            row = (temp != r) ? 0 : r; //abs(r); // should be 0

            for (int c = temp - row; c < sz - row; ++c) {
                list.push_back(matrix[r + c][c]);
            }
        }

        // dealloc
        for (int i = 0; i < sz; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;

        return list;
    }


    void compute_spiral_line(params &par, int **matrix) {
        int steps = par.steps;
        int counter = par.counter;
        int x = par.coord.first, y = par.coord.second;
        int x_inc = 0, y_inc = 0;

        switch (par.dir) {
            case RIGHT:
                x_inc = 1;
                par.dir = direction::DOWN;
                break;
            case DOWN:
                y_inc = 1;
                par.dir = direction::LEFT;
                break;
            case LEFT:
                x_inc = -1;
                par.dir = direction::UP;
                break;
            case UP:
                y_inc = -1;
                par.dir = direction::RIGHT;
                break;
        }

        while (steps--) {
            matrix[x][y] = counter++;
            x += x_inc;
            y += y_inc;
        }

        par.counter = counter;
        par.coord.first = x;
        par.coord.second = y;
    }


    /*****************************************************************
    *
    *                     MAGIC SQUARE SECTION
    *
    *****************************************************************/



    // computing the magic square matrix by using the algorithm
    // from https://introcs.cs.princeton.edu/java/14array/MagicSquare.java.html
    // more about the algorithm can be found in
    // https://en.wikipedia.org/wiki/Magic_square
    std::vector<int64_t> compute_magic_sq_matrix(int64_t size) {

        int n = sqrt(size) + 1;
        n = (n % 2 == 0) ? n + 1 : n;
        std::vector<int64_t> list{};

        // A function to generate odd sized magic squares
        int magic[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                magic[i][j] = 0;

        int row = n - 1;
        int col = n / 2;
        magic[row][col] = 0;

        for (int i = 2; i <= n * n; i++) {
            if (magic[(row + 1) % n][(col + 1) % n] == 0) {
                row = (row + 1) % n;
                col = (col + 1) % n;
            } else {
                row = (row - 1 + n) % n;
            }
            magic[row][col] = i;
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                list.push_back(magic[i][j]);
            }
        }

        return list;
    }
}