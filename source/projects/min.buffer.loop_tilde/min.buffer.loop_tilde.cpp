/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2016, Cycling '74
/// @author		Timothy Place
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class buffer_loop : public object<buffer_loop>, vector_operator {
public:
	
	MIN_DESCRIPTION { "Read from a buffer~." };
	MIN_TAGS		{ "audio, sampling" };
	MIN_AUTHOR		{ "Cycling '74" };
	MIN_RELATED		{ "index~, buffer~, wave~" };

	inlet<>				index_inlet		{ this, "(signal) Sample index" };
	inlet<>				channel_inlet	{ this, "(float) Audio channel to use from buffer~" };
	outlet<>			output			{ this, "(signal) Sample value at index", "signal" };
	outlet<>			sync			{ this, "(signal) Sync", "signal" };

	buffer_reference	buffer			{ this, 
		MIN_FUNCTION {
			length.touch();
			return {};
		}
	};

	
	argument<symbol> name_arg { this, "buffer-name", "Initial buffer~ from which to read.",
		MIN_ARGUMENT_FUNCTION {
			buffer.set(arg);
		}
	};

	argument<int> channel_arg { this, "channel", "Initial channel to read from the buffer~.",
		MIN_ARGUMENT_FUNCTION {
			channel = arg;
		}
	};


	attribute<int> channel { this, "channel", 1,
		description { "Channel to read from the buffer~." },
		setter { MIN_FUNCTION {
			int n = args[0];
			if (n < 1)
				n = 1;
			return {n};
		}}
	};


	attribute<number> length { this, "length", 1000.0,
		title { "Length (ms)"},
		description { "Length of the buffer~ in milliseconds." },
		setter { MIN_FUNCTION {
			number new_length = args[0];
			if (new_length <= 0.0)
				new_length = 1.0;

			buffer_lock<false> b { buffer };
			b.resize(new_length);

			return {new_length};
		}},
		getter { MIN_GETTER_FUNCTION {
			buffer_lock<false> b { buffer };
			return { b.length_in_seconds() * 1000.0 };
		}}
	};


	attribute<number> speed { this, "speed", 1.0,
		description { "Playback speed of the loop" }
	};


	attribute<bool> record { this, "record", false,
		description { "Record into the loop" }
	};


	message<> number_message { this, "number", "Toggle the recording attribute.",
		MIN_FUNCTION {
			record = args[0];
			return {};
		}
	};


	message<> dspsetup { this, "dspsetup", MIN_FUNCTION {
		double samplerate = args[0];

		m_one_over_samplerate = 1.0 / samplerate;
		return {};
	}};


	void operator()(audio_bundle input, audio_bundle output) {
		auto			in = input.samples(0);
		auto			out = output.samples(0);
		auto			sync = output.samples(1);
		buffer_lock<>	b(buffer);
		auto			chan = std::min<int>(channel-1, b.channelcount());
		
		if (b.valid()) {
			number	speed = this->speed;
			auto	position = m_playback_position;
			auto	frames = b.framecount();
			auto	length_in_seconds = b.length_in_seconds();
			auto	frequency = (1.0 / length_in_seconds) * speed;
			auto	stepsize = frequency * m_one_over_samplerate;

			for (auto i=0; i<input.framecount(); ++i) {
				// phasor
				position += stepsize;
				position = std::fmod(position, 1.0);
				sync[i] = position;

				// buffer playback
				auto frame = position * frames;
				out[i] = b.lookup(frame, chan);
			}
			m_playback_position = position;

			if (bool(record)) {
				auto record_position = m_record_position;

				for (auto i=0; i<input.framecount(); ++i) {
					if (record_position >= frames)
						record_position = 0;
					b.lookup(record_position, chan) = in[i];
					++record_position;
				}
				m_record_position = record_position;
				b.dirty();
			}

		}
		else {
			output.clear();
		}
	}

private:
	double	m_playback_position		{ 0.0 };	// normalized range
	size_t	m_record_position		{ 0 };		// native range
	double	m_one_over_samplerate	{ 1.0 };
};


MIN_EXTERNAL(buffer_loop);
