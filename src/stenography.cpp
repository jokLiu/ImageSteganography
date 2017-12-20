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

	void enocde_length(uint64_t msg_length, 
    	CImg<unsigned char>& image);

	uint64_t decode_length(CImg<unsigned char>& image);

	std::pair<int,int> encode_single_byte(uint8_t to_encode, 
    	CImg<unsigned char>& image,
    	std::pair<int, int>& coord);

	char decode_single_byte(CImg<unsigned char>& image,
    	std::pair<int, int>& coord);

	// first 64 pixels encodes the length of the message in bytes
    // message has to be of at least 64x64 size
    void StegCoding::LSB_encode(std::string name, std::string message){
    	CImg<unsigned char> src(name.c_str());
    	uint64_t msg_length = message.length();
    	
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
    	
    	src.save(name.c_str());

    }

    std::string StegCoding::LSB_decode(std::string name){
    	CImg<unsigned char> src(name.c_str());
    	std::string message = "";
    	uint64_t msg_length = 0;

    	// decode length and translate it to bits
    	msg_length = decode_length(src) * BIT_TO_BYTE + ENCODE_SIZE;

    	// decode message
    	int height = src.height();
    	int width = src.width();
    	int total_pixels = width * height;
    	char t ;
    	auto coord = std::make_pair(ENCODE_SIZE, 0);
    	for(int nr=ENCODE_SIZE; nr<total_pixels && nr < msg_length; nr+=BIT_TO_BYTE){
    		coord.first = nr % width; // width
    		coord.second = nr / width; //height
    		t = decode_single_byte(src, coord);
    		message += t;
    	}
    	return message;
    }

    char decode_single_byte(CImg<unsigned char>& image,
    	std::pair<int, int>& coord){
    	int bit = 0;
    	uint8_t to_decode = 0;
    	int w = coord.first;
    	int count = 0;
    	for(int h = coord.second; h<image.height(); h++){
    		for(; w<image.width() && count < BIT_TO_BYTE; w++, count++){
    			bit = image(w, h, 1) & 1U;
    			to_decode <<= 1;
    			to_decode ^= (-bit ^ to_decode) & 1U;
    		}
    	}
    	return to_decode;
    }

    uint64_t decode_length(CImg<unsigned char>& image){
    	uint64_t msg_length = 0;
    	int bit;
    	for(int i=0; i<ENCODE_SIZE; i++){
    		msg_length <<= 1; 
    		bit = image(i, 0, 1) & 1U;
    		msg_length ^= (-bit ^ msg_length) & 1U;
    	}
    	return msg_length;

    }



    void enocde_length(uint64_t msg_length, 
    	CImg<unsigned char>& image){

		int bit;
    	// encoding the size to the picture
    	for(int i=ENCODE_SIZE-1, j = 0; i>=0; i--, j++){
    		bit = (msg_length >> j) & 1U; // get the bit at the position
    		// changing green value of the message
    		image(i, 0, 1) ^= (-bit ^ image(i, 0, 1)) & 1U;
    	}
    }

    std::pair<int,int> encode_single_byte(uint8_t to_encode, 
    	CImg<unsigned char>& image,
    	std::pair<int, int>& coord){
    	int shift_count = BIT_TO_BYTE - 1;
    	int bit;
    	int w = coord.first;
    	for(int h = coord.second; h<image.height(); h++){
    		for(; w<image.width(); w++){
    			bit = (to_encode >> shift_count) & 1U;
    			shift_count--;
    			image(w, h, 1) ^= (-bit ^ image(w,h,1)) & 1U;

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