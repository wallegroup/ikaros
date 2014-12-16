//
//	LogicModelModule.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2014 <Robert Bagge, Olof Knape, Henrik Siljestråle>
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

#ifndef LogicModelModule_
#define LogicModelModule_

#include "IKAROS.h"

class LogicModelModule: public Module
{
public:
    static Module * Create(Parameter * p) { return new LogicModelModule(p); }

    LogicModelModule(Parameter * p) : Module(p) {}
    virtual ~LogicModelModule();

    void 		Init();
    void 		Tick();
    
    //Computes the goal position for servo 0
    float       computePositionHorizontal(float, float, float, float);
    
    //Computes the goal position for servo 2
    float       computePositionVertical(float, float,float,float, float, float);

    //Help function
    float       angleBetweenTwoVectors(float, float, float, float);
    
    //Help function
    float       absOfVector2(float, float);
    
    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;

    float *     output_array;
    int         output_array_size;


    // internal data storage

    float *     internal_array;
    float **    internal_matrix;
    
    // The offset in degrees from the real 0
    // for the servo to the 0 of the logical model.
    // The real 0 for servo 0 is straight back.
    int         servo_p0_offset;
    
    //How many degrees from the logical 0 the servo can turn to the left.
    int         servo_p0_max;
    
    //How many degrees from the logical 0 the servo can turn to the right.
    int         servo_p0_min;
    
    //The real 0 for servo 1 is straight down
    int         servo_p1_offset;
    int         servo_p1_max;
    int         servo_p1_min;
    
    //The real 0 for servo 2 is straight down
    int         servo_p2_offset;
    int         servo_p2_max;
    int         servo_p2_min;

    //The distance from the camera center to the foot of the Dynamixal-robot in every direction. Note that this is on the ikaros-coordination system, x - is the depth, y - is the width and z is the height.
    int         camera_x_offset;
    int         camera_y_offset;
    int         camera_z_offset;

};

#endif
