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
    fifo<double>        m_high_frequency_coefficient { 10 };       ///< coefficient for onepole filter
    
    lib::allpass		m_input_diffusion_1a { 142 };				///< allpass filter 1a
    lib::allpass		m_input_diffusion_1b;				///< allpass filter 1b
    fifo<double>        m_input_diffusion_1_coefficient { 10 };       ///< coefficient for allpass filters 1a & 1b
    lib::allpass		m_input_diffusion_2a;				///< allpass filter 2a
    lib::allpass		m_input_diffusion_2b;				///< allpass filter 2b
    fifo<double>        m_input_diffusion_2_coefficient { 10 };       ///< coefficient for allpass filters 2a & 2b
    
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
            
        m_input_diffusion_1a.delay(142);
        m_input_diffusion_1a.gain(0.750);
        m_input_diffusion_1b.delay(107);
        m_input_diffusion_1b.gain(0.750);
    
        m_input_diffusion_2a.delay(379);
        m_input_diffusion_2a.gain(0.625);
        m_input_diffusion_2b.delay(277);
        m_input_diffusion_2b.gain(0.625);
	}


    attribute<double, threadsafe::no, limit::clamp> input_damping_coefficient { this, "input damping coefficient", 0.9995,
        range { 0.0, 1.0 },
        setter { MIN_FUNCTION {
            m_high_frequency_coefficient.try_enqueue(args[0]);
            return args;
        }}
    };
    
    
    attribute<double, threadsafe::no, limit::clamp> input_diffusion1_coefficient { this, "input diffusion 1 coefficient", 0.750,
        range { 0.0, 1.0 },
        setter { MIN_FUNCTION {
            m_input_diffusion_1_coefficient.try_enqueue(args[0]);
            return args;
        }}
    };
    
    
    attribute<double, threadsafe::no, limit::clamp> input_diffusion2_coefficient { this, "input diffusion 2 coefficient", 0.625,
        range { 0.0, 1.0 },
        setter { MIN_FUNCTION {
            m_input_diffusion_2_coefficient.try_enqueue(args[0]);
            return args;
        }}
    };
    

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
        
        number x;
        while (m_high_frequency_coefficient.try_dequeue(x)) {
            m_high_frequency_attenuation.coefficient(x);
        }
        while (m_input_diffusion_1_coefficient.try_dequeue(x)) {
            m_input_diffusion_1a.gain(x);
            m_input_diffusion_1b.gain(x);
        }
        while (m_input_diffusion_2_coefficient.try_dequeue(x)) {
            m_input_diffusion_2a.gain(x);
            m_input_diffusion_2b.gain(x);
        }
        
        auto node_10 = m_high_frequency_attenuation(input);
        
        // node numbering below comes from Dattoro 1997, page 662
        auto node_14 = m_input_diffusion_1a(node_10);
        auto node_20 = m_input_diffusion_1b(node_14);
        auto node_16 = m_input_diffusion_2a(node_20);
        auto node_22 = m_input_diffusion_2b(node_16);
        
        // TODO: sending different nodes to each channel for testing
        return {{ node_20, node_22 }};
    }
   

};

MIN_EXTERNAL(diffuser);
