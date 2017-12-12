/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class allpass : public object<allpass>, public sample_operator<1,2> {
private:
    // note: these must be created prior to any attributes that might set parameters below
    lib::allpass        m_allpass_filter { 44100 };        ///< allpass filter
    
public:

	MIN_DESCRIPTION {	"Apply allpass filter. "

	};
	MIN_TAGS		{	"audio, filters"		};
	MIN_AUTHOR		{	"Cycling '74"			};
	MIN_RELATED		{	"allpass~, filterdesign"	};

    
    inlet<>			in1		{ this, "(signal) Input 1" };
    outlet<>		out1	{ this, "(signal) Left Output", "signal" };
    outlet<>		out2	{ this, "(signal) Right Output", "signal" };

	allpass(const atoms& args = {}) {
		if (!args.empty())
			// TODO: what happens when there are no arguments?

        m_allpass_filter.delay(10);
        m_allpass_filter.gain(0.75);
	}


	message<> clear { this, "clear",
		"Reset the allpass filter. Because this is an IIR filter it has the potential to blow-up, requiring a reset.",
		MIN_FUNCTION {
            m_allpass_filter.clear();
			return {};
		}
	};
    
    
    attribute<bool>	bypass { this, "bypass" , false, description{"Pass the input straight-through."} };

	/// Process one sample
	/// Max takes care of squashing denormal for us by setting the FTZ bit on the CPU.

	samples<2> operator()(sample input) {
		
        auto output = m_allpass_filter(input);
        
        return {{ output, output }};
	}

    
};

MIN_EXTERNAL(allpass);
