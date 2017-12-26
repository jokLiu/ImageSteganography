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
#include "steganography.h"
#include "CImg.h"

using namespace cimg_library;


namespace steg {


    static char LSB_decode_single_byte(CImg<unsigned char> &image,
                                       std::pair<int, int> &coord);

    static uint64_t LSB_decode_length(CImg<unsigned char> &image);

    static void LSB_enocde_length(const uint64_t msg_length,
                                  CImg<unsigned char> &image);

    static std::pair<int, int> LSB_encode_single_byte(uint8_t to_encode,
                                                      CImg<unsigned char> &image,
                                                      std::pair<int, int> &coord);

    void StegCoding::LSB_encode(const std::string &name, const std::string &message) {
        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        assert(src.width() >= 64 &&  // width and height of correct size
               src.height() >= 64);

        // encode length of the text
        LSB_enocde_length(msg_length, src);

        // encoding the actual message
        auto coord = std::make_pair(ENCODE_SIZE, 0);
        for (char c : message) {
            coord = LSB_encode_single_byte(c, src, coord);
        }

        // save the encoded image
        src.save(name.c_str());
    }

    std::string StegCoding::LSB_decode(const std::string &name) {
        CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length = 0;

        // decode length and translate it to bits
        msg_length = LSB_decode_length(src) * BIT_TO_BYTE
                     + ENCODE_SIZE;

        // decode message
        int width = src.width();
        int total_pixels = width * src.height();
        // coordinates to pass into the decode single byte
        auto coord = std::make_pair(ENCODE_SIZE, 0);
        for (int nr = ENCODE_SIZE; nr < total_pixels && nr < msg_length;
             nr += BIT_TO_BYTE) {
            // determine the the pixel coordinates
            coord.first = nr % width; // width
            coord.second = nr / width; //height
            message += LSB_decode_single_byte(src, coord);

        }
        return message;
    }

    static char LSB_decode_single_byte(CImg<unsigned char> &image,
                                       std::pair<int, int> &coord) {
        // bit to be retrieved from a single pixel
        int bit = 0;
        // single byte to decode
        uint8_t to_decode = 0;
        // width pixel
        int w = coord.first;
        int count = 0;
        for (int h = coord.second; h < image.height(); h++) {
            for (; w < image.width() && count < BIT_TO_BYTE; w++, count++) {
                // retrieve a single bit
                bit = image(w, h, BLUE) & 1U;
                // update the byte
                to_decode <<= 1;
                to_decode ^= (-bit ^ to_decode) & 1U;
            }
            w = 0;
        }
        return to_decode;
    }

    static uint64_t LSB_decode_length(CImg<unsigned char> &image) {
        uint64_t msg_length = 0;
        int bit;
        for (int i = 0; i < ENCODE_SIZE; i++) {
            msg_length <<= 1;
            // get the bit at the specific pixel
            bit = image(i, 0, BLUE) & 1U;

            // update the length
            msg_length ^= (-bit ^ msg_length) & 1U;
        }
        return msg_length;

    }


    static void LSB_enocde_length(const uint64_t msg_length,
                                  CImg<unsigned char> &image) {

        int bit;
        // encoding the size to the picture
        for (int i = ENCODE_SIZE - 1, j = 0; i >= 0; i--, j++) {
            // get the bit at the position
            bit = (msg_length >> j) & 1U;

            // changing blue value of the message
            image(i, 0, BLUE) ^= (-bit ^ image(i, 0, BLUE)) & 1U;
        }
    }

    static std::pair<int, int> LSB_encode_single_byte(uint8_t to_encode,
                                                      CImg<unsigned char> &image,
                                                      std::pair<int, int> &coord) {
        int shift_count = BIT_TO_BYTE - 1;
        int bit;
        int w = coord.first;
        for (int h = coord.second; h < image.height(); h++) {
            for (; w < image.width(); w++) {
                bit = (to_encode >> shift_count) & 1U;
                shift_count--;
                image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;

                if (shift_count < 0) {
                    coord.first = w + 1;
                    coord.second = h;
                    goto end;
                }
            }
            w = 0;
        }
        end:
        return coord;

    }


}