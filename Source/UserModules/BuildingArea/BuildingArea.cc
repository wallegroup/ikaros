//
//	BuildingArea.cc		This file is a part of the IKAROS project
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
//  This example is intended as a starting point for writing new Ikaros modules
//  The example includes most of the calls that you may want to use in a module.
//  If you prefer to start with a clean example, use he module MinimalModule instead.
//

#include "BuildingArea.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
BuildingArea::Init()
{
    //Get the boundary values for the BuildingArea
    Bind(boundary_x_furthest, "boundary_x_furthest");
    Bind(boundary_x_closest, "boundary_x_closest");
    Bind(boundary_y_right, "boundary_y_right");
    Bind(boundary_y_left, "boundary_y_left");
    Bind(boundary_z_top, "boundary_z_top");
    Bind(boundary_z_bottom, "boundary_z_bottom");
    Bind(focus_point_z, "focus_point_z");
    

    
    // Get pointer to a matrix and treat it as a matrix. If an array is
    // connected to this input, size_y will be 1.

    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");

    // Do the same for the outputs

    output_matrix = GetOutputMatrix("OUTPUT");
    output_matrix_size_x = GetOutputSizeX("OUTPUT");//Number of values per marker
    output_matrix_size_y = GetOutputSizeY("OUTPUT");//Number of markers
    
    output_BACoords = GetOutputMatrix("BACoords");
    output_BACoords_size_x = GetOutputSizeX("BACoords");
    output_BACoords_size_y = GetOutputSizeY("BACoords");

    // Allocate some data structures to use internaly
    // in the module

    // Create an array with ten elements
    // To access the array use internal_array[i].

    internal_array = create_array(3);

    // Create a matrix with the same size as INPUT2
    // IMPORTANT: For the matrix the sizes are given as X, Y
    // which is the OPPOSITE of ROW, COLUMN.

    internal_matrix = create_matrix(input_matrix_size_x, input_matrix_size_y);

    // To acces the matrix use internal_matrix[y][x].
    //
    // IMPORTANT: y is the first index and x the second,
    //
    // It is also possible to use the new operator to
    // create arrays, but create_array and create_matix
    // should be used to make sure that memeory is
    // allocated in a way that is suitable for the math
    // library and fast copying operations.
    
    markerCoordinates = create_matrix(5, output_matrix_size_y);
    output_BACoords[0][0] = (boundary_x_closest + boundary_x_furthest)/2; //Compute the center of the BA (X-axis) and store it in the constant output BACoords
    output_BACoords[0][1] = (boundary_y_left + boundary_y_right)/2; //Compute the center of the BA (Y-axis) and store it in the constant output BACoords
    output_BACoords[0][2] = focus_point_z;
    //Store the boundary values of the BA
    output_BACoords[1][0] = boundary_x_closest;
    output_BACoords[1][1] = boundary_x_furthest;
    output_BACoords[2][0] = boundary_y_left;
    output_BACoords[2][1] = boundary_y_right;
    output_BACoords[3][0] = boundary_z_bottom;
    output_BACoords[3][1] = boundary_z_top;
    
}



BuildingArea::~BuildingArea()
{
    // Destroy data structures that you allocated in Init.

    destroy_array(internal_array);
    destroy_matrix(internal_matrix);
    destroy_matrix(markerCoordinates);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}

void
BuildingArea::Tick()
{

    copy_matrix(internal_matrix, input_matrix, input_matrix_size_x, input_matrix_size_y);
    
    int i = 0;
    int j = 0;
    // loop through every input row until empty row is found
    while(i < input_matrix_size_y && internal_matrix[i][16] != 0){
        
        // find row (j) in markerCoordinates that is empty OR has the same ID as detected marker
        while(j < output_matrix_size_y && markerCoordinates[j][0] != 0 && markerCoordinates[j][0] != internal_matrix[i][16]){
            j++;
        }
        markerCoordinates[j][0] = internal_matrix[i][16]; //The ID of the marker
        markerCoordinates[j][1] = internal_matrix[i][3]; //The x-value
        markerCoordinates[j][2] = internal_matrix[i][7]; //The y-value
        markerCoordinates[j][3] = internal_matrix[i][11]; //The z-value
        
        // if stored marker is inside the building area, set 1 for yes
        if(markerCoordinates[j][1] > boundary_x_closest && markerCoordinates[j][1] < boundary_x_furthest && markerCoordinates[j][2] > boundary_y_left && markerCoordinates[j][2] < boundary_y_right && markerCoordinates[j][3] > boundary_z_bottom && markerCoordinates[j][3] < boundary_z_top)
        {
            markerCoordinates[j][4] = 1;
        }else{
            markerCoordinates[j][4] = 0;
        }
        i++;
        j = 0;
    }
    
    copy_matrix(output_matrix, markerCoordinates, 5, output_matrix_size_y);
}



// Install the module. This code is executed during start-up.

static InitClass init("BuildingArea", &BuildingArea::Create, "Source/UserModules/BuildingArea/");


