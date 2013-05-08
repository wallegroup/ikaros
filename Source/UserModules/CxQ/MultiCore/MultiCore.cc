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



void MultiCore::SetSizes(void)
{
	if(m_iActionLength != unknown_size &&
		m_flDiscount != unknown_size &&
		m_iInfinite != unknown_size)
	{
		m_iActionLength = GetIntValue("actions", unknown_size);
		SetOutputSize("ACTOR_TARGET", m_iActionLength);
		SetOutputSize("CRITIC_TARGET", m_iActionLength);
		SetOutputSize("LEARN", 1);
		SetOutputSize("ACTOR_GRAD", 1);
		SetOutputSize("CRITIC_GRAD", 1);
	}
	else
	{
		Notify(msg_fatal_error, "Module \"%s\": One or more invalid parameter.\n", GetName());
        return;
    }
}



void MultiCore::Init(void)
{
	m_pInActor_v			= GetInputArray("ACTOR");
	m_pInCritic_v			= GetInputArray("CRITIC");
	m_pInLastActorAction_v  = GetInputArray("LAST_ACTOR");
	m_pInLastCriticAction_v = GetInputArray("LAST_CRITIC");
	m_pInReinforcement_v	= GetInputArray("REINFORCEMENT");
	m_pInSelectedAction_v	= GetInputArray("SELECTED_ACTION");
	
	m_pOutActorTarget_v  = GetOutputArray("ACTOR_TARGET");
	m_pOutCriticTarget_v = GetOutputArray("CRITIC_TARGET");
	m_pOutLearn_v		 = GetOutputArray("LEARN");
	m_pOutActorGrad_v	 = GetOutputArray("ACTOR_GRAD");
	m_pOutCriticGrad_v	 = GetOutputArray("CRITIC_GRAD");
	
	m_flDiscount = GetFloatValue("discount");
	m_flScale = GetFloatValue("scale");
	m_iInfinite = GetIntValue("infinite_horizon");
	m_iEpochs = GetIntValue("epochs");
	
	m_iTick = 0;
	m_iPrint = 0;
	m_iAverage = 0;
	m_iLastPrintTick = 0;
	m_pOutLearn_v[0] = 1;
}



void MultiCore::Tick(void)
{
	// iSelected is the action selected by the selection module based on the actor's last output
	int iSelected = arg_max(m_pInSelectedAction_v, m_iActionLength);

	copy_array(m_pOutActorTarget_v, m_pInLastCriticAction_v, m_iActionLength);
	copy_array(m_pOutCriticTarget_v, m_pInLastCriticAction_v, m_iActionLength);
	
	if(m_iInfinite == 1)
	{
		m_pOutCriticTarget_v[iSelected] = *m_pInReinforcement_v + m_flDiscount * max(m_pInCritic_v, m_iActionLength);
		m_pOutActorTarget_v[iSelected] = m_pOutCriticTarget_v[iSelected];
	}
	else if(m_iInfinite == 2)
	{
		for(int i=0; i<m_iActionLength; ++i)
			m_pOutActorTarget_v[i] = m_pInLastActorAction_v[i];

		if(*m_pInReinforcement_v > 0)	// Reward the action that lead to the rewarding state
		{
			m_pOutActorTarget_v[iSelected] = m_pOutCriticTarget_v[iSelected] = *m_pInReinforcement_v;
		}
		else	// The reward for the last tick is based on the ANN output this tick
		{
			m_pOutCriticTarget_v[iSelected] = *m_pInReinforcement_v + m_flDiscount * max(m_pInCritic_v, m_iActionLength);
			m_pOutActorTarget_v[iSelected] = max(log((0.01f+m_pOutCriticTarget_v[iSelected]) / (0.01f+m_pInLastCriticAction_v[iSelected])), 0.0f);
		}
	}
	else
	{
		if(*m_pInReinforcement_v > 0)	// Reward the action that lead to the rewarding state
			m_pOutCriticTarget_v[iSelected] = *m_pInReinforcement_v;
		else	// The reward for the last tick is based on the ANN output this tick
			m_pOutCriticTarget_v[iSelected] = *m_pInReinforcement_v + m_flDiscount * max(m_pInCritic_v, m_iActionLength);

		m_pOutActorTarget_v[iSelected] = m_pOutCriticTarget_v[iSelected];
	}

	m_pOutActorGrad_v[0] = max(m_pInActor_v, m_iActionLength);
	m_pOutCriticGrad_v[0] = max(m_pInCritic_v, m_iActionLength);

	if(*m_pInReinforcement_v > 0 && m_flScale > 0)
		m_pOutLearn_v[0] = max(0.1f, m_pOutLearn_v[0] * m_flScale);
	
	if(m_iEpochs > 0)
	{
		if(*m_pInReinforcement_v > 0)	// Arrived at goal state
		{
			m_iPrint++;
			printf("%i\n", m_iTick - m_iLastPrintTick);
			m_iLastPrintTick = m_iTick;

			if(m_iPrint >= m_iEpochs)
				Notify(msg_terminate, "End\n");
		}
	}
	else if(m_iEpochs < 0)
	{
		if(*m_pInReinforcement_v < 0)	// Failure
		{
			if(m_iLastPrintTick > 1000)
				printf("%i:\t%i\n", m_iPrint, m_iLastPrintTick);
			
			m_iAverage += m_iLastPrintTick;
			m_iLastPrintTick = 0;
			m_iPrint++;
		}
		else
		{
			m_iLastPrintTick++;
		}
		
		if(-m_iPrint == m_iEpochs)
		{
			printf("Average ticks to failure: %f\n", m_iAverage/(float)m_iPrint);
			Notify(msg_terminate, "Average ticks to failure: %f\n", m_iAverage/(float)m_iPrint);
		}
	}

	m_iTick++;
}



static InitClass init("MultiCore", &MultiCore::Create, "Source/UserModules/CxQ/MultiCore/");