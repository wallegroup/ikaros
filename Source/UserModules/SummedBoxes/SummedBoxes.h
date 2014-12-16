//
//	SummedBoxes.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2014 Wall-E Group Inc.
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    See http://www.ikaros-project.org/ for more information.
//

#ifndef SummedBoxes_
#define SummedBoxes_

#include "IKAROS.h"

class SummedBoxes: public Module
{
public:
    static Module * Create(Parameter * p) { return new SummedBoxes(p); }

    SummedBoxes(Parameter * p) : Module(p) {}
    virtual ~SummedBoxes();

    void 		Init();
    void 		Tick();

    float **    input_matrix;
    int         input_matrix_size_x;
    int         input_matrix_size_y;

    float *     output_array;
    int         output_array_size;

    float *     output_array_box;
    int         output_array_box_size;

    float **    sAT;
    int         boxSize;

    int sizeX;
    int sizeY;

    int         box_count;
};

#endif
