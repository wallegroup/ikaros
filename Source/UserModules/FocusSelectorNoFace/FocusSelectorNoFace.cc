//
//	MyModule.cc		This file is a part of the IKAROS project
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

#include "FocusSelectorNoFace.h"
// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
FocusSelectorNoFace::Init()
{

    input_array_doc = GetInputArray("INPUT_DOC");
    input_array_doc_size = GetInputSize("INPUT_DOC");

    input_matrix_markers = GetInputMatrix("INPUT_MARKERS");
    input_matrix_markers_size_x = GetInputSizeX("INPUT_MARKERS");
    input_matrix_markers_size_y = GetInputSizeY("INPUT_MARKERS");

    input_matrix_ba = GetInputMatrix("INPUT_BA");
    input_matrix_ba_size_x = GetInputSizeX("INPUT_BA");
    input_matrix_ba_size_y = GetInputSizeY("INPUT_BA");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    output_array2 = GetOutputArray("OUTPUT2");
    output_array2_size = GetOutputSize("OUTPUT2");

    internal_array = create_array(3);

    moving_marker = false;
    z_constant =-37;
}


FocusSelectorNoFace::~FocusSelectorNoFace()
{

}



void
FocusSelectorNoFace::Tick()
{


//----------------MOVING MARKERS----------------
for(int i = 0; i < input_matrix_markers_size_y; i++){
    // if the marker is outside the BA and if the marker has moved.
    if(input_matrix_markers[i][4]==0 && input_matrix_markers[i][3] - z_constant > 40 ){
        moving_marker=true;
        break;
    }else{
        moving_marker=false;
    }
}

if(moving_marker){
    output_array[0]=input_matrix_ba[0][0];
    output_array[1]=input_matrix_ba[0][1];
    output_array[2]=-37;
    
}else{



//-------------HAND DIRECTION--------------------
    if(input_array_doc[0]==2){  //found change to the right
        for(int i = 0; i < input_matrix_markers_size_y; i++){
            if(input_matrix_markers[i][2] < input_matrix_ba[2][0]){
                //found marker to the right
                output_array[0]=input_matrix_markers[i][1];
                output_array[1]=input_matrix_markers[i][2];
                output_array[2]=input_matrix_markers[i][3];
                break;
            }
        }

    }else if(input_array_doc[0]==1){  //found change to the left
        for(int i = 0; i < input_matrix_markers_size_y; i++){
            if(input_matrix_markers[i][2] > input_matrix_ba[2][1]){
                //Found marker to the left
                output_array[0]=input_matrix_markers[i][1];
                output_array[1]=input_matrix_markers[i][2];
                output_array[2]=input_matrix_markers[i][3];
                break;
            }
        }

    }else{
        output_array[0]=internal_array[0];
        output_array[1]=internal_array[1];
        output_array[2]=internal_array[2];
    }
}
    copy_array(internal_array,output_array,3);
}




static InitClass init("FocusSelectorNoFace", &FocusSelectorNoFace::Create, "Source/UserModules/FocusSelectorNoFace/");


