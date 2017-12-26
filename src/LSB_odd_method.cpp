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


    struct Args {
        int w;
        int h;
    };

    static std::pair<int, int> LSB_encode_length_odd_even(uint64_t msg_length,
                                                          CImg<unsigned char> &image,
                                                          const Args args);

    static uint64_t LSB_decode_length_odd_even(const CImg<unsigned char> &image,
                                               const Args args);

    static std::pair<int, int> LSB_encode_single_byte_odd(uint8_t to_encode,
                                                          CImg<unsigned char> &image,
                                                          std::pair<int, int> &coord);

    static char LSB_decode_single_byte_odd(const CImg<unsigned char> &image,
                                           const std::pair<int, int> &coord);

    void StegCoding::LSB_encode_odd(const std::string& name,
                                    const std::string& message) {
        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        assert(src.width() >= 64 && src.height() >= 64);

        // encode length of the text
        Args args{1, 0};
        auto coord = LSB_encode_length_odd_even(msg_length, src, args);

        // encoding the message
        for (char c : message) {
            coord = LSB_encode_single_byte_odd(c, src, coord);
        }

        src.save(name.c_str());
    }


    std::string StegCoding::LSB_decode_odd(const std::string& name) {
        CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length = 0;

        // decode length and translate it to bits
        Args args{1, 0};
        msg_length = LSB_decode_length_odd_even(src, args) * BIT_TO_BYTE * 2 + ENCODE_SIZE * 2 + 1;

        // decode message
        int width = src.width();
        int total_pixels = width * src.height();

        auto coord = std::make_pair(ENCODE_SIZE, 0);
        for (int nr = 1 + ENCODE_SIZE * 2; nr < total_pixels && nr < msg_length;
             nr += BIT_TO_BYTE * 2) {
            coord.first = nr % width; // width
            coord.second = nr / width; //height
            message += LSB_decode_single_byte_odd(src, coord);
        }

        return message;
    }

    void StegCoding::LSB_encode_even(const std::string& name, const std::string& message) {
        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        assert(src.width() >= 64 && src.height() >= 64);

        // encode length of the text
        Args args{0, 0};
        auto coord = LSB_encode_length_odd_even(msg_length, src, args);

        // encoding the message
        for (char c : message) {
            coord = LSB_encode_single_byte_odd(c, src, coord);
        }

        src.save(name.c_str());
    }

    std::string StegCoding::LSB_decode_even(const std::string& name) {
        CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length = 0;

        // decode length and translate it to bits
        Args args{0, 0};
        msg_length = LSB_decode_length_odd_even(src, args) * BIT_TO_BYTE * 2 + ENCODE_SIZE * 2;

        // decode message
        int width = src.width();
        int total_pixels = width * src.height();

        auto coord = std::make_pair(ENCODE_SIZE, 0);
        for (int nr = ENCODE_SIZE * 2; nr < total_pixels && nr < msg_length; nr += BIT_TO_BYTE * 2) {
            coord.first = nr % width; // width
            coord.second = nr / width; //height
            message += LSB_decode_single_byte_odd(src, coord);
        }
        return message;
    }

    static std::pair<int, int> LSB_encode_length_odd_even(uint64_t msg_length,
                                                          CImg<unsigned char> &image,
                                                          const Args args) {

        int bit, w, h, i, j;
        int width = image.width();
        int total = image.height() * width;

        // encoding the size to the picture
        for (i = args.w, j = ENCODE_SIZE - 1;
             i < total && j >= 0; i += 2, j--) {

            bit = (msg_length >> j) & 1U; // get the bit at the position

            // changing blue value of the message
            w = i % width; // width
            h = i / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }

        return std::make_pair(i % width, i / width);
    }

    static uint64_t LSB_decode_length_odd_even(const CImg<unsigned char> &image,
                                               const Args args) {
        uint64_t msg_length = 0;
        int width = image.width();
        int total = image.height() * width;
        int bit, w, h;
        for (int i = args.w, j = 0; j < ENCODE_SIZE && i < total; i += 2, j++) {
            msg_length <<= 1;

            w = i % width; // width
            h = i / width; // height

            bit = image(w, h, BLUE) & 1U;
            msg_length ^= (-bit ^ msg_length) & 1U;
        }
        return msg_length;
    }

    static std::pair<int, int> LSB_encode_single_byte_odd(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            std::pair<int, int> &coord) {
        int shift_count = BIT_TO_BYTE - 1;
        int bit;
        int w = coord.first;
        for (int h = coord.second; h < image.height(); h++) {
            for (; w < image.width(); w += 2) {
                bit = (to_encode >> shift_count) & 1U;
                shift_count--;
                image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;

                if (shift_count < 0) {
                    coord.first = w + 2;
                    coord.second = h;
                    goto end;
                }
            }
            w %= image.width();
        }
        end:
        return coord;

    }

    static char LSB_decode_single_byte_odd(const CImg<unsigned char> &image,
                                           const std::pair<int, int> &coord) {
        int bit = 0;
        uint8_t to_decode = 0;
        int w = coord.first;
        int count = 0;
        for (int h = coord.second; h < image.height(); h++) {
            for (; w < image.width() && count < BIT_TO_BYTE; w += 2, count++) {
                bit = image(w, h, BLUE) & 1U;
                to_decode <<= 1;
                to_decode ^= (-bit ^ to_decode) & 1U;
            }
            w %= image.width();
        }
        return to_decode;
    }


}