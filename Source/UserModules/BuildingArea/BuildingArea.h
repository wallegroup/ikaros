//
//	BuildingArea.h		This file is a part of the IKAROS project
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

#ifndef BuildingArea_
#define BuildingArea_

#include "IKAROS.h"

class BuildingArea: public Module
{
public:
    static Module * Create(Parameter * p) { return new BuildingArea(p); }

    BuildingArea(Parameter * p) : Module(p) {}
    virtual ~BuildingArea();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float **    input_matrix;
    int         input_matrix_size_x;
    int         input_matrix_size_y;
    
    float **    output_matrix;
    int         output_matrix_size_x;
    int         output_matrix_size_y;
    
    float **    output_BACoords;
    int         output_BACoords_size_x;
    int         output_BACoords_size_y;

    // internal data storage

    float *     internal_array;
    float **    internal_matrix;
    float **    markerCoordinates;
    
    // parameter values
    
    float boundary_x_furthest;
    float boundary_x_closest;
    float boundary_y_left;
    float boundary_y_right;
    float boundary_z_top;
    float boundary_z_bottom;
    float focus_point_z;
    
    
};

#endif
