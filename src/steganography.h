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
        static void LSB_encode(const std::string& name, const std::string& message);


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
        static std::string LSB_decode(const std::string& name);


        static void LSB_encode_odd(std::string name, std::string message);


        static std::string LSB_decode_odd(std::string name);


        static void LSB_encode_even(std::string name, std::string message);


        static std::string LSB_decode_even(std::string name);


        static void LSB_encode_prime(std::string name, std::string message);


        static std::string LSB_decode_prime(std::string name);


        static void LSB_encode_max(std::string name, std::string message);


        static std::string LSB_decode_max(std::string name);

        
        static void LSB_encode_min(std::string name, std::string message);


        static std::string LSB_decode_min(std::string name);


        static void LSB_encode_spiral(std::string name, std::string message);


        static std::string LSB_decode_spiral(std::string name);


        static void LSB_encode_magic_sq(std::string name, std::string message);


        static std::string LSB_decode_magic_sq(std::string name);


    };
}


#endif //IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H
