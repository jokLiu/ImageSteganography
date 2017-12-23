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

	#define MAX_MATRIX 4096

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

	std::vector<int64_t> compute_spiral_matrix(int size);

	void compute_spiral_line(params& par, int **matrix);

	void encode_length_spiral(const uint64_t msg_length,
                              CImg<unsigned char> &image,
                              const std::vector<int64_t> &spiral_n);

	int64_t encode_single_byte_spiral(
            uint8_t to_encode,
            CImg<unsigned char> &image,
            int64_t elem,
            const std::vector<int64_t> &spiral_n);

	uint64_t decode_length_spiral(CImg<unsigned char> &image,
                               const std::vector<int64_t> &spiral_n);


	char LSB_decode_single_byte_spiral(CImg<unsigned char> &image,
                                    int64_t elem, const std::vector<int64_t> &spiral_n);

	void StegCoding::LSB_encode_spiral(std::string name, std::string message) {
		CImg<unsigned char> src(name.c_str());
        uint64_t msg_length = message.length();

        // assert(src.width() >= 64 &&
        //        src.height() >= 64 &&
        //        src.width() * src.height() / BIT_TO_BYTE >=
        //        ENCODE_SIZE + msg_length);

        auto spiral_n = compute_spiral_matrix(src.width() * src.height());


        //assert(spiral_n.size() > msg_length * BIT_TO_BYTE + ENCODE_SIZE);

        // encode length of the text
        encode_length_spiral(msg_length, src, spiral_n);

        // encoding the message
        int64_t elem = ENCODE_SIZE;
        for (char c : message) {
            elem = encode_single_byte_spiral(c, src, elem, spiral_n);
        }

        src.save(name.c_str());

	}	

	void encode_length_spiral(const uint64_t msg_length,
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


    int64_t encode_single_byte_spiral(
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


	std::string StegCoding::LSB_decode_spiral(std::string name) {
		CImg<unsigned char> src(name.c_str());
        std::string message = "";
        uint64_t msg_length;

        auto spiral_n = compute_spiral_matrix(src.width() * src.height());

        // decode length and translate it to bits
        msg_length = decode_length_spiral(src, spiral_n) * BIT_TO_BYTE + ENCODE_SIZE;

        // decode message
        int height = src.height();
        int width = src.width();
        int total_pixels = width * height;

        for (int nr = ENCODE_SIZE; nr < total_pixels && nr < msg_length; nr += BIT_TO_BYTE) {
            message += LSB_decode_single_byte_spiral(src, nr, spiral_n);
        }
        return message;
	}



	char LSB_decode_single_byte_spiral(CImg<unsigned char> &image,
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


    uint64_t decode_length_spiral(CImg<unsigned char> &image,
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


	std::vector<int64_t> compute_spiral_matrix(const int size){
		const int sz = sqrt(size);

		// alloc
		int **matrix = new int*[sz];
		for(int i = 0; i< sz; i++)
			matrix[i] = new int[sz];

		std::vector<int64_t> list {};
		int x = sz >> 1, y= sz >> 1 ;

		int inc = 0;
		int steps = 1;
		params par {direction::RIGHT, std::make_pair(x,y), steps, 0};

		while(x >= 0 && y >= 0 && x < sz && y < sz){
			compute_spiral_line(par, matrix);
			if(++inc == 2){
				inc = 0;
				steps++;
			}
			par.steps = steps;
			x = par.coord.first;
			y = par.coord.second;
		} 

		int row, temp;
		for(int r = sz-1; r>-sz ; --r){
			temp = abs(r);
			row = (temp != r) ? 0 : r; //abs(r); // should be 0

			for(int c = temp - row; c<sz-row ; ++c)
			{
				list.push_back(matrix[r+c][c]);
			}
		}

		// dealloc
		for(int i = 0; i < sz; i++) {
    		delete [] matrix[i];
		}
		delete [] matrix;

		return list;
	}



	void compute_spiral_line(params& par, int **matrix)
	{
		int steps = par.steps;
		int counter = par.counter;
		int x = par.coord.first, y = par.coord.second;
		int x_inc=0, y_inc=0;

		switch(par.dir){
			case RIGHT:
				x_inc=1;
				par.dir = direction::DOWN;
				break;
			case DOWN:
				y_inc=1;
				par.dir = direction::LEFT;
				break;
			case LEFT:
				x_inc=-1;
				par.dir = direction::UP;
				break;
			case UP:
				y_inc=-1;
				par.dir = direction::RIGHT;
				break;
		}

		while(steps--){
			matrix[x][y] = counter++;
			x+=x_inc;
			y+=y_inc;
		} 

		par.counter = counter;
		par.coord.first = x ;//- x_inc;
		par.coord.second = y ;//- y_inc;
	}


}