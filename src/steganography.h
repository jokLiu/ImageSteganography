//
// Created by jokubas on 11/12/17.
//

#ifndef IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H
#define IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H

#include <string>

namespace steg {

    #define ENCODE_SIZE 64
    #define BIT_TO_BYTE 8

    class StegCoding {
    public:
        // Encodes the image of the given name (and path to the image)
        // using the Least Significant Bit (LSB) method where each pixel
        // in the image is modified and it's least significant bit is
        // replaced with the the one of the given message which has to
        // be hidden.
        // The example of the possible encoding:
        // Given grey image with the values between 0 to 255:
        //
        //          200 198 0
        //          100 15  10
        //           99 12  5
        //
        // We want to encode the letter L which is 76 in decimal
        // this yields 01001100 in binary.
        //
        // We are going to encode each bit starting from the top left
        // and working our way to the right, and then down the rows
        // i.e 200 -> 198 -> 0 -> 100 -> 15 -> 10 -> 99 -> 12 -> 5
        //
        // First we write our picture's pixels in binary format:
        //
        //       11001000  11000110  00000000
        //       01100100  00001111  00001010
        //       01100011  00001100  00000101
        //
        // Now we are going to encrypt our message [0]1001100
        // First bit in the message is 0 and the first pixel is
        // 11001000 we replace the LSB of the pixel this yields the
        // 1100100[0] pixel with encoded bit in the brackets
        //
        // replacing the second bit in the message 0[1]001100
        // this yields the second pixel 11000110 to become:
        // 1100011[1], and so on, until we get the image:
        //
        //       11001000  11000110  00000000
        //       01100100  00001111  00001010
        //       01100011  00001100  00000101
        //
        static void LSB_encode(std::string name, std::string message);


        static std::string LSB_decode(std::string name);
    };
}


#endif //IMAGE_STEGANOGRPAHY_STEGANOGRAPHY_H
