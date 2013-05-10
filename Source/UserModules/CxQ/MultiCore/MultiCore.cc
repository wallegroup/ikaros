//
//	MultiCore.h		This file is a part of the IKAROS project
//
// 		Reinforcement learning module with separate learning module and action selection.
//
//    Copyright (C) 2007 Stefan Winberg
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
//	Created: 2007-05-08
//

#include "MultiCore.h"



MultiCore::~MultiCore()
{
	delete rand_gen;
}



void MultiCore::SetSizes(void)
{
	action_length = GetIntValue("actions");
	SetOutputSize("GRADIENT_TARGET", action_length);
	SetOutputSize("BIAS_TARGET", action_length);
	SetOutputSize("ACTION", action_length);
}



void MultiCore::Init(void)
{
	InGradient		= GetInputArray("GRADIENT");
	InLastGradient	= GetInputArray("LAST_GRAD");
	InBias			= GetInputArray("BIAS");
	InLastBias		= GetInputArray("LAST_BIAS");
	InPunish		= GetInputArray("PUNISH");
	InReward		= GetInputArray("REWARD");
	
	OutGradientTarget	= GetOutputArray("GRADIENT_TARGET");
	OutBiasTarget		= GetOutputArray("BIAS_TARGET");
	OutSelectedAction	= GetOutputArray("ACTION");
	
	discount = GetFloatValue("discount");
	infinite_horizon = GetIntValue("infinite_horizon");
	select = GetIntValueFromList("select");
	exploration = GetFloatValue("exploration");
	epochs = GetIntValue("epochs");
	
	tick = 0;
	print = 0;
	average = 0;
	last_print_tick = 0;
	last_action = -1;	// the action selected by the selection module based on the actor's last output

	rand_gen = new TRanrotWGenerator((uint32)rand());	// random double
}



void MultiCore::Tick(void)
{
	copy_array(OutGradientTarget, InLastGradient, action_length);
	
	if(last_action > -1)
	{
		if(*InReward > 0)	// reward the action that lead to the rewarding state
			OutGradientTarget[last_action] = *InReward;
		else	// the reward for the last tick is based on the ANN output this tick
			OutGradientTarget[last_action] = *InReward + discount * max(InGradient, action_length);

		OutSelectedAction[last_action] = 0;	// clear the old action
	}

	float tmp[] = {0,0,0,0};
	add(tmp, InPunish, action_length);
	add(tmp, InGradient, action_length);

	// select a new action
	if(zero(tmp, action_length))
	{
		last_action = rand_gen->IRandom(0, action_length-1);
	}
	else
	{
		if(select == 0)
		{
			// epsilon greedy
			last_action = 0;
			for(int i=1; i<action_length; ++i)
			{
				if(tmp[i] > tmp[last_action])
					last_action = i;
				else if(tmp[i] == tmp[last_action] && rand_gen->Random() < 0.5)
					last_action = i;
			}

			if(rand_gen->Random() < exploration)
			{
				int random_action;
				do
				{
					random_action = rand_gen->IRandom(0, action_length-1);
				}
				while(last_action == random_action);
			}
		}
		else
		{
			// boltzmann select
			last_action = -1;

			float low = min(tmp, action_length);
			if(low < 0)
				add(tmp, low, action_length);

			double sum = 0;
			double alpha = 1.0 / double(exploration);
			
			for(int i=0; i<action_length; ++i)
				sum += ::exp(alpha*double(tmp[i]));
			
			double rs = rand_gen->Random() * sum;
			double ss = 0;
			for (int i=0; i<action_length; i++)
			{
				ss += ::exp(alpha*double(tmp[i]));
				if (ss >= rs)
				{
					last_action = i;
					break;
				}
			}
		
			if(last_action < 0)
				last_action = rand_gen->IRandom(0, action_length-1);
		}
	}

	OutSelectedAction[last_action] = 1;	// set the next action

	if(epochs > 0 && *InReward > 0)	// arrived at goal state
	{
		++print;
		printf("%i\n", tick - last_print_tick);
		last_print_tick = tick;

		if(print >= epochs)
			Notify(msg_terminate, "End\n");
	}

	++tick;
}



static InitClass init("MultiCore", &MultiCore::Create, "Source/UserModules/CxQ/MultiCore/");