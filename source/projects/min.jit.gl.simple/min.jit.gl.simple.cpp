/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2016, Cycling '74
/// @author		Timothy Place
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"
#include "jit.gl.h"

using namespace c74::min;

class jit_gl_simple : public object<jit_gl_simple>, public gl_operator {
public:
	
	MIN_DESCRIPTION { "Simple example of GL object using min." };
	MIN_TAGS		{ "OpenGL" };
	MIN_AUTHOR		{ "Cycling '74" };
	MIN_RELATED		{ "jit.gl.gridshape" };

	inlet<>		input	{ this, "(matrix) Input", "matrix" };
	outlet<>	output	{ this, "(matrix) Output", "matrix" };
	

	attribute<number> width { this, "width", 1.0,
		description { "The width of our quad." },
		setter { MIN_FUNCTION {
			double in = args[0];
			halfwidth = in / 2.0;
			return args;
		}}
	};
	
	
	attribute<number> height { this, "height", 1.0,
		description { "The height of our quad." },
		setter { MIN_FUNCTION {
			double in = args[0];
			halfheight = in / 2.0;
			return args;
		}}
	};

	message<> draw = { this, "draw", MIN_FUNCTION {
		// draw our OpenGL geometry.
		
		glBegin(GL_QUADS);
		glVertex3f(-halfwidth, -halfheight, 0);
		glVertex3f(-halfwidth, halfheight, 0);
		glVertex3f(halfwidth, halfheight, 0);
		glVertex3f(halfwidth, -halfheight, 0);
		glEnd();
		
		return {};
	}};
	
private:
	float	halfwidth;
	float	halfheight;
};

MIN_EXTERNAL(jit_gl_simple);
