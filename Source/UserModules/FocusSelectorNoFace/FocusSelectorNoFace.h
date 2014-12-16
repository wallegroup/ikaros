//
//	FocusSelectorNoFace.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2012 <Author Name>
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

#ifndef FocusSelectorNoFace_
#define FocusSelectorNoFace_

#include "IKAROS.h"

class FocusSelectorNoFace: public Module
{
public:
    static Module * Create(Parameter * p) { return new FocusSelectorNoFace(p); }

    FocusSelectorNoFace(Parameter * p) : Module(p) {}
    virtual ~FocusSelectorNoFace();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array_doc;
    int         input_array_doc_size;

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

    int         z_constant;
    bool        moving_marker;

    float *     output_array2;
    int         output_array2_size;

};

#endif
