//
//	MarkToWorld.cc		This file is a part of the IKAROS project
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

#include "MarkToWorld.h"
// use the ikaros namespace to access the math library
// this is preferred to using math.h
using namespace ikaros;

void
MarkToWorld::Init()
{
    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    internal_array = create_array(3);
    internal_matrix = create_matrix(input_matrix_size_x, input_matrix_size_y);
    
    memory_x = 0;
    memory_y = 0;
    memory_z = 0;
}



MarkToWorld::~MarkToWorld()
{
    // Destroy data structures that you allocated in Init.

    destroy_array(internal_array);
    destroy_matrix(internal_matrix);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}

void
MarkToWorld::Tick()
{

    copy_matrix(internal_matrix, input_matrix, input_matrix_size_x, input_matrix_size_y);
    
    if(internal_matrix[0][3] != 0){
        memory_x = internal_matrix[0][3];
    }
    if(internal_matrix[0][7] != 0){
        memory_y = internal_matrix[0][7];
    }
    if(internal_matrix[0][11] != 0){
        memory_z = internal_matrix[0][11];
    }

    //djup är x
    //horizantal är y
    //vertical är z
    
    output_array[0] = memory_x;
    output_array[1] = memory_y;
    output_array[2] = memory_z;

}



// Install the module. This code is executed during start-up.

static InitClass init("MarkToWorld", &MarkToWorld::Create, "Source/UserModules/MarkToWorld/");


