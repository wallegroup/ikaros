//
//	FocusSelector2.cc		This file is a part of the IKAROS project
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


#include "FocusSelector2.h"

using namespace ikaros;


void
FocusSelector2::Init()
{

    input_array_doc = GetInputArray("INPUT_DOC");
    input_array_doc_size = GetInputSize("INPUT_DOC");

    input_array_face = GetInputArray("INPUT_FACE");
    input_array_face_size = GetInputSize("INPUT_FACE");

    input_array_change = GetInputArray("INPUT_CHANGE");
    input_array_change_size = GetInputSize("INPUT_CHANGE");

    input_matrix_markers = GetInputMatrix("INPUT_MARKERS");
    input_matrix_markers_size_x = GetInputSizeX("INPUT_MARKERS");
    input_matrix_markers_size_y = GetInputSizeY("INPUT_MARKERS");

    input_matrix_ba = GetInputMatrix("INPUT_BA");
    input_matrix_ba_size_x = GetInputSizeX("INPUT_BA");
    input_matrix_ba_size_y = GetInputSizeY("INPUT_BA");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    internal_array = create_array(3);

    Bind(z_movementThreshold, "z_movementThreshold");
    Bind(tick_faceThreshold, "tick_faceThreshold");
    Bind(change_threshold, "change_threshold");

    t=0;
    moving_marker = false;
}


FocusSelector2::~FocusSelector2()
{

}

void
FocusSelector2::Tick()
{
// ----------FACE-------------
if(input_array_change[0]<=change_threshold ){
    t++;
}else{
    t=0;
}
if(t>tick_faceThreshold){
    output_array[0]=input_array_face[0];
    output_array[1]=input_array_face[1];
    output_array[2]=input_array_face[2];
    copy_array(internal_array,output_array,3);
    return;
}

//----------------MOVING MARKERS----------------
for(int i = 0; i < input_matrix_markers_size_y; i++){
    // if the marker is outside the BA and if the marker has moved.
    if(input_matrix_markers[i][4]==0 && input_matrix_markers[i][3] > z_movementThreshold){
        moving_marker=true;
        break;
    }else{
        moving_marker=false;
    }
}
if(moving_marker){
    output_array[0]=input_matrix_ba[0][0];
    output_array[1]=input_matrix_ba[0][1];
    output_array[2]=input_matrix_ba[0][2];
}else{


//-------------HAND DIRECTION------------------
    //Check how many markers that are outside the Building area
    int markersOutsideBA=0;
    for(int i = 0; i < input_matrix_markers_size_y; i++){
        if(input_matrix_markers[i][0] > 0 && input_matrix_markers[i][4]==0){
            markersOutsideBA++;
        }
    }
    if(input_array_doc[0]==2 && markersOutsideBA>1){  //found change to the right
        for(int i = 0; i < input_matrix_markers_size_y; i++){
            if(input_matrix_markers[i][2] < input_matrix_ba[2][0]){
                //found marker to the right
                output_array[0]=input_matrix_markers[i][1];
                output_array[1]=input_matrix_markers[i][2];
                output_array[2]=input_matrix_markers[i][3];
                break;
            }
        }
    }else if(input_array_doc[0]==1 && markersOutsideBA>1){  //found change to the left
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
        //If the marker is not found, output the cord from last tick
        output_array[0]=internal_array[0];
        output_array[1]=internal_array[1];
        output_array[2]=internal_array[2];
    }
}
    copy_array(internal_array,output_array,3);
}




static InitClass init("FocusSelector2", &FocusSelector2::Create, "Source/UserModules/FocusSelector2/");


