/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class allpass : public object<allpass>, public sample_operator<1,1> {
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
    //outlet<>		out2	{ this, "(signal) Right Output", "signal" };

	allpass(const atoms& args = {}) {
        if (!args.empty()) {
			// TODO: what happens when there are no arguments?
        }

        //m_allpass_filter.delay(142);
        //m_allpass_filter.gain(0.75);
        
	}
    
    
    attribute<double, threadsafe::yes, limit::clamp> gain_coefficient { this, "gain coefficient",
        0.75,
        description{"Control the intensity of the allpass effect. This value is applied to the undelayed input and delayed feedback. At 0.0, this filter will function as a feedforward delay. At 1.0 or -1.0, the feedback will become unstable."}, 
        range { -1.0, 1.0 },
        setter { MIN_FUNCTION {
            m_allpass_filter.gain(args[0]);
            return args;
        }}
    };
    
    
    attribute<double, threadsafe::no, limit::clamp> delay_time { this, "delay time",
        1.0,
        description{"Duration of delay in milliseconds. This value will control the frequencies that exhibit phase changes. Above 40.0, the delayed signals become audible echoes."},
        range { 0.0, 1000.0 },
        setter { MIN_FUNCTION {
            number new_delay_time = args[0];
            int delay_samps = new_delay_time * 0.001 * samplerate(); // NW: For now, we truncate to the nearest sample
            m_allpass_filter.delay(delay_samps);
            return args;
        }}
    };


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

	sample operator()(sample input) {
        
        //m_allpass_filter.delay(10);
        //m_allpass_filter.gain(0.75);
		
        auto output = m_allpass_filter(input);
        
        return { output };
	}

    
};

MIN_EXTERNAL(allpass);
