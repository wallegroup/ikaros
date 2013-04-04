//
//	CriticCore.h		This file is a part of the IKAROS project
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

#include "CriticCore.h"

Module* CriticCore::Create(Parameter *pParam)
{
	return new CriticCore(pParam);
}

CriticCore::CriticCore(Parameter *pParam):Module(pParam)
{
	AddInput("ACTOR");				// Action output from the learning module
	AddInput("CRITIC");
	AddInput("LAST_ACTOR");
	AddInput("LAST_CRITIC");
	AddInput("REINFORCEMENT");		// Reinforcement from the world module
	AddInput("SELECTED_ACTION");	// The action corresponding to the incomming reinforcement
	
	AddOutput("ACTOR_TARGET");		// Target output to the learning modules
	AddOutput("CRITIC_TARGET");
	
	m_pInActor_v		  = NULL;
	m_pLastActorAction_v  = NULL;
	m_pLastCriticAction_v = NULL;
	m_pInReinforcement_v  = NULL;
	m_pInSelectedAction_v = NULL;
	m_pInTrainAction_v	  = NULL;
	
	m_pOutActorTarget_v = NULL;
	m_pOutCriticTarget_v = NULL;
	
	m_iActionLength = GetIntValue("actions", unknown_size);
	m_flDiscount = GetFloatValue("discount", unknown_size);
	m_iInfinite = GetIntValue("infinite_horizon", unknown_size);
	m_iEpochs = GetIntValue("epochs", 0);
}

CriticCore::~CriticCore(void)
{
	
}

void CriticCore::SetSizes(void)
{
	if(m_iActionLength != unknown_size &&
		m_flDiscount != unknown_size &&
		m_iInfinite != unknown_size)
	{
		SetOutputSize("ACTOR_TARGET", m_iActionLength);
		SetOutputSize("CRITIC_TARGET", m_iActionLength);
	}
	else
	{
		Notify(msg_fatal_error, "Module \"%s\": One or more invalid parameter.\n", GetName());
        return;
    }
}

void CriticCore::Init(void)
{
	m_pInActor_v		  = GetInputArray("ACTOR");
	m_pInCritic_v		  = GetInputArray("CRITIC");
	m_pLastActorAction_v  = GetInputArray("LAST_ACTOR");
	m_pLastCriticAction_v = GetInputArray("LAST_CRITIC");
	m_pInReinforcement_v  = GetInputArray("REINFORCEMENT");
	m_pInSelectedAction_v = GetInputArray("SELECTED_ACTION");
	
	m_pOutActorTarget_v = GetOutputArray("ACTOR_TARGET");
	m_pOutCriticTarget_v = GetOutputArray("CRITIC_TARGET");
	
	m_iTick = 0;
	m_iPrint = 0;
	m_iLastPrintTick = 0;
	m_iAverage = 0;
}

void CriticCore::Tick(void)
{
	int iSelected = arg_max(m_pInSelectedAction_v, m_iActionLength);	// iSelected is the action selected by the selection module based on the actor's last output

	copy_array(m_pOutActorTarget_v, m_pLastCriticAction_v, m_iActionLength);
	copy_array(m_pOutCriticTarget_v, m_pLastCriticAction_v, m_iActionLength);
	
	if(m_iInfinite)
	{
		m_pOutCriticTarget_v[iSelected] = m_pInReinforcement_v[0] + m_flDiscount * max(m_pInCritic_v, m_iActionLength);
		
		if(m_pInReinforcement_v[0] != 0)	// Reward the action that lead to the rewarding state
		{
			m_pOutActorTarget_v[iSelected] = m_pInReinforcement_v[0];
		}
		else
		{
			m_pOutActorTarget_v[iSelected] = m_flDiscount * max(m_pInCritic_v, m_iActionLength);	// The reward for the last tick is based on the ANN output this tick
		}
	}
	else
	{
		if(m_pInReinforcement_v[0] != 0)	// Reward the action that lead to the rewarding state
		{
			m_pOutCriticTarget_v[iSelected] = m_pOutActorTarget_v[iSelected] = m_pInReinforcement_v[0];
		}
		else
		{
			m_pOutCriticTarget_v[iSelected] = m_pOutActorTarget_v[iSelected] = m_flDiscount * max(m_pInCritic_v, m_iActionLength);	// The reward for the last tick is based on the ANN output this tick
		}
	}
	
	if(m_iEpochs > 0)
	{
		if(m_pInReinforcement_v[0] > 0)	// Arrived at goal state
		{
			m_iPrint++;
			printf("%i\n", m_iTick - m_iLastPrintTick);
			m_iLastPrintTick = m_iTick;
		}
		
		if(m_iPrint >= m_iEpochs)
		{
			Notify(msg_terminate, "End\n");
		}
		
		m_iTick++;
	}
	else if(m_iEpochs < 0)
	{
		if(m_pInReinforcement_v[0] < 0)	// Failure
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
		
		m_iTick++;
	}
}

static InitClass init("CriticCore", &CriticCore::Create, "Source/UserModules/CxQ/CriticCore/");