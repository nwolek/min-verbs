/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::lib;

class testverb : public object<testverb>, public sample_operator<1,2> {
private:
    // note: these must be created prior to any attributes that might set parameters below
    onepole         m_high_frequency_attenuation;    ///< onepole filter for input
    
    allpass         m_input_diffusion_1a { 142 };           ///< allpass filter 1a
    allpass         m_input_diffusion_1b { 107 };           ///< allpass filter 1b
    allpass         m_input_diffusion_2a { 379 };           ///< allpass filter 2a
    allpass         m_input_diffusion_2b { 277 };           ///< allpass filter 2b
    
    allpass         m_decay_diffusion_1L { 672 };           ///< allpass filter 1L (TODO: modulation)
    allpass         m_decay_diffusion_1R { 908 };           ///< allpass filter 1R (TODO: modulation)
    
    delay<>         m_delay_1L { 4453 };                     ///< delay 1L
    delay<>         m_delay_2L { 3720 };                     ///< delay 2L
    
    delay<>         m_delay_1R { 4217 };                     ///< delay 1L
    delay<>         m_delay_2R { 3163 };                     ///< delay 2L
    
    sample          m_last_out_L;                   ///< last sample from the left channel
    sample          m_last_out_R;                   ///< last sample from the right channel

public:

	MIN_DESCRIPTION {	"Beginnings of new gverb implementation."
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
            
        m_input_diffusion_1a.gain(0.750);
        m_input_diffusion_1b.gain(0.750);
    
        m_input_diffusion_2a.gain(0.625);
        m_input_diffusion_2b.gain(0.625);
        
        m_decay_diffusion_1L.gain(-0.70);
        m_decay_diffusion_1R.gain(-0.70);

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
        
        // initial diffusion
        auto node_14 = m_input_diffusion_1a(node_10);
        auto node_20 = m_input_diffusion_1b(node_14);
        auto node_16 = m_input_diffusion_2a(node_20);
        auto node_22 = m_input_diffusion_2b(node_16);
        
        // left channel decays
        auto node_23 = node_22 + 0.5 * m_last_out_R;
        auto node_24 = m_decay_diffusion_1L(node_23);
        auto node_30 = m_delay_1L(node_24);
        auto node_33 = 0.5 * node_30;
        auto node_39 = m_delay_2L(node_33);
        
        // right channel decays
        auto node_46 = node_22 + 0.5 * m_last_out_L;
        auto node_48 = m_decay_diffusion_1R(node_46);
        auto node_54 = m_delay_1R(node_48);
        auto node_59 = 0.5 * node_54;
        auto node_63 = m_delay_2R(node_59);
        
        m_last_out_L = node_39;
        m_last_out_R = node_63;
        
        // TODO: sending different nodes to each channel for testing
        return {{ node_39, node_63 }};
    }

};

MIN_EXTERNAL(testverb);
