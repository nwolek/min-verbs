// Copyright (c) 2017, lowkey digital studio
// Nathan Wolek
// Usage of this file and its contents is governed by the MIT License

#include "c74_min_unittest.h"		// required unit test header
#include "nw.allpass_tilde.cpp"	// need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

TEST_CASE( "produces valid impulse response" ) {
	ext_main(nullptr);	// every unit test must call ext_main() once to configure the class

	// create an instance of our object
	test_wrapper<allpass> an_instance;
	allpass& my_object = an_instance;
    
    REQUIRE( my_object.delay_time == 1.0 );             // check default attribute value
    REQUIRE( my_object.gain_coefficient == 0.75 );      // check default attribute value

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
		output.push_back(y);
	}
	
	// set up a reference impulse response to compare against
    // calculated in Octave using impz(a,b,256)
    sample_vector reference = {
        0.75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.4375, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, -0.328125, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0.24609375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        -0.1845703125, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.138427734375, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
	
	// check it
	REQUIRE( output == reference );
}

// NW: borrowed and adapted the following from dcblocker tests
SCENARIO( "responds appropriately to messages and attrs" ) {
    
    // create an input buffer to process... 10 cycles of a cos wave
    const int		buffersize = 1024;
    sample_vector	input(buffersize);
    
    std::generate(input.begin(), input.end(), lib::generator::cosine<sample>(buffersize, 10));
    
    GIVEN( "An instance of allpass~" ) {
        allpass	my_object;
        
        REQUIRE( my_object.bypass == false );	// default attr value
        
        WHEN( "the bypass attr is turned-on" ) {
            my_object.bypass = true;
            
            sample_vector output;
            for (auto x : input) {
                auto y = my_object(x);
                output.push_back(y);
            }
            
            THEN( "the output is identical to the input" )
            REQUIRE( output == input );
        }
        AND_WHEN( "the bypass is turned back off" ) {
            my_object.bypass = false;
            
            sample_vector output;
            for (auto x : input) {
                auto y = my_object(x);
                output.push_back(y);
            }
            
            THEN( "the output is processed again" )
            REQUIRE( output != input );
        }
        WHEN( "the input goes silent" ) {
            // dirty the history first
            for (auto x : input) {
                my_object(x);
            }
            
            // then zero and process
            std::fill_n(input.begin(), buffersize, 0.0);
            
            sample_vector output;
            for (auto x : input) {
                auto y = my_object(x);
                output.push_back(y);
            }
            
            THEN( "the input is all zeroes" )
            REQUIRE( input == sample_vector(buffersize, 0.0) );
            AND_THEN( "the output will not immediately go down zero" )
            REQUIRE( output != input );
        }
        //
    }
}
