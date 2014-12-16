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

#include "FocusSelector.h"
// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
FocusSelector::Init()
{
    tickCounter = 0;

    input_array_change = GetInputArray("INPUT_CHANGE");
    input_array_change_size = GetInputSize("INPUT_CHANGE");

    input_array_face = GetInputArray("INPUT_FACE");
    input_array_face_size = GetInputSize("INPUT_FACE");

    input_array_marker = GetInputArray("INPUT_MARKER");
    input_array_marker_size = GetInputSize("INPUT_MARKER");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


}



FocusSelector::~FocusSelector()
{
    // Destroy data structures that you allocated in Init.

    destroy_array(internal_array);
    destroy_matrix(internal_matrix);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}



void
FocusSelector::Tick()
{

    if(input_array_change[0]>0.02 ){
            output_array[0] = input_array_marker[0];
            output_array[1] = input_array_marker[1];
            output_array[2] = input_array_marker[2];
            tickCounter=0;
    }else{
        tickCounter++;
    }

    if(tickCounter>20){
        output_array[0] = input_array_face[0];
        output_array[1] = input_array_face[1];
        output_array[2] = input_array_face[2];
    }

}



// Install the module. This code is executed during start-up.

static InitClass init("FocusSelector", &FocusSelector::Create, "Source/UserModules/FocusSelector/");


