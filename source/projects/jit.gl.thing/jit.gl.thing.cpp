/// @file	
///	@ingroup 	minexamples
///	@copyright	Copyright (c) 2016, Cycling '74
/// @author		Andrew Benson
///	@license	Usage of this file and its contents is governed by the MIT License

#include "c74_min.h"

using namespace c74::min;
using namespace std;


// texture map coordinates
#define X1	0.0
#define X2	0.25
#define X3	0.5
#define X4	0.75
#define X5	1.0
#define Y1	0.125
#define Y2	0.375
#define Y3	0.625
#define Y4	0.875
// edge flags with padding for intermediate planes
#define E0	0, 0, 0, 0, 0, 0, 0, 0
#define E1	0, 0, 0, 0, 0, 0, 0, 1

static float cube_v[13*12*3] {
	-1, 1, 1,  X2, Y3, 	E1,//3
	-1, 1, -1,  X2, Y4, E1,//2
	1, 1, -1,  X3, Y4, 	E0,//6
	
	1, 1, -1,  X3, Y4, 	E1,//6
	1, 1, 1, 	X3, Y3, E1,//7
	-1, 1, 1,  X2, Y3, 	E0,//3
	
	-1, -1, -1, X1, Y2, E1,//0
	-1, 1, -1,  X1, Y3, E1,//2
	-1, 1, 1,  X2, Y3, 	E0,//3
	
	-1, 1, 1,  X2, Y3, 	E1,//3
	-1, -1, 1,  X2, Y2, E1,//1
	-1, -1, -1, X1, Y2, E0,//0
	
	-1, -1, 1,  X2, Y2, E1,//1
	-1, 1, 1,  X2, Y3, 	E1,//3
	1, 1, 1, 	X3, Y3, E0,//7
	
	1, 1, 1, 	X3, Y3, E1,//7
	1, -1, 1,  X3, Y2, 	E1,//5
	-1, -1, 1,  X2, Y2, E0,//1
	
	1, -1, 1,  X3, Y2, 	E1,//5
	1, 1, 1, 	X3, Y3, E1,//7
	1, 1, -1,  X4, Y3, 	E0,//6
	
	1, 1, -1,  X4, Y3, 	E1,//6
	1, -1, -1,  X4, Y2, E1,//4
	1, -1, 1,  X3, Y2, 	E0,//5
	
	1, -1, -1,  X4, Y2, E1,//4
	1, 1, -1,  X4, Y3, 	E1,//6
	-1, 1, -1,  X5, Y3, E0,//2
	
	-1, 1, -1,  X5, Y3, E1,//2
	-1, -1, -1, X5, Y2, E1,//0
	1, -1, -1,  X4, Y2, E0,//4
	
	-1, -1, -1, X2, Y1, E1,//0
	-1, -1, 1,  X2, Y2, E1,//1
	1, -1, 1,  X3, Y2, 	E0,//5
	
	1, -1, 1,  X3, Y2, 	E1,//5
	1, -1, -1,  X3, Y1, E1,//4
	-1, -1, -1, X2, Y1, E0//0
};

#undef X1
#undef X2
#undef X3
#undef X4
#undef X5
#undef Y1
#undef Y2
#undef Y3
#undef Y4
#undef E0
#undef E1


class gl_thing : public object<gl_thing>, gl_operator {
public:
	
	inlet	input	{ this, "(matrix) Geometry", "matrix" };
//	outlet	output	{ this, "(matrix) Output", "matrix" };
	
	
	
/*
	ATTRIBUTE (distance, double, 0.1) {
		double value = args[0];
		
		if (value < 0.0)
			value = 0.0;
		
		args[0] = value;
	}
	END
	
	
	ATTRIBUTE (strength, double , 0.2) {
		double value = args[0];
		
		if (value < 0.0)
			value = 0.0;
		
		args[0] = value;
	}
	END
*/
	
	/*
	template<class matrix_type, size_t planecount>
	cell<matrix_type,planecount> calc_cell(cell<matrix_type,planecount> input, const matrix_info& source, matrix_coord& position) {
		cell<matrix_type,planecount> output;
        long x1 = MIN_CLAMP( position.x() + 1, 0, source.width()-1);
        auto p1 = source.in_cell<matrix_type,planecount>(x1, position.y());
        
        long x2 = MIN_CLAMP( position.x() - 1, 0, source.width()-1);
        auto y1 = MIN_CLAMP( 0, position.y() + 1, source.height()-1);
        auto y2 = MIN_CLAMP( 0, position.y() - 1, source.height()-1);

        //constrain to the right
        double dx1 = input[0]-p1[0];
        double dy1 = input[1]-p1[1];
        double dz1 = input[2]-p1[2];
        
        double len1 = std::fmax(std::sqrt(dx1*dx1+dy1*dy1+dz1*dz1),0.0001);
        double amt1 = ((len1-distance)/len1)*strength;
        double ox1 = input[0]-amt1*dx1;
        double oy1 = input[1]-amt1*dy1;
        double oz1 = input[2]-amt1*dz1;
        
        
        //constrain to the left
        p1 = source.in_cell<matrix_type,planecount>(x2,position.y());
        dx1 = ox1-p1[0];
        dy1 = oy1-p1[1];
        dz1 = oz1-p1[2];
        
        len1 = std::fmax(std::sqrt(dx1*dx1+dy1*dy1+dz1*dz1),0.0001);
        amt1 = ((len1-distance)/len1)*strength;
        
        ox1 = ox1-amt1*dx1;
        oy1 = oy1-amt1*dy1;
        oz1 = oz1-amt1*dz1;
        
        //constrain downward
        p1 = source.in_cell<matrix_type,planecount>(position.x(),y1);
        dx1 = ox1-p1[0];
        dy1 = oy1-p1[1];
        dz1 = oz1-p1[2];
        
        len1 = std::fmax(std::sqrt(dx1*dx1+dy1*dy1+dz1*dz1),0.0001);
        amt1 = ((len1-distance)/len1)*strength;
        
        
        ox1 = ox1-amt1*dx1;
        oy1 = oy1-amt1*dy1;
        oz1 = oz1-amt1*dz1;

        //constrain upward
        p1 = source.in_cell<matrix_type,planecount>(position.x(),y2);
        dx1 = ox1-p1[0];
        dy1 = oy1-p1[1];
        dz1 = oz1-p1[2];
        
        len1 = std::fmax(std::sqrt(dx1*dx1+dy1*dy1+dz1*dz1),0.00001);
        amt1 = ((len1-distance)/len1)*strength;
        
        output[0] = ox1-amt1*dx1;
        output[1] = oy1-amt1*dy1;
        output[2] = oz1-amt1*dz1;

		return output;
	}
	*/
	
	
	
private:
	
	unique_ptr<glchunk> build_chunk() {
		
//		glchunk(gl_ob* context, gl_drawing_primitive prim, int planecount, int vertexcount, int indexcount)

		auto newchunk = make_unique<glchunk>(m_maxobj, triangles, 13, 12*3, 0);
		
		
		return newchunk;
	}

	
	unique_ptr<glchunk> chunk { build_chunk() };

};


MIN_EXTERNAL(gl_thing);
