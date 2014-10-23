//
//	FaceLocation.cc		This file is a part of the IKAROS project
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

#include "FaceLocation.h"
using namespace ikaros;


void
FaceLocation::Init()
{

    Bind(fov_v, "fov_v");
    Bind(fov_h, "fov_h");
    Bind(xRes, "xRes");
    Bind(yRes, "yRes");
    Bind(camDepth, "camDepth");

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    memory_x = 0;
    memory_y = 0;
    memory_z = 0;
}



FaceLocation::~FaceLocation()
{
    // no internal arrays to destroy
}

void 
FaceLocation::depth_to_sensor_coords(float x, float y, float z, double xRes, double yRes, double fXToZ, double fYToZ)
{

    // compensate for perspective
    float ty = (float)((y - 0.5) * x * fYToZ);
    float tz = (float)((0.5 - z) * x * fXToZ);
    float tx = x;

    // shift to sensor/world coordinate system
    // x is pointing forwards and y to the side; z is up

    if(tx != 0){
        memory_x = tx;
    }
    if(ty != 0){
        memory_y = ty;
    }
    if(tz != 0){
        memory_z = tz;
    }

    output_array[0] = memory_x;
    output_array[1] = -memory_y;
    output_array[2] = memory_z;

}

void
FaceLocation::Tick()
{

    float camHorizontal = input_array[1];
    float camVertical = input_array[0];

    // Here we start with the conversion 
    double fXToZ = tan(fov_h/2)*2;
    double fYToZ = tan(fov_v/2)*2;

    depth_to_sensor_coords(camDepth, camVertical, camHorizontal, xRes, yRes, fXToZ, fYToZ);

    // xRes och yRes är upplösnigen på kameran (640 x 480)
    // fov_h och fov_v är kamerans vinkel i radianer (=1.1)

}



// Install the module. This code is executed during start-up.

static InitClass init("FaceLocation", &FaceLocation::Create, "Source/UserModules/FaceLocation/");


