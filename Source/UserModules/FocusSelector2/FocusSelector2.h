//
//	FocusSelector2.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2012 <Olof Knape>
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

#ifndef FocusSelector2_
#define FocusSelector2_

#include "IKAROS.h"

class FocusSelector2: public Module
{
public:
    static Module * Create(Parameter * p) { return new FocusSelector2(p); }

    FocusSelector2(Parameter * p) : Module(p) {}
    virtual ~FocusSelector2();

    void 		Init();
    void 		Tick();

    float *     input_array_doc;
    int         input_array_doc_size;

    float *     input_array_face;
    int         input_array_face_size;

    float *     input_array_change;
    int         input_array_change_size;

    float **    input_matrix_markers;
    int         input_matrix_markers_size_x;
    int         input_matrix_markers_size_y;

    float **    input_matrix_ba;
    int         input_matrix_ba_size_x;
    int         input_matrix_ba_size_y;

    float *     output_array;
    int         output_array_size;

    float *     internal_array;
    int         internal_array_size;

    int         z_movementThreshold;
    int         tick_faceThreshold;
    float       change_threshold;
    bool        moving_marker;
    int         t;

};

#endif
