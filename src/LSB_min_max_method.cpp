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
#include <climits>

using namespace cimg_library;


namespace steg {

    static int find_max_location(CImg<unsigned char> &image, int height);

    static int find_min_location(CImg<unsigned char> &image, int height);

    static std::string generic_min_max_decode(std::string name,
                                              const std::function<int(CImg<unsigned char> &, int)> &f);

    static void generic_min_max_encode(std::string name,
                                       std::string message,
                                       const std::function<int(CImg<unsigned char> &, int)> &f);

    void encode_length(uint64_t msg_length,
                       CImg<unsigned char> &image,
                       const std::function<int(CImg<unsigned char> &, int)> &f);

    uint64_t decode_length(CImg<unsigned char> &image,
                           const std::function<int(CImg<unsigned char> &, int)> &f);

    void encode_single_byte(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            const std::function<int(CImg<unsigned char> &, int)> &f,
            int64_t height);

    char decode_single_byte(CImg<unsigned char> &image,
                            const std::function<int(CImg<unsigned char> &, int)> &f,
                            int64_t height);


    void encode_single_byte(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            const std::function<int(CImg<unsigned char> &, int)> &f,
            int64_t height) {

        int bit, w;
        int h2 = image.height();

        // encoding the size to the picture
        for (int h = height, shift_count = BIT_TO_BYTE - 1;
             shift_count >= 0 && h < h2; h++, shift_count--) {

            bit = (to_encode >> shift_count) & 1U;
            w = f(image, h);
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
    }

    char decode_single_byte(CImg<unsigned char> &image,
                            const std::function<int(CImg<unsigned char> &, int)> &f,
                            int64_t height) {
        int bit = 0, w;
        uint8_t to_decode = 0;

        for (int h = height; h < height + BIT_TO_BYTE; h++) {
            w = f(image, h);
            bit = image(w, h, BLUE) & 1U;
            to_decode <<= 1;
            to_decode ^= (-bit ^ to_decode) & 1U;
        }

        return to_decode;
    }


    void encode_length(uint64_t msg_length,
                       CImg<unsigned char> &image,
                       const std::function<int(CImg<unsigned char> &, int)> &f) {
        int bit, w;
        int height = image.height();

        // encoding the size to the picture
        for (int h = 0, j = ENCODE_SIZE - 1;
             h < height && j >= 0; h++, j--) {

            bit = (msg_length >> j) & 1U; // get the bit at the position
            w = f(image, h);
            // changing blue value of the message
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
    }


    uint64_t decode_length(CImg<unsigned char> &image,
                           const std::function<int(CImg<unsigned char> &, int)> &f) {
        uint64_t msg_length = 0;
        int bit, w;

        for (int h = 0; h < ENCODE_SIZE; h++) {
            msg_length <<= 1;
            w = f(image, h);

            bit = image(w, h, BLUE) & 1U;
            msg_length ^= (-bit ^ msg_length) & 1U;
        }

        return msg_length;
    }


    static int find_max_location(CImg<unsigned char> &image, int height) {
        int max_loc = 0, max_colour = INT_MIN;
        for (int w = 0; w < image.width(); w++) {
            if (image(w, height, RED) > max_colour) {
                max_colour = image(w, height, RED);
                max_loc = w;
            }
        }
        return max_loc;
    }

    static int find_min_location(CImg<unsigned char> &image, int height) {
        int min_loc = 0, min_colour = INT_MAX;
        for (int w = 0; w < image.width(); w++) {
            if (image(w, height, RED) > min_colour) {
                min_colour = image(w, height, RED);
                min_loc = w;
            }
        }
        return min_loc;
    }


    void StegCoding::LSB_encode_max(std::string name, std::string message) {
        generic_min_max_encode(name, message, find_max_location);
    }


    std::string StegCoding::LSB_decode_max(std::string name) {
        return generic_min_max_decode(name, find_max_location);
    }


    void StegCoding::LSB_encode_min(std::string name, std::string message) {
        generic_min_max_encode(name, message, find_min_location);
    }


    std::string StegCoding::LSB_decode_min(std::string name) {
        return generic_min_max_decode(name, find_min_location);
    }

    static std::string generic_min_max_decode(std::string name,
                                              const std::function<int(CImg<unsigned char> &, int)> &f) {
        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = decode_length(src, f);
        msg_length += ENCODE_SIZE;
        std::string message = "";

        for (int h = ENCODE_SIZE; h < msg_length && h < src.width(); h += BIT_TO_BYTE) {
            message += decode_single_byte(src, f, h);
        }
        return message;
    }


    static void generic_min_max_encode(std::string name,
                                       std::string message,
                                       const std::function<int(CImg<unsigned char> &, int)> &f) {
        CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        // TODO check whether width is enough
        encode_length(msg_length, src, f);

        int64_t height = ENCODE_SIZE;
        for (char c : message) {
            encode_single_byte(c, src, f, height);
            height += BIT_TO_BYTE;
        }

        src.save(name.c_str());
    }

}