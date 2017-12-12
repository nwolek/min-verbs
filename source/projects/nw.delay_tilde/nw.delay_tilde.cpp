/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class delay : public object<delay>, public sample_operator<1,2> {
private:
    // note: these must be created prior to any attributes that might set parameters below
    lib::delay<>        m_delay { 44100 };        ///< delay filter
    
public:

	MIN_DESCRIPTION {	"Apply delay filter. "

	};
	MIN_TAGS		{	"audio, filters"		};
	MIN_AUTHOR		{	"Cycling '74"			};
	MIN_RELATED		{	"delay~, filterdesign"	};

    
    inlet<>			in1		{ this, "(signal) Input 1" };
    outlet<>		out1	{ this, "(signal) Left Output", "signal" };
    outlet<>		out2	{ this, "(signal) Right Output", "signal" };

	delay(const atoms& args = {}) {
		if (!args.empty())
			// TODO: what happens when there are no arguments?

        m_delay.size(10);
        
	}


	message<> clear { this, "clear",
		"Reset the delay filter. Because this is an IIR filter it has the potential to blow-up, requiring a reset.",
		MIN_FUNCTION {
            m_delay.clear();
			return {};
		}
	};
    
    
    attribute<bool>	bypass { this, "bypass" , false, description{"Pass the input straight-through."} };

	/// Process one sample
	/// Max takes care of squashing denormal for us by setting the FTZ bit on the CPU.

	samples<2> operator()(sample input) {
		
        auto output = m_delay(input);
        
        return {{ output, output }};
	}

    
};

MIN_EXTERNAL(delay);
