//
//	FocusSelector3.cc		This file is a part of the IKAROS project
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


#include "FocusSelector3.h"

using namespace ikaros;


void
FocusSelector3::Init()
{
    input_array_doc = GetInputArray("INPUT_DOC");
    input_array_doc_size = GetInputSize("INPUT_DOC");

    input_array_face = GetInputArray("INPUT_FACE");
    input_array_face_size = GetInputSize("INPUT_FACE");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    internal_array = create_array(3);

    cubeInHand = false;
    t=0;
}


FocusSelector3::~FocusSelector3()
{

}

void
FocusSelector3::Tick()
{
//---------No motions in the picture------------
if(input_array_doc[0]==0 && input_array_doc[1]==0){
    t++;
}else{
    t=0;
}
if(t>8){
    output_array[0]=input_array_face[0];
    output_array[1]=input_array_face[1];
    output_array[2]=input_array_face[2];
    copy_array(internal_array,output_array,3);
    return;
}

//-------Motion in the picture-------------------
if(!cubeInHand){
    if(input_array_doc[1]==1 && input_array_doc[0]==1){
        cubeInHand=true;
    }else if(input_array_doc[1]==2 && input_array_doc[0]==2){
        cubeInHand=true;
    }
    //Left
    if(input_array_doc[0]==1){
        output_array[0]=500;
        output_array[1]=300;
        output_array[2]=-30;
    }
    //Right
    else if(input_array_doc[0]==2){
        output_array[0]=500;
        output_array[1]=-350;
        output_array[2]=-30;
    }else{
        output_array[0]=internal_array[0];
        output_array[1]=internal_array[1];
        output_array[2]=internal_array[2];
    }
    //Middle
    }else{
        output_array[0]=700;
        output_array[1]=0;
        output_array[2]=0;
        if(input_array_doc[0]==3){
            cubeInHand=false;
        }
    }
copy_array(internal_array,output_array,3);
}




static InitClass init("FocusSelector3", &FocusSelector3::Create, "Source/UserModules/FocusSelector3/");


