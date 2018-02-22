/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class delay : public object<delay>, public sample_operator<1,1> {
private:
    // note: these must be created prior to any attributes that might set parameters below
    number              m_samples_per_second = samplerate();
    lib::delay<>        m_delay { m_samples_per_second };        ///< delay filter
    bool                m_needs_to_be_cleared = false;
    int                 m_delay_samps = 0;
    
public:

	MIN_DESCRIPTION {	"Apply delay filter. "

	};
	MIN_TAGS		{	"audio, filters"		};
	MIN_AUTHOR		{	"Cycling '74"			};
	MIN_RELATED		{	"delay~, filterdesign"	};

    
    inlet<>			in1		{ this, "(signal) Input" };
    outlet<>		out1	{ this, "(signal) Output", "signal" };

	delay(const atoms& args = {}) {
		if (!args.empty())
			// TODO: what happens when there are no arguments?

        m_delay.size(10);
        
	}
    
    
    attribute<double, threadsafe::yes, limit::clamp> delay_time { this, "delay time",
        1.0,
        description{"Duration of delay in milliseconds."},
        range { 0.0, 1000.0 },
        setter { MIN_FUNCTION {
            number new_delay_time = args[0];
            m_delay_samps = new_delay_time * 0.001 * samplerate(); // NW: For now, we truncate to the nearest sample
            return args;
        }}
    };


	message<> clear { this, "clear",
		"Reset the delay filter. Erases the contents of internal memory.",
		MIN_FUNCTION {
            m_delay.clear();
			return {};
		}
	};
    
    
    message<> dspsetup {this, "dspsetup",
        MIN_FUNCTION {
            
            // Because the maximum size of our delay is one second, we have to protect against changes in sample rate.
            // To do this, we create a new delay filter with the proper size and copy over the old parameter settings.
            
            number new_samples_per_second = samplerate();
            if (m_samples_per_second != new_samples_per_second) {
                
                lib::delay<> new_delay { new_samples_per_second };
                new_delay.size( m_delay.size() );
                
                m_samples_per_second = new_samples_per_second;
                m_delay = new_delay;
                
            }
            
            return {};
            
        }
    };
    
    
    attribute<bool>	bypass { this, "bypass" , false, description{"Pass the input straight-through."} };

	/// Process one sample
	/// Max takes care of squashing denormal for us by setting the FTZ bit on the CPU.

	sample operator()(sample input) {
		
        if (m_needs_to_be_cleared) {
            m_delay.clear();
        }
        
        sample output;
        
        if (bypass) {
            
            output = input;
            
        } else {
            
            m_delay.size(m_delay_samps);
            output = m_delay(input);
            
        }
        
        return { output };
	}

    
};

MIN_EXTERNAL(delay);
