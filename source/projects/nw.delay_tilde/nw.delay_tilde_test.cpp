// Copyright (c) 2017, lowkey digital studio
// Nathan Wolek
// Usage of this file and its contents is governed by the MIT License

#include "c74_min_unittest.h"		// required unit test header
#include "nw.delay_tilde.cpp"	// need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

TEST_CASE( "produces valid impulse response" ) {
	ext_main(nullptr);	// every unit test must call ext_main() once to configure the class

	// create an instance of our object
	test_wrapper<delay> an_instance;
	delay& my_object = an_instance;

	// create an impulse buffer to process
	const int		buffersize = 256;
	sample_vector	impulse(buffersize);
	
	std::fill_n(impulse.begin(), buffersize, 0.0);
	impulse[0] = 1.0;
	
	// output from our object's processing
	sample_vector	output;
	
	// run the calculations
	for (auto x : impulse) {
		auto y = my_object(x);
		//output.push_back(y);
	}
	
	// get a reference impulse response to compare against
	//auto reference = lib::filters::generate_impulse_response({1.0,-1.0}, {1.0,-0.9997}, buffersize);
	
	// check it
	//REQUIRE( output == reference );
}
