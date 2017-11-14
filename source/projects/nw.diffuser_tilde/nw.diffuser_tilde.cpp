/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2017, lowkey digital studio
/// @author		Nathan Wolek
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class diffuser : public object<diffuser>, public vector_operator<> {
public:

	MIN_DESCRIPTION {	"Apply 4 allpass filters in series"
	};
	MIN_TAGS		{	"audio, filters"		};
	MIN_AUTHOR		{	"Cycling '74"			};
	MIN_RELATED		{	"allpass~, filterdesign"	};


	// Because our object defines a constructor (below) this argument definition is for
	// documentation purposes only.
	argument<int> channel_count_arg { this, "channel_count", "The number of channels to process." };


	diffuser(const atoms& args = {}) {
		if (!args.empty())
			m_channel_count = args[0];

		for (auto i=0; i<m_channel_count; ++i) {
			m_inlets.push_back(	 std::make_unique<inlet<>>(this, "(signal) audio input") );
			m_outlets.push_back( std::make_unique<outlet<>>(this, "(signal) audio output", "signal") );
			m_filters.push_back( std::make_unique<lib::allpass>() );
		}
	}


	message<> clear { this, "clear",
		"Reset the allpass filter. Because this is an IIR filter it has the potential to blow-up, requiring a reset.",
		MIN_FUNCTION {
			for (auto& filter : m_filters)
				filter->clear();
			return {};
		}
	};


	attribute<bool>	bypass { this, "bypass" , false, description{"Pass the input straight-through."} };


	/// Process N channels of audio
	/// Max takes care of squashing denormal for us by setting the FTZ bit on the CPU.

	void operator()(audio_bundle input, audio_bundle output) {
		if (bypass)
			output = input;
		else {
			for (auto channel=0; channel<m_channel_count; ++channel) {
				auto	x = input.samples(channel);
				auto	y = output.samples(channel);
				auto&	f = *m_filters[channel];

				for (auto i=0; i<input.frame_count(); ++i) {
					y[i] = f(x[i]);
				}
			}
		}
	}


	// Inherit the a sample_operator-style call for processing audio through the vector_operator above
	// This can helpful (and is in this case) for writing cleaner unit tests

	using vector_operator::operator();


private:
	int										m_channel_count = 1;		///< number of channels
	vector< unique_ptr<inlet<>> >			m_inlets;				///< this object's inlets
	vector< unique_ptr<outlet<>> >			m_outlets;				///< this object's outlets
	vector< unique_ptr<lib::allpass> >	m_filters;				///< allpass filters for each channel
};

MIN_EXTERNAL(diffuser);
