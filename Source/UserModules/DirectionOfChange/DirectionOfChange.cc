//
//	DirectionOfChange.cc		This file is a part of the IKAROS project
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

#include "DirectionOfChange.h"

using namespace ikaros;


void
DirectionOfChange::Init()
{
    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");
}



DirectionOfChange::~DirectionOfChange()
{

}



void
DirectionOfChange::Tick()
{
    if(input_array[0]>20 || input_array[1]>250){
        output_array[0]=1;
    }else if(input_array[3]>250 || input_array[4]>20){
        output_array[0]=2;
    }else{
        output_array[0]=0;
    }
    
    /*if(input_array[0]>20 ){
        output_array[0]=1;
    }else if(input_array[4]>20){
        output_array[0]=2;
    }else{
        output_array[0]=0;
    }*/
}

static InitClass init("DirectionOfChange", &DirectionOfChange::Create, "Source/UserModules/DirectionOfChange/");


