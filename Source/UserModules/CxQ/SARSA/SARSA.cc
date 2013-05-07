//
//	SARSA.cc		This file is a part of the IKAROS project
//                          Implements a simple maze generator
//
//    Copyright (C) 2009 Christian Balkenius
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



#include "SARSA.h"

using namespace ikaros;



SARSA::SARSA(Parameter * p):Module(p) 
{
	action_length = GetIntValue("actions");

	epsilon	= GetFloatValue("epsilon");
	gamma	= GetFloatValue("gamma");
	epochs  = GetIntValue("epochs");

	rand_gen = new TRanrotWGenerator((uint32)rand());	// random double

	tick = 0;
	print = 0;
	last_print_tick = 0;
}



SARSA::~SARSA()
{
	delete rand_gen;
}



void
SARSA::SetSizes()
{
	if(action_length == unknown_size)
		Notify(msg_fatal_error, "Module \"%s\": The 'actions' parameter is invalid.\n", GetName());
	
	SetOutputSize("ACTION", action_length);
	SetOutputSize("TARGET", action_length);
}



void
SARSA::Init()
{
	new_actions = GetInputArray("ACTION");
	old_actions = GetInputArray("LAST_ACTION");
	old_reward  = GetInputArray("REINFORCEMENT");
	
	actions_out	= GetOutputArray("ACTION");
	target_out	= GetOutputArray("TARGET");
}



void
SARSA::Tick()
{
	int old_a = Select(old_actions);
	int new_a = Select(new_actions);
	float new_target = *old_reward;

	// use a finite horizon
	if(*old_reward <= 0)
		new_target += gamma * new_actions[new_a];

	// output for the world module
	reset_array(actions_out, action_length);
	actions_out[new_a] = 1;

	// output for the learning module
	copy_array(target_out, old_actions, action_length);
	target_out[old_a] = new_target;

	if(epochs > 0)
	{
		if(*old_reward > 0)	// Arrived at goal state
		{
			printf("%i\n", tick - last_print_tick);
			last_print_tick = tick;
			++print;
		}
		
		if(print >= epochs)
			Notify(msg_terminate, "End\n");
		
		++tick;
	}
}



int
SARSA::Select(float * actions)
{
	if(rand_gen->Random() < epsilon)
		return rand_gen->IRandom(0, action_length-1);

	int high = arg_max(actions, action_length);
	int low = arg_min(actions, action_length);
	if(actions[high] == actions[low])
		return rand_gen->IRandom(0, action_length-1);

	return high;
}


static InitClass init("SARSA", &SARSA::Create, "Source/UserModules/CxQ/SARSA/");