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

#ifndef IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H
#define IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H

#include <string>

namespace steg {

#define ENCODE_SIZE 64
#define BIT_TO_BYTE 8
#define RED 0
#define GREEN 1
#define BLUE 2

    class StegCoding {
    public:

        /************************************************
         * Encodes the message into the image which name is passed to 
         * the function using the Least Significant Bit (LSB) method 
         * where each pixel in the image is modified and it's least 
         * significant bit is replaced with the the one of the given 
         * message which has to be hidden. Then it replaces the actual
         * image with the new stego image with the message hidden in it.
         *
         * Image to encode has to be of the "png" file format, and
         * all the data is at the moment hidden in the BLUE pixels
         * as it is the least sensitive to the human eye.
         *
         * First of all the length of the message is encoded into first
         * 64 pixels, this provides the max message size of 2^64 which
         * is very unlikely to be exceeded (heh).
         *
         * At the moment image has to be of at least 64x64 size.
         *
         **************************************************
         *
         * The example of the possible encoding:
         * Given the image with the BLUE values between 0 to 255:
         *
         *          200 198 0
         *          100 15  10
         *           99 12  5
         *
         * We want to encode the letter L which is 76 in ASCII
         * this yields 01001100 in binary.
         *
         * We are going to encode each bit starting from the top left
         * and working our way to the right, and then down the rows
         * i.e 200 -> 198 -> 0 -> 100 -> 15 -> 10 -> 99 -> 12 -> 5
         *
         * First we write our picture's pixels in binary format:
         *
         *       11001000  11000110  00000000
         *       01100100  00001111  00001010
         *       01100011  00001100  00000101
         *
         * Now we are going to encrypt our message bit by bit:
         *
         *                   [0]1001100
         *
         * First bit in the message is [0] and the first pixel is
         * 11001000 we replace the LSB of the pixel this yields the
         * 1100100[0] pixel with encoded bit in the brackets
         *
         * replacing the second bit in the message 0[1]001100
         * yields the second pixel 11000110 to become:
         * 1100011[1], and so on, until we get the image:
         *
         *       1100100[0]  1100011[1]  0000000[0]
         *       0110010[0]  0000111[1]  0000101[1]
         *       0110001[0]  0000110[0]  00000101
         ***********************************************/
        static void LSB_encode(const std::string &name, const std::string &message);


        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the Least Significant Bit (LSB) method.
         * This is the opposite of LSB_encode, and the message encoded 
         * with LSB_encoded can be retrieved using this LSB_decode method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode(const std::string &name);


        /************************************************
         * Encodes the message into the image which name is passed to 
         * the function using the LSB method with each bit encoded in
         * every odd location of the image provided where every odd
         * pixel in the image is modified and it's least 
         * significant bit is replaced with the the one of the given 
         * message which has to be hidden. Then it replaces the actual
         * image with the new stego image with the message hidden in it.
         *
         * Image to encode has to be of the "png" file format, and
         * all the data is at the moment hidden in the BLUE pixels
         * as it is the least sensitive to the human eye.
         *
         * First of all the length of the message is encoded into first
         * 64 odd pixels of the image, this provides the max message 
         * size of 2^64 which is very unlikely to be exceeded (heh).
         *
         * At the moment image has to be of at least 64x64 size.
         * 
         **************************************************
         *
         * The example of the possible encoding:
         * Given the image with the BLUE values between 0 to 255:
         *
         *          200 198 0
         *          100 15  10
         *           99 12  5
         *
         * We want to encode the letter L which is 76 in ASCII
         * this yields 01001100 in binary.
         *
         * We are going to encode each odd bit starting from the top left
         * and working our way to the right, and then down the rows
         * i.e. 198 ->  100 ->  10 ->  12 -> .... (counting from 0)
         *
         *
         * First we write our picture's pixels in binary format:
         *
         *        11001000  [11000110]  00000000
         *       [01100100]  00001111  [00001010]
         *        01100011  [00001100]  00000101
         *
         * Now we are going to encrypt our message bit by bit:
         *
         *                   [0]1001100
         *
         * First bit in the message is [0] and the first odd pixel is
         * 11000110 we replace the LSB of the pixel this yields the
         * 1100011[0] pixel with encoded bit in the brackets
         *
         * replacing the second odd pixel [01100100] in the 
         * message 0[1]001100 yields the second pixel 1[1]000110 to become:
         * 0110010[1], and so on, until we get the image:
         *
         *        11001000  1100011[0]  00000000
         *       0110010[1]  00001111  0000101[0]
         *        01100011  0000110[0]  00000101
         ***********************************************/
        static void LSB_encode_odd(const std::string name, const std::string message);


        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB odd method.
         * This is the opposite of LSB_encode_odd, and the message encoded 
         * with LSB_encode_odd can be retrieved using this LSB_decode_odd method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_odd(const std::string name);


        /************************************************
         * Encodes the message into the image which name is passed to 
         * the function using the LSB method with each bit encoded in
         * every even location of the image provided where every even
         * pixel in the image is modified and it's least 
         * significant bit is replaced with the the one of the given 
         * message which has to be hidden. Then it replaces the actual
         * image with the new stego image with the message hidden in it.
         *
         * Image to encode has to be of the "png" file format, and
         * all the data is at the moment hidden in the BLUE pixels
         * as it is the least sensitive to the human eye.
         *
         * First of all the length of the message is encoded into first
         * 64 even pixels of the image, this provides the max message 
         * size of 2^64 which is very unlikely to be exceeded (heh).
         *
         * At the moment image has to be of at least 64x64 size.
         * 
         **************************************************
         *
         * The example of the possible encoding:
         * Given the image with the BLUE values between 0 to 255:
         *
         *          200 198 0
         *          100 15  10
         *           99 12  5
         *
         * We want to encode the letter L which is 76 in ASCII
         * this yields 01001100 in binary.
         *
         * We are going to encode each even bit starting from the top left
         * and working our way to the right, and then down the rows
         * i.e. 200 -> 0 -> 15 -> 99 -> 5 -> .... (counting from 0)
         *
         * First we write our picture's pixels in binary format:
         *
         *       [11001000]  11000110  [00000000]
         *        01100100  [00001111]  00001010
         *       [01100011]  00001100  [00000101]
         *
         * Now we are going to encrypt our message bit by bit:
         *
         *                   [0]1001100
         *
         * First bit in the message is [0] and the first even pixel is
         * 11001000 we replace the LSB of the pixel this yields the
         * 1100100[0] pixel with encoded bit in the brackets
         *
         * replacing the second even pixel [00000000] in the 
         * message 0[1]001100 yields the second pixel 0000000[0] to become:
         * 00000000[1], and so on, until we get the image:
         *
         *       1100100[0]  11000110  0000000[1]
         *        01100100  0000111[0]  00001010
         *       0110001[0]  00001100  0000010[1]
         ***********************************************/
        static void LSB_encode_even(const std::string name, const std::string message);


        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB even method.
         * This is the opposite of LSB_encode_even, and the message encoded 
         * with LSB_encode_even can be retrieved using this LSB_decode_even method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_even(const std::string name);

        /************************************************
        * Encodes the message into the image which name is passed to
        * the function using the LSB max method where the largest pixel in a row
        * (i.e. the pixel with the largest RED value in each row) in the image
        * is modified and it's least significant bit is replaced with the the
        * one of the given message which has to be hidden.
        * Then it replaces the actual image with the new stego image
        * with the message hidden in it.
        *
        * Image to encode has to be of the "png" file format, and
        * all the data is at the moment hidden in the BLUE pixels
        * as it is the least sensitive to the human eye.
        *
        * First of all the length of the message is encoded into first
        * 64 rows, this provides the max message size of 2^64 which
        * is very unlikely to be exceeded (heh).
        *
        * At the moment image has to be of at least 64x64 size.
        *
        **************************************************
        *
        * The example of the possible encoding:
        * Given the image with the BLUE/RED values between 0 to 255:
        *
        *          [200/10] [198/78] [0/101]
        *          [100/1]  [15/3]   [10/8]
        *          [99/155] [12/250] [5/71]
        *
        * We want to encode the letter L which is 76 in ASCII
        * this yields 01001100 in binary.
        *
        * We are going to encode each bit starting from the top row
        * and working our way to the bottom
        * i.e 1. [0/178] ( RED 101 largest out of 10, 78, 101 in the first row)
        *     2. [10/8] (RED 8 largest out of 1, 3, 8 in the second row)
        *     3. [12/250] (RED 250 largest out of 155, 250, 71 in the third row)
        *
        * First we write our BLUE picture's pixels in binary format:
        *
        *       11001000  11000110  00000000
        *       01100100  00001111  00001010
        *       01100011  00001100  00000101
        *
        * Now we are going to encrypt our message bit by bit:
        *
        *                   [0]1001100
        *
        * First bit in the message is [0] and the largest RED pixel is [0/101]
        * therefore BLUE pixel 0 (00000000) will be encoded.
        * We replace the LSB of the pixel and this yields the
        * 0000000[0] pixel with encoded bit in the brackets
        *
        * replacing the second bit in the message 0[1]001100
        * yields the largest pixel in row two [10/8] (BLUE 00001010)
        * to become 0000101[1], and so on, until we get the image:
        *
        *       11001000  11000110    0000000[0]
        *       01100100  00001111    0000101[1]
        *       01100011  0000110[0]  00000101
        ***********************************************/
        static void LSB_encode_max(const std::string name, const std::string message);


        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB max method.
         * This is the opposite of LSB_encode_max, and the message encoded 
         * with LSB_encode_max can be retrieved using this LSB_decode_max method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB_max method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_max(const std::string name);

        /************************************************
         * Encodes the message into the image which name is passed to 
         * the function using the LSB min method where the smallest pixel in a row
         * (i.e. the pixel with the smallest RED value in each row) in the image
         * is modified and it's least significant bit is replaced with the the
         * one of the given message which has to be hidden. 
         * Then it replaces the actual image with the new stego image 
         * with the message hidden in it.
         *
         * Image to encode has to be of the "png" file format, and
         * all the data is at the moment hidden in the BLUE pixels
         * as it is the least sensitive to the human eye.
         *
         * First of all the length of the message is encoded into first
         * 64 rows, this provides the max message size of 2^64 which
         * is very unlikely to be exceeded (heh).
         *
         * At the moment image has to be of at least 64x64 size.
         *
         **************************************************
         *
         * The example of the possible encoding:
         * Given the image with the BLUE/RED values between 0 to 255:
         *
         *          [200/10] [198/78] [0/101]
         *          [100/1]  [15/3]   [10/8]
         *          [99/155] [12/250] [5/71]
         *
         * We want to encode the letter L which is 76 in ASCII
         * this yields 01001100 in binary.
         *
         * We are going to encode each bit starting from the top row
         * and working our way to the bottom
         * i.e 1. [200/10] ( RED 10 smallest out of 10, 78, 101 in the first row)
         *     2. [100/1] (RED 1 largest out of 1, 3, 8 in the second row)
         *     3. [5/71] (RED 71 largest out of 155, 250, 71 in the third row)
         *
         * First we write our BLUE picture's pixels in binary format:
         *
         *       11001000  11000110  00000000
         *       01100100  00001111  00001010
         *       01100011  00001100  00000101
         *
         * Now we are going to encrypt our message bit by bit:
         *
         *                   [0]1001100
         *
         * First bit in the message is [0] and the smallest RED pixel is [200/10]
         * therefore BLUE pixel 200 (11001000) will be encoded.
         * We replace the LSB of the pixel and this yields the
         * 1100100[0] pixel with encoded bit in the brackets
         *
         * replacing the second bit in the message 0[1]001100
         * yields the smallest pixel in row two [100/1] (BLUE 01100100)
         * to become 0110010[1], and so on, until we get the image:
         *
         *       1100100[0]  11000110  00000000
         *       0110010[1]  00001111  00001010
         *       01100011    00001100  0000010[0]
         ***********************************************/
        static void LSB_encode_min(const std::string name, const std::string message);

        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB min method.
         * This is the opposite of LSB_encode_min, and the message encoded 
         * with LSB_encode_min can be retrieved using this LSB_decode_max method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB_min method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_min(std::string name);

        /************************************************
        * Encodes the message into the image which name is passed to
        * the function using the LSB prime method where each bit of the message
        * is encoded into every prime pixel of the image.
        *
        * Image to encode has to be of the "png" file format, and
        * all the data is at the moment hidden in the BLUE pixels
        * as it is the least sensitive to the human eye.
        *
        * First of all the length of the message is encoded into first
        * 64 locations, this provides the max message size of 2^64 which
        * is very unlikely to be exceeded (heh).
        *
        * At the moment image has to be of at least 64x64 size.
        *
        **************************************************
        *
        * The example of the possible encoding:
        * Given the image with the BLUE values between 0 to 255:
        *
        *          200 198 0
        *          100 15  10
        *           99 12  5
        *
        * We want to encode the letter L which is 76 in ASCII
        * this yields 01001100 in binary.
        *
        * We are going to encode each bit in each primary located pixel
        * starting from top left and working our way to the right and then
        * down the rows.
        *
        * First primary numbers are 2, 3, 5, 7 ,...
        *
        * First we write our BLUE picture's pixels in binary format:
        *
        *       11001000  11000110  00000000
        *       01100100  00001111  00001010
        *       01100011  00001100  00000101
        *
        * Now we are going to encrypt our message bit by bit:
        *
        *                   [0]1001100
        *
        * First bit in the message is [0] and the first primary number is
        * 2 therefore pixel 0 (00000000) will be encoded.
        * We replace the LSB of the pixel and this yields the
        * 0000000[0] pixel with encoded bit in the brackets
        *
        * replacing the second bit in the message 0[1]001100
        * yields the second prime number located pixel 3 (i.e. 100)
        * to become  0110010[1], and so on, until we get the image:
        *
        *       11001000    11000110  0000000[0]
        *       0110010[1]  00001111  0000101[0]
        *       01100011    0000110[0]  00000101
        *
        * location 2, 3, 5, 7 were encoded (starting from 0)
        ***********************************************/
        static void LSB_encode_prime(std::string name, std::string message);


        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB prime method.
         * This is the opposite of LSB_encode_prime, and the message encoded 
         * with LSB_encode_prime can be retrieved using this LSB_decode_prime method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB_prime method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_prime(std::string name);

        /************************************************
        * Encodes the message into the image which name is passed to
        * the function using the LSB spiral matrix method where each
        * bit of the message is encoded into every pixel produced by
        * spiral matrix.
        *
        * Image to encode has to be of the "png" file format, and
        * all the data is at the moment hidden in the BLUE pixels
        * as it is the least sensitive to the human eye.
        *
        * First of all the length of the message is encoded into first
        * 64 locations, this provides the max message size of 2^64 which
        * is very unlikely to be exceeded (heh).
        *
        * At the moment image has to be of at least 64x64 size.
        *
        **************************************************
        *
        * The example of the possible encoding:
        * Given the image with the BLUE values between 0 to 255:
        *
        *          200 198 0
        *          100 15  10
        *           99 12  5
        *
        * We want to encode the letter L which is 76 in ASCII
        * this yields 01001100 in binary.
        *
        * We are going to encode each bit in each spiral matrix located pixel
        * starting from top left and working our way to the right and then
        * down the rows.
        *
        * First we generate our spiral matrix i.e.
        *
        *        4 5 6
        *        3 0 7
        *        2 1 8
        *
        * and the list collected will be:
        * 2 3 1 4 0 8 5 7 6 (pattern of collecting
        * the elements can be seen from the example)
        * More about the method can be found in the implementation source file.
        *
        * So our first bit will go into location 2, second to 3,
        * third into 1 and so on.
        *
        *
        * We write our BLUE picture's pixels in binary format:
        *
        *       11001000  11000110  00000000
        *       01100100  00001111  00001010
        *       01100011  00001100  00000101
        *
        * Now we are going to encrypt our message bit by bit:
        *
        *                   [0]1001100
        *
        * First bit in the message is [0] and the first location is
        * 2 therefore pixel 0 (00000000) will be encoded.
        * We replace the LSB of the pixel and this yields the
        * 0000000[0] pixel with encoded bit in the brackets
        *
        * replacing the second bit in the message 0[1]001100
        * yields the pixel in location 3 (i.e. 100)
        * to become  0110010[1], and so on, until we get the image:
        *
        *       1100100[1]  1100011[0]  0000000[0]
        *       0110010[1]  0000111[0]  0000101[0]
        *       01100011    0000110[0]  0000010[1]
        *
        ***********************************************/
        static void LSB_encode_spiral(std::string name, std::string message);

        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB spiral method.
         * This is the opposite of LSB_encode_spiral, and the message encoded 
         * with LSB_encode_spiral can be retrieved using this LSB_decode_spiral method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB_spiral method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_spiral(std::string name);


        /************************************************
        * Encodes the message into the image which name is passed to
        * the function using the LSB magic square method where each
        * bit of the message is encoded into every pixel produced by
        * magic square.
        *
        * Image to encode has to be of the "png" file format, and
        * all the data is at the moment hidden in the BLUE pixels
        * as it is the least sensitive to the human eye.
        *
        * First of all the length of the message is encoded into first
        * 64 locations, this provides the max message size of 2^64 which
        * is very unlikely to be exceeded (heh).
        *
        * At the moment image has to be of at least 64x64 size.
        *
        **************************************************
        *
        * The example of the possible encoding:
        * Given the image with the BLUE values between 0 to 255:
        *
        *          200 198 0
        *          100 15  10
        *           99 12  5
        *
        * We want to encode the letter L which is 76 in ASCII
        * this yields 01001100 in binary.
        *
        * We are going to encode each bit in each magic square pixel
        * starting from top left and working our way to the right and then
        * down the rows.
        *
        * First we generate our spiral matrix i.e.
        *
        *   4  9  2 
        *   3  5  7 
        *   8  1  6 
        *
        * and the list collected will be:
        * 4 9 2 3 5 7 8 1 6 (pattern of collecting
        * the elements can be seen from the example)
        * More about the method can be found in the implementation source file.
        *
        * So our first bit will go into location 4, second to 9,
        * third into 2 and so on.
        *
        *
        * We write our BLUE picture's pixels in binary format:
        *
        *       11001000  11000110  00000000
        *       01100100  00001111  00001010
        *       01100011  00001100  00000101
        *
        * Now we are going to encrypt our message bit by bit:
        *
        *                   [0]1001100
        *
        * First bit in the message is [0] and the first location is
        * 4 therefore pixel 15 (00001111) will be encoded.
        * We replace the LSB of the pixel and this yields the
        * 0000111[0] pixel with encoded bit in the brackets
        *
        * replacing the second bit in the message 0[1]001100
        * yields the pixel in location 9 (there is not location 9 in our example)
        * to be modified, and so on, until we get the image:
        *
        *       11001000    1100011[0]  0000000[0]
        *       0110010[0]  0000111[0]  0000101[1]
        *       01100011    0000110[1]  0000010[0]
        *
        ***********************************************/
        static void LSB_encode_magic_sq(std::string name, std::string message);


        /************************************************
         * Decodes the message from the image which name is passed to 
         * the function using the LSB magic square.
         * This is the opposite of LSB_encode_magic_sq, and the message encoded 
         * with LSB_encode_magic_sq can be retrieved using this LSB_decode_magic_sq method.
         *
         * The message is retrieved from the image and returned as a string.
         * 
         * If random image is passed the are no checks done whether the
         * encoding was used, and therefore it is impossible to determine
         * if the LSB_magic_square method was used or not.
         *         
         ***********************************************/
        static std::string LSB_decode_magic_sq(std::string name);


    };
}


#endif //IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H
