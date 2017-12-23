//
// Created by jokubas on 12/12/17.
//
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <functional>
#include <math.h>
#include <stdlib.h>  
#include "steganography.h"
#include "CImg.h"

using namespace cimg_library;


namespace steg {

	std::vector<int64_t> compute_magic_sq_matrix(const int size);

	void encode_length_magic_sq(const uint64_t msg_length,
                              CImg<unsigned char> &image,
                              const std::vector<int64_t> &spiral_n);

	int64_t encode_single_byte_magic_sq(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            int64_t elem,
            const std::vector<int64_t> &spiral_n);

	char LSB_decode_single_byte_magic_sq(CImg<unsigned char> &image,
                                    int64_t elem, const std::vector<int64_t> &spiral_n);

	uint64_t decode_length_magic_sq(CImg<unsigned char> &image,
                               const std::vector<int64_t> &spiral_n);

	void StegCoding::LSB_encode_magic_sq(std::string name, std::string message) {
		CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        // assert(src.width() >= 64 &&
        //        src.height() >= 64 &&
        //        src.width() * src.height() / BIT_TO_BYTE >=
        //        ENCODE_SIZE + msg_length);

        auto spiral_n = compute_magic_sq_matrix(src.width() * src.height());

        //assert(spiral_n.size() > msg_length * BIT_TO_BYTE + ENCODE_SIZE);

        // encode length of the text
        encode_length_magic_sq(msg_length, src, spiral_n);

        // encoding the message
        int64_t elem = ENCODE_SIZE;
        for (char c : message) {
            elem = encode_single_byte_magic_sq(c, src, elem, spiral_n);
        }

        src.save(name.c_str());

	}


	void encode_length_magic_sq(const uint64_t msg_length,
                              CImg<unsigned char> &image,
                              const std::vector<int64_t> &spiral_n) {

        int bit, w, h, numb;
        int width = image.width();

        // encoding the size to the picture
        for (int i = 0, j = ENCODE_SIZE - 1;
             i < ENCODE_SIZE && j >= 0; i++, j--) {

            bit = (msg_length >> j) & 1U; // get the bit at the position

            // changing blue value of the message
            numb = spiral_n[i];
            w = numb % width; // width
            h = numb / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
    }


    int64_t encode_single_byte_magic_sq(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            int64_t elem,
            const std::vector<int64_t> &spiral_n) {
        int shift_count = BIT_TO_BYTE - 1;
        int bit, i, w, h, numb;
        int width = image.width();
        // encoding the size to the picture
        for (i = elem; shift_count >= 0; i++, shift_count--) {
            bit = (to_encode >> shift_count) & 1U;

            numb = spiral_n[i];
            w = numb % width; // width
            h = numb / width; // height
            image(w, h, BLUE) ^= (-bit ^ image(w, h, BLUE)) & 1U;
        }
        return i;
    }


	std::string StegCoding::LSB_decode_magic_sq(std::string name) {
		CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length;

        auto spiral_n = compute_magic_sq_matrix(src.width() * src.height());

        // decode length and translate it to bits
        msg_length = decode_length_magic_sq(src, spiral_n) * BIT_TO_BYTE + ENCODE_SIZE;

        // decode message
        int height = src.height();
        int width = src.width();
        int total_pixels = width * height;

        for (int nr = ENCODE_SIZE; nr < total_pixels && nr < msg_length; nr += BIT_TO_BYTE) {
            message += LSB_decode_single_byte_magic_sq(src, nr, spiral_n);
        }
        return message;
	}

	char LSB_decode_single_byte_magic_sq(CImg<unsigned char> &image,
                                    int64_t elem, const std::vector<int64_t> &spiral_n) {
        int bit = 0, w, h, numb;
        int width = image.width();
        uint8_t to_decode = 0;

        for (int i = elem; i < elem + BIT_TO_BYTE; i++) {
            numb = spiral_n[i];
            w = numb % width; // width
            h = numb / width; // height
            bit = image(w, h, BLUE) & 1U;
            to_decode <<= 1;
            to_decode ^= (-bit ^ to_decode) & 1U;
        }

        return to_decode;
    }


    uint64_t decode_length_magic_sq(CImg<unsigned char> &image,
                               const std::vector<int64_t> &spiral_n) {

        uint64_t msg_length = 0;
        int width = image.width();
        int total = image.height() * width;
        int bit, w, h, numb;
        for (int i = 0; i < ENCODE_SIZE; i++) {
            msg_length <<= 1;

            numb = spiral_n[i];
            w = numb % width; // width
            h = numb / width; // height

            bit = image(w, h, BLUE) & 1U;
            msg_length ^= (-bit ^ msg_length) & 1U;
        }
        return msg_length;
    }



	// computing the magic square matrix by using the algorithm
    // from https://www.geeksforgeeks.org/magic-square/
    // more about the algorithm can be found in
    // https://en.wikipedia.org/wiki/Magic_square
	std::vector<int64_t> compute_magic_sq_matrix(const int size){
	
	int n = sqrt(size)+1;
	n = (n%2==0) ? n+1 : n;
	std::vector<int64_t> list {};

	// A function to generate odd sized magic squares
    int magicSquare[n][n];
    for(int i = 0 ; i < n; i++)
    	for(int j = 0; j< n; j++)
    		magicSquare[i][j] = 0;
    // Initialize position for 1
    int i = n>>1;
    int j = n-1;
 
    // One by one put all values in magic square
    for (int num=1; num <= n*n; )
    {
        if (i==-1 && j==n) //3rd condition
        {
            j = n-2;
            i = 0;
        }
        else
        {
            //1st condition helper if next number 
            // goes to out of square's right side
            if (j == n)
                j = 0;

            //1st condition helper if next number 
            // is goes to out of square's upper side
            if (i < 0)
                i=n-1;
        }
        if (magicSquare[i][j]) //2nd condition
        {
            j -= 2;
            i++;
            continue;
        }
        magicSquare[i][j] = num++; //set number
 
        j++; i--; //1st condition
    }
 
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
        	list.push_back(magicSquare[i][j]);
        }
    }
    return list;

	}


}
