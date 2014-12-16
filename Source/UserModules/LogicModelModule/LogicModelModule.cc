//
//	LogicModelModule.cc		This file is a part of the IKAROS project
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

#include "LogicModelModule.h"
using namespace ikaros;


void
LogicModelModule::Init()
{
    
    Bind(camera_x_offset, "camera_x_offset");
    Bind(camera_y_offset, "camera_y_offset");
    Bind(camera_z_offset, "camera_z_offset");

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");


    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


    internal_array = create_array(10);
    internal_matrix = create_matrix(2, 2);
    
    servo_p0_offset = 180;
    servo_p0_max = 270;
    servo_p0_min = 90;
    
    servo_p1_offset = 0;
    servo_p1_max = 270;
    servo_p1_min = 90;
    
    servo_p2_offset = 180;
    servo_p2_max = 270;
    servo_p2_min = 50;
    
    camera_z_offset = camera_z_offset + (-175);
}



LogicModelModule::~LogicModelModule()
{
    // Destroy data structures that you allocated in Init.
    
    destroy_array(internal_array);
    destroy_matrix(internal_matrix);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}



void
LogicModelModule::Tick()
{
    //Fill the output array with the positions for the three different servos.
    int input_x = input_array[0]+camera_x_offset;
    int input_y = input_array[1]+camera_y_offset;
    int input_z = input_array[2]+camera_z_offset;


    //It has to be Integers that are sent to the Dynamixel
    int position_p0 = computePositionHorizontal(0, 2, input_y, input_x);
    int position_p2 = computePositionVertical(0, 0, 2, input_y, input_z, input_x);
    output_array[0] = position_p0;
    output_array[1] = 180; //At this point the position of the second servo is always 180 (pointing straight up)
    output_array[2] = position_p2;
}

//Uses a calibration vector to find the desired goal position to reach point y,x in the horizontal plane with the gaze.
float
LogicModelModule::computePositionHorizontal(float cal_vector_y, float cal_vector_x, float goal_vector_y, float goal_vector_x)
{
    float position = angleBetweenTwoVectors(cal_vector_y, cal_vector_x, goal_vector_y, goal_vector_x);
    if(goal_vector_y>0){
        position = (position)*(180/pi) + servo_p0_offset; // (180/pi) omvandlar från radianer till grader.
    }else{
        position = -(position)*(180/pi) + servo_p0_offset;
    }
     
    if(position < servo_p0_min){
        return servo_p0_min;
    }else if(position > servo_p0_max){
        return servo_p0_max;
    }
    
    return position;
}

//Uses a calibration vector to find the desired goal position (degrees) for servo 2 to reach a particular point with the gaze of the robot. Note that a new coordinate system is formed before the calculations are done. This is to be able to do the calculations in 2 dimensions instead of 3.
float
LogicModelModule::computePositionVertical(float cal_vector_y, float cal_vector_z, float cal_vector_x, float goal_vector_y, float goal_vector_z, float goal_vector_x){
    float cal_axis_x = absOfVector2(cal_vector_y, cal_vector_x);
    float goal_axis_x = absOfVector2(goal_vector_y, goal_vector_x);

    float position = angleBetweenTwoVectors(cal_axis_x, cal_vector_z, goal_axis_x, goal_vector_z);
    if(goal_vector_z>0){
        position = (position)*(180/pi) + servo_p2_offset;
    }else{
        position = -(position)*(180/pi) + servo_p2_offset;
    }

    if(position < servo_p2_min){
        return servo_p2_min;
    }else if(position > servo_p2_max){
        return servo_p2_max;
    }
    return position;

}

//Calculates the angle between two 2-dimensional vectors in degrees.
float
LogicModelModule::angleBetweenTwoVectors(float vector_1_a, float vector_1_b, float vector_2_a, float vector_2_b){
   return acos((vector_1_a * vector_2_a + vector_1_b * vector_2_b)/(absOfVector2(vector_1_a, vector_1_b) * absOfVector2(vector_2_a, vector_2_b)));
}

//Calculates the length of a 2-dimensional vector
float
LogicModelModule::absOfVector2(float a, float b){
    return sqrt(a*a + b*b);
}

static InitClass init("LogicModelModule", &LogicModelModule::Create, "Source/UserModules/LogicModelModule/");


