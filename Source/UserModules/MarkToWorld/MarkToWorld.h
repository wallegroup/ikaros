//
//	MarkToWorld.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2014 <wall-E team>
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

#ifndef MarkToWorld_
#define MarkToWorld_

#include "IKAROS.h"

class MarkToWorld: public Module
{
public:
    static Module * Create(Parameter * p) { return new MarkToWorld(p); }

    MarkToWorld(Parameter * p) : Module(p) {}
    virtual ~MarkToWorld();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float **    input_matrix;
    int         input_matrix_size_x;
    int         input_matrix_size_y;

    float *     output_array;
    int         output_array_size;

    // internal data storage

    float *     internal_array;
    float **    internal_matrix;

    float       memory_x;
    float       memory_y;
    float       memory_z;
};

#endif
