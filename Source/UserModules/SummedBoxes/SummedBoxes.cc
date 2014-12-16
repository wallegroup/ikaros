//
//	SummedBoxes.cc		This file is a part of the IKAROS project
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


#include "SummedBoxes.h"

using namespace ikaros;


void
SummedBoxes::Init()
{
    Bind(box_count, "box_count");

    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    output_array_box = GetOutputArray("OUTPUT_BOX");
    output_array_box_size = GetOutputSize("OUTPUT_BOX");

    boxSize = (int) input_matrix_size_x/box_count;
    // sAT = summed Area Table
    //col,row
    sAT = create_matrix(input_matrix_size_x, input_matrix_size_y);

}



SummedBoxes::~SummedBoxes()
{
    destroy_matrix(sAT);
}



void
SummedBoxes::Tick()
{
    // create the Summed Area Table
    integral_image(sAT, input_matrix, input_matrix_size_x, input_matrix_size_y);

    /*
    int pos=0;
    for (int i=0; i < box_count; i++) {
        //A - B - C + D
        output_array[i] = sAT[0][pos] - sAT[0][pos+boxSize-1] - sAT[input_matrix_size_y-1][pos] + sAT[input_matrix_size_y-1][pos+boxSize-1];
        pos+=boxSize;
    }  
*/
    output_array[0] = sAT[0][0] - sAT[0][127] - sAT[input_matrix_size_y-1][0] +sAT[input_matrix_size_y-1][127];
    output_array[1] = sAT[0][128] - sAT[0][207] - sAT[input_matrix_size_y-1][128] + sAT[input_matrix_size_y-1][207];
    output_array[2] = sAT[0][208] -sAT[0][423] -sAT[input_matrix_size_y-1][208] +sAT[input_matrix_size_y-1][423];  
    output_array[3] = sAT[0][424] -sAT[0][511] -sAT[input_matrix_size_y-1][424] +sAT[input_matrix_size_y-1][511]; 
    output_array[4] = sAT[0][512] -sAT[0][639] -sAT[input_matrix_size_y-1][512] +sAT[input_matrix_size_y-1][639]; 

    output_array_box[0] = sAT[0][0] - sAT[0][60] - sAT[input_matrix_size_y-1][0] + sAT[input_matrix_size_y-1][60];
    output_array_box[1] = sAT[0][input_matrix_size_x-61] - sAT[0][input_matrix_size_x-1] - sAT[input_matrix_size_y-1][input_matrix_size_x-61] + sAT[input_matrix_size_y-1][input_matrix_size_x-1];          
}

static InitClass init("SummedBoxes", &SummedBoxes::Create, "Source/UserModules/SummedBoxes/");


