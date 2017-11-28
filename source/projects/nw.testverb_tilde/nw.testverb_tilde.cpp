/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class testverb : public object<testverb>, public sample_operator<1,2> {
private:
    // note: these must be created prior to any attributes that might set parameters below
    lib::onepole        m_high_frequency_attenuation;          ///< onepole filter for input
    
    lib::allpass		m_input_diffusion_1a;				///< allpass filter 1a
    lib::allpass		m_input_diffusion_1b;				///< allpass filter 1b
    lib::allpass		m_input_diffusion_2a;				///< allpass filter 2a
    lib::allpass		m_input_diffusion_2b;				///< allpass filter 2b
    
    lib::allpass		m_decay_diffusion_1L;				///< allpass filter 1L (TODO: modulation)
    lib::allpass		m_decay_diffusion_1R;				///< allpass filter 1R (TODO: modulation)
    
    lib::delay<>        m_delay_1L = lib::delay<>(4453);    ///< delay 1L
    lib::delay<>        m_delay_2L = lib::delay<>(3720);    ///< delay 2L
    
    sample              m_last_out_L;                       ///< last sample from the left channel
    //sample              m_last_out_R;                       ///< last sample from the right channel
    
    
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

	testverb(const atoms& args = {}) {
		if (!args.empty())
			// TODO: what happens when there are no arguments?

        m_high_frequency_attenuation.coefficient(0.9995);
            
        m_input_diffusion_1a.delay(142);
        m_input_diffusion_1a.gain(0.750);
        m_input_diffusion_1b.delay(107);
        m_input_diffusion_1b.gain(0.750);
    
        m_input_diffusion_2a.delay(379);
        m_input_diffusion_2a.gain(0.625);
        m_input_diffusion_2b.delay(277);
        m_input_diffusion_2b.gain(0.625);
        
        //m_delay_1L.size(4453);
        //m_delay_2L.size(3720);
	}



	message<> clear { this, "clear",
		"Reset the allpass filters. Because this is an IIR filter it has the potential to blow-up, requiring a reset.",
		MIN_FUNCTION {
			m_input_diffusion_1a.clear();
			m_input_diffusion_1b.clear();
			m_input_diffusion_2a.clear();
			m_input_diffusion_2b.clear();
			
			return {};
		}
	};


	attribute<bool>	bypass { this, "bypass" , false, description{"Pass the input straight-through."} };
    
    /// Process one sample
    /// Max takes care of squashing denormal for us by setting the FTZ bit on the CPU.
    
    samples<2> operator()(sample input) {
        auto node_10 = m_high_frequency_attenuation(input);
        
        // node numbering below comes from Dattoro 1997, page 662
        auto node_14 = m_input_diffusion_1a(node_10);
        auto node_20 = m_input_diffusion_1b(node_14);
        auto node_16 = m_input_diffusion_2a(node_20);
        auto node_22 = m_input_diffusion_2b(node_16) + 0.5 * m_last_out_L;
        
        auto node_30 = m_delay_1L(node_22);
        auto node_33 = 0.5 * node_30;
        auto node_39 = m_delay_2L(node_33);
        
        // TODO: sending different nodes to each channel for testing
        return {{ node_30, node_39 }};
    }

};

MIN_EXTERNAL(testverb);