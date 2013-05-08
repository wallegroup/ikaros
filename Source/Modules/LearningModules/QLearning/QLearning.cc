//
//	QLearning.cc		This file is a part of the IKAROS project
//                      Implementation of Q-learning
//
//    Copyright (C) 2003 Christian Balkenius
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
//	Created: 2003-08-02
//

#include "QLearning.h"
#include <cfloat>


using namespace ikaros;

void
QLearning::Init()
{
	horizon     = GetIntValueFromList("horizon");
	alpha		= GetFloatValue("alpha");
	gamma		= GetFloatValue("gamma");
	epsilon		= GetFloatValue("epsilon");
	initial     = GetFloatValue("initial");
	end_epochs  = GetIntValue("epochs");

	state           = GetInputMatrix("STATE");
	size_x          = GetInputSizeX("STATE");
	size_y          = GetInputSizeY("STATE");

	reinforcement	= GetInputArray("REINFORCEMENT");
	action          = GetOutputArray("ACTION");
	select          = GetOutputArray("SELECT");

	rand_gen = new TRanrotWGenerator((uint32)rand());	// random double

	Q[0]             = set_matrix(create_matrix(size_x, size_y), initial, size_x, size_y);
	Q[1]             = set_matrix(create_matrix(size_x, size_y), initial, size_x, size_y);
	Q[2]             = set_matrix(create_matrix(size_x, size_y), initial, size_x, size_y);
	Q[3]             = set_matrix(create_matrix(size_x, size_y), initial, size_x, size_y);
    
	for(int j=0; j<size_y; j++)
		for(int i=0; i<size_x; i++)
			for(int a = 0; a<4; a++)
				Q[a][j][i] = rand_gen->Random() * initial;

	value            = GetOutputMatrix("VALUE");
    
	last_action     = -1;
    
	steps = 0;
	epochs = 0;

	// Fill in initial values
	/*
	for(int j=0; j<size_y; j++)
	{
		for(int i=0; i<size_x; i++)
		{
			value[j][i] = 0;
			for(int a = 0; a < 4; a++)
				value[j][i] = max(Q[a][j][i], value[j][i]);
		}
	}*/
}



QLearning::~QLearning()
{
	destroy_matrix(Q[0]);
	destroy_matrix(Q[1]);
	destroy_matrix(Q[2]);
	destroy_matrix(Q[3]);
}



void
QLearning::Tick()
{
    int x = 0, y = 0;
    arg_max(x, y, state, size_x, size_y);
	
	// Evaluate last action

    if(last_action != -1)
    {
        float V = FLT_MIN;
        for(int i = 0; i < 4; i++)
            V = max(Q[i][y][x], V);
        
        switch(horizon)
        {
            case 0: // infinite
                V += *reinforcement;

            case 1: // finite
                if(*reinforcement > 0)
                    V = *reinforcement;
				else
					V += *reinforcement;
        }
        
        Q[last_action][last_y][last_x] += alpha * (gamma * V - Q[last_action][last_y][last_x]);

		if(*reinforcement > 0.99)
		{
			printf("%i\n", steps);
			steps = 0;
			epochs++;

			if(end_epochs > 0 && epochs >= end_epochs)
				Notify(msg_terminate, "End\n");
		}
		else
		{
			steps++;
		}

		value[last_y][last_x] = 0;
		for(int a = 0; a<4; a++)
			value[last_y][last_x] += Q[a][last_y][last_x] * 0.25f;
	}
    
	// Produce new action
	
    int a = rand_gen->IRandom(0, 3);
	if(rand_gen->Random() > epsilon)
    {
        float action_strength[4];
		for(int i = 0; i < 4; i++)
			action_strength[i] = Q[i][y][x];

		int high = arg_max(action_strength, 4);
		int low = arg_min(action_strength, 4);
		if(action_strength[high] != action_strength[low])
			a = high;	// pick a random action if all values are the same

		for(int i = 0; i < 4; i++)
			select[i] = Q[i][y][x];
	}
	else
	{
		reset_array(select, 4);
		select[a] = 1;
	}

	reset_array(action, 4);
	action[a] = 1;

	/*
	value[last_y][last_x] = FLT_MIN;
	for(int i = 0; i < 4; i++)
        value[last_y][last_x] = max(Q[i][last_y][last_x], value[last_y][last_x]);
	*/

	// Remember action and state

	last_action = a;
    last_x = x;
    last_y = y;
}

static InitClass init("QLearning", &QLearning::Create, "Source/Modules/LearningModules/QLearning/");