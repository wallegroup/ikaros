//
//	ConstantOutput.cc		This file is a part of the IKAROS project
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
//  This example is intended as a starting point for writing new Ikaros modules
//  The example includes most of the calls that you may want to use in a module.
//  If you prefer to start with a clean example, use he module MinimalModule instead.
//

#include "ConstantOutput.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
ConstantOutput::Init()
{
    // To get the parameters from the IKC file, use the Bind
    // function for each parameter. The parameters are initialized
    // from the IKC and can optionally be changed from the
    // user interface while Ikaros is running. If the parameter is not
    // set, the default value will be used instead.
    
    Bind(float_parameter, "parameter1");
    Bind(int_parameter, "parameter2");
    
    // This is were we get pointers to the inputs and outputs

    // Get a pointer to the input INPUT1 and its size which we set
    // to 10 above
    // It does not matter whether a matrix of array is connected
    // to the inputs. We will treat it an array in this module
    // anyway.


    output_matrix = GetOutputMatrix("OUTPUT1");
    output_matrix_size_x = GetOutputSizeX("OUTPUT1");
    output_matrix_size_y = GetOutputSizeY("OUTPUT1");

    output_matrix2 = GetOutputMatrix("OUTPUT2");
    output_matrix2_size_x = GetOutputSizeX("OUTPUT2");
    output_matrix2_size_y = GetOutputSizeY("OUTPUT2");
    
    output_array = GetOutputArray("OUTPUT3");
    output_array_size = GetOutputSize("OUTPUT3");

    // Allocate some data structures to use internaly
    // in the module

    // Create an array with ten elements
    // To access the array use internal_array[i].

    internal_array = create_array(10);

    // Create a matrix with the same size as INPUT2
    // IMPORTANT: For the matrix the sizes are given as X, Y
    // which is the OPPOSITE of ROW, COLUMN.


    // To acces the matrix use internal_matrix[y][x].
    //
    // IMPORTANT: y is the first index and x the second,
    //
    // It is also possible to use the new operator to
    // create arrays, but create_array and create_matix
    // should be used to make sure that memeory is
    // allocated in a way that is suitable for the math
    // library and fast copying operations.
    matrix = create_matrix(4,3);
    matrix2 = create_matrix(4,3);
}



ConstantOutput::~ConstantOutput()
{
    // Destroy data structures that you allocated in Init.

    destroy_array(internal_array);
    destroy_matrix(internal_matrix);
    destroy_matrix(matrix);
    destroy_matrix(matrix2);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}



void
ConstantOutput::Tick()
{

    matrix[0][0]=1;
    matrix[0][1]=2;
    matrix[0][2]=3;
    matrix[0][3]=4;

    matrix[1][0]=8;
    matrix[1][1]=7;
    matrix[1][2]=6;
    matrix[1][3]=5;

    matrix[2][0]=4;
    matrix[2][1]=3;
    matrix[2][2]=2;
    matrix[2][3]=1;

    //copy_matrix(matrix2,matrix,640,480);
    copy_matrix(output_matrix,matrix,4,3);
    integral_image(matrix2, matrix, 4, 3);
    copy_matrix(output_matrix2,matrix2,4,3);

    output_array[0] = output_matrix2[0][4];

    //copy_matrix(output_matrix,matrix,3,3);
}



// Install the module. This code is executed during start-up.

static InitClass init("ConstantOutput", &ConstantOutput::Create, "Source/UserModules/ConstantOutput/");


