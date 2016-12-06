/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2016, Cycling '74
/// @author		Timothy Place
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;

class jit_gl_simple : public object<jit_gl_simple>, gl_operator {
public:
	
	MIN_DESCRIPTION { "Simple example of GL object using min." };
	MIN_TAGS		{ "OpenGL" };
	MIN_AUTHOR		{ "Cycling '74" };
	MIN_RELATED		{ "jit.clip" };

	inlet<>		input	{ this, "(matrix) Input", "matrix" };
	outlet<>	output	{ this, "(matrix) Output", "matrix" };
	

	attribute<number> min { this, "min", 0.0,
		description { "The minimum value below which clipping occurs." },
		setter { MIN_FUNCTION {
			double in = args[0];
			cmin = static_cast<uchar>(c74::max::clamp(255.0 * in, 0.0, 255.0));
			return args;
		}},
		getter { MIN_GETTER_FUNCTION {
			return { cmin / 255.0 };
		}}
	};
	
	
	attribute<number> max { this, "max", 1.0,
		description { "The maximum value above which clipping occurs." },
		setter { MIN_FUNCTION {
			double in = args[0];
			cmax = static_cast<uchar>(c74::max::clamp(255.0 * in, 0.0, 255.0));
			return args;
		}},
		getter { MIN_GETTER_FUNCTION {
			return { cmax / 255.0 };
		}}
	};


private:
	uchar	cmin;
	uchar	cmax;
};

MIN_EXTERNAL(jit_gl_simple);
