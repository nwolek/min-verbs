/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class diffuser : public object<diffuser>, public sample_operator<1,2> {
private:
    // note: these must be created prior to any attributes that might set parameters below
    lib::onepole        m_high_frequency_attenuation;          ///< onepole filter for input
    
    lib::allpass		m_diffusion1a;				///< allpass filter 1a
    lib::allpass		m_diffusion1b;				///< allpass filter 1b
    lib::allpass		m_diffusion2a;				///< allpass filter 2a
    lib::allpass		m_diffusion2b;				///< allpass filter 2b
    
public:

	MIN_DESCRIPTION {	"Apply 4 allpass filters in series"
	};
	MIN_TAGS		{	"audio, filters"		};
	MIN_AUTHOR		{	"Cycling '74"			};
	MIN_RELATED		{	"allpass~, filterdesign"	};

    
    inlet<>			in1		{ this, "(signal) Input 1" };
    outlet<>		out1	{ this, "(signal) Left Output", "signal" };
    outlet<>		out2	{ this, "(signal) Right Output", "signal" };

    /*
	// Because our object defines a constructor (below) this argument definition is for
	// documentation purposes only.
	argument<int> channel_count_arg { this, "channel_count", "The number of channels to process." };
     */

	diffuser(const atoms& args = {}) {
		if (!args.empty())
			// TODO: what happens when there are no arguments?

        m_high_frequency_attenuation.coefficient(0.9995);
            
        m_diffusion1a.delay(142);
        m_diffusion1a.gain(0.750);
        m_diffusion1b.delay(107);
        m_diffusion1b.gain(0.750);
    
        m_diffusion2a.delay(379);
        m_diffusion2a.gain(0.625);
        m_diffusion2b.delay(277);
        m_diffusion2b.gain(0.625);
	}



	message<> clear { this, "clear",
		"Reset the allpass filters. Because this is an IIR filter it has the potential to blow-up, requiring a reset.",
		MIN_FUNCTION {
			m_diffusion1a.clear();
			m_diffusion1b.clear();
			m_diffusion2a.clear();
			m_diffusion2b.clear();
			
			return {};
		}
	};


	attribute<bool>	bypass { this, "bypass" , false, description{"Pass the input straight-through."} };
    
    /// Process one sample
    /// Max takes care of squashing denormal for us by setting the FTZ bit on the CPU.
    
    samples<2> operator()(sample input) {
        auto node_10 = m_high_frequency_attenuation(input);
        
        auto node_14 = m_diffusion1a(node_10);
        auto node_20 = m_diffusion1b(node_14);
        auto node_16 = m_diffusion2a(node_20);
        auto node_22 = m_diffusion2b(node_16);
        
        return {{ node_20, node_22 }};
    }

};

MIN_EXTERNAL(diffuser);
