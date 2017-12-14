//
// Created by jokubas on 12/12/17.
//
#include <string>
#include <assert.h> 
#include <iostream>
#include "steganography.h"
#include "CImg.h"
using namespace cimg_library;


namespace steg {

	#define ENCODE_SIZE 64
	#define BIT_TO_BYTE 8

	void enocde_length(int64_t msg_length, 
    	CImg<unsigned char> image);

	std::pair<int,int> encode_single_byte(uint8_t to_encode, 
    	CImg<unsigned char> image,
    	std::pair<int, int> coord);

	// first 64 pixels encodes the length of the message in bytes
    // message has to be of at least 64x64 size
    void StegCoding::LSB_encode(std::string name, std::string message){
    	CImg<unsigned char> src(/*name*/"hello.jpeg");
    	int64_t msg_length = message.length();
    	
    	assert(src.width() >= 64 && 
    		src.height() >= 64 &&
    		src.width() * src.height() / BIT_TO_BYTE >=
    		ENCODE_SIZE + msg_length);

    	// encode length of the text
    	enocde_length(msg_length, src);

    	// encoding the message
    	auto coord = std::make_pair(ENCODE_SIZE, 0);
    	for(char c : message){
			coord = encode_single_byte(c, src, coord);
    	}

    }

    void enocde_length(int64_t msg_length, 
    	CImg<unsigned char> image){

		int bit;
    	// encoding the size to the picture
    	for(int i=ENCODE_SIZE-1, j = 0; i>=0; i--, j++){
    		bit = (msg_length >> j) & 1U; // get the bit at the position

    		// changing green value of the message
    		image(0, i, 1) ^= (-bit ^ image(0, i, 1)) & 1U;
    	}

    }

    std::pair<int,int> encode_single_byte(uint8_t to_encode, 
    	CImg<unsigned char> image,
    	std::pair<int, int> coord){
    	uint8_t shift_count = BIT_TO_BYTE - 1;
    	uint8_t bit;
    	int w = coord.first;
    	for(int h = coord.second; h<image.height(); h++){
    		for(; w<image.width(); w++){
    			bit = (to_encode >> shift_count) & 1U;
    			shift_count--;
    			//std::cout << image(w, h, 1)  << '\n';
    			image(w, h, 1) ^= (-bit ^ image(w,h,1)) & 1U;	
  				//std::cout << image(w, h, 1)  << "\n\n\n";  		
    			if(shift_count < 0){
    				coord.first = w+1;
    				coord.second = h;
    				goto end;
    			}
    		}
    		w=0;
    	}
    	end:
    	return coord;

    }



}