//
//	AverageMatrixValue.cc		This file is a part of the IKAROS project
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

#include "AverageMatrixValue.h"

using namespace ikaros;


void
AverageMatrixValue::Init()
{
    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    Bind(threshold,"threshold");
    value=0;
    size=0;
}



AverageMatrixValue::~AverageMatrixValue()
{

}

void
AverageMatrixValue::Tick()
{
        for (int j=0; j<input_matrix_size_y * input_matrix_size_x ; j++){
           if(input_matrix[0][j]>threshold){
            value = value + input_matrix[0][j];
            size++;
        }
        }
        if(value==0 && size ==0){
            output_array[0]=0;
        }else{
            output_array[0] = value/size;
        }  
    value=0;
    size=0;
}

static InitClass init("AverageMatrixValue", &AverageMatrixValue::Create, "Source/UserModules/AverageMatrixValue/");


