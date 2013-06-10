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
	destroy_array(ActionScore);
	destroy_array(LastPunish);
}



void MultiCore::SetSizes(void)
{
	action_length = GetIntValue("actions");
	SetOutputSize("GRADIENT_TARGET", action_length);
	SetOutputSize("BIAS_TARGET", action_length);
	SetOutputSize("EXPLORE_TARGET", action_length);
	SetOutputSize("ACTION", action_length);
}

void MultiCore::Init(void)
{
	InGradient		= GetInputArray("GRADIENT");
	InLastGradient	= GetInputArray("LAST_GRAD");
	InBias			= GetInputArray("BIAS");
	InLastBias		= GetInputArray("LAST_BIAS");
	InExplore		= GetInputArray("EXPLORE");
	InLastExplore	= GetInputArray("LAST_EXPLORE");
	InPunish		= GetInputArray("PUNISH");
	InReward		= GetInputArray("REWARD");
	
	OutGradientTarget	= GetOutputArray("GRADIENT_TARGET");
	OutBiasTarget		= GetOutputArray("BIAS_TARGET");
	OutExploreTarget	= GetOutputArray("EXPLORE_TARGET");
	OutSelectedAction	= GetOutputArray("ACTION");
	
	discount = GetFloatValue("discount");
	infinite_horizon = GetIntValue("infinite_horizon");
	select = GetIntValueFromList("select");
	exploration = double(GetFloatValue("exploration"));
	epochs = GetIntValue("epochs");

	ActionScore = create_array(action_length);
	LastPunish = create_array(action_length);
	
	tick = 0;
	print = 0;
	average = 0;
	last_print_tick = 0;
	last_action = -1;	// the action selected by the selection module based on the actor's last output

	rand_gen = new TRanrotWGenerator((uint32)rand());	// random double
}



void MultiCore::Tick(void)
{
	copy_array(ActionScore, InGradient, action_length);
	add(ActionScore, InPunish, action_length);

	// train
	if(last_action > -1)
	{
		copy_array(OutGradientTarget, InLastGradient, action_length);
		copy_array(OutBiasTarget, InLastBias, action_length);
		copy_array(OutExploreTarget, InLastExplore, action_length);

		add(OutExploreTarget, LastPunish, action_length);
		OutExploreTarget[last_action] = discount * max(InLastExplore, action_length) - 0.01f;

		if(*InReward > 0)	// reward the action that lead to the rewarding state
			OutGradientTarget[last_action] = *InReward;
		else	// the reward for the last tick is based on the ANN output this tick
			OutGradientTarget[last_action] = discount * max(ActionScore, action_length) + *InReward;

		if(OutGradientTarget[last_action] > 0)
		{
			if(OutGradientTarget[last_action] * discount >= InLastGradient[last_action])
				OutBiasTarget[last_action] = 1;
			else
				OutBiasTarget[last_action] = -1;
		}

		OutSelectedAction[last_action] = 0;	// clear the previous action
	}

	add(ActionScore, InBias, action_length);
	add(ActionScore, InExplore, action_length);

	// select a new action
	int action = rand_gen->IRandom(0, action_length-1);
	if(!zero(ActionScore, action_length))
	{
		if(select == 0)	// greedy
		{
			if(rand_gen->Random() > exploration)
				for(int i=0; i<action_length; ++i)
					if(ActionScore[i] > ActionScore[action])
						action = i;
		}
		else
		{
			if(exploration > 0.0015)	// boltzmann
			{
				normalize(ActionScore, action_length);

				double sum = 0;
				double alpha = 1 / exploration;
				for(int i=0; i<action_length; ++i)
					sum += ::exp(alpha * ActionScore[i]);
		
				if(0 != sum)
				{
					double limit = rand_gen->Random() * sum;
					action = action_length - 1;
					sum = 0;

					for(int i=0; i<action_length; ++i)
					{
						sum += ::exp(alpha * ActionScore[i]);
						if(sum >= limit)
						{
							action = i;
							break;
						}
					}
				}
			}
			else	// greedy
			{
				for(int i=0; i<action_length; ++i)
					if(ActionScore[i] > ActionScore[action])
						action = i;
			}
		}
	}

	OutSelectedAction[action] = 1;	// set the next action
	last_action = action;
	copy_array(LastPunish, InPunish, action_length);

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