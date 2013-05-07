//
//	RLCore.h		This file is a part of the IKAROS project
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

#include "RLCore.h"


RLCore::RLCore(Parameter *pParam):Module(pParam)
{
	AddInput("ACTION");			// Action output from the learning module
	AddInput("LAST_ACTION");
	AddInput("REINFORCEMENT");	// Reinforcement from the world module
	AddInput("SELECTED_ACTION");	// The action corresponding to the incomming reinforcement
	
	AddOutput("ACTION");			// Action to the selection module
	AddOutput("TARGET");			// Target output to the learning module
	
	m_pInAction_v		  = NULL;
	m_pLastInAction_v	  = NULL;
	m_pInReinforcement_v  = NULL;
	m_pInSelectedAction_v = NULL;
	m_pInTrainAction_v	  = NULL;
	
	m_pOutAction_v = NULL;
	m_pOutTarget_v = NULL;
	
	m_iActionLength = GetIntValue("actions");
	m_flDiscount = GetFloatValue("discount");
	m_iInfinite = GetIntValue("infinite_horizon");
	m_iEpochs = GetIntValue("epochs");
}


void RLCore::SetSizes(void)
{
	if(m_iActionLength == unknown_size)
		Notify(msg_fatal_error, "Module \"%s\": The 'actions' parameter is invalid.\n", GetName());
	
	if(m_flDiscount == unknown_size)
		Notify(msg_fatal_error, "Module \"%s\": The 'discount' parameter is invalid.\n", GetName());
	
	if(m_iInfinite == unknown_size)
		Notify(msg_fatal_error, "Module \"%s\": The 'infinite_horizon' parameter is invalid.\n", GetName());
	
	SetOutputSize("ACTION", m_iActionLength);
	SetOutputSize("TARGET", m_iActionLength);
}


void RLCore::Init(void)
{
	m_pInAction_v		  = GetInputArray("ACTION");
	m_pLastInAction_v	  = GetInputArray("LAST_ACTION");
	m_pInReinforcement_v  = GetInputArray("REINFORCEMENT");
	m_pInSelectedAction_v = GetInputArray("SELECTED_ACTION");
	
	m_pOutAction_v = GetOutputArray("ACTION");
	m_pOutTarget_v = GetOutputArray("TARGET");
	
	m_iTick = 0;
	m_iPrint = 0;
	m_iLastPrintTick = 0;
}


void RLCore::Tick(void)
{	
	// iSelected is the action selected by the selection module based on the ANN output last tick
	int iSelected = arg_max(m_pInSelectedAction_v, m_iActionLength);
	copy_array(m_pOutTarget_v, m_pLastInAction_v, m_iActionLength);

	// Forward the action selected by the learning module
	copy_array(m_pOutAction_v, m_pInAction_v, m_iActionLength);
	
	if(m_iInfinite == 1)
	{
		m_pOutTarget_v[iSelected] = *m_pInReinforcement_v + m_flDiscount * max(m_pInAction_v, m_iActionLength);
	}
	else if(m_iInfinite == 2)
	{		
		if(*m_pInReinforcement_v > 0)	// Reward the action that lead to the rewarding state
			m_pOutTarget_v[iSelected] = *m_pInReinforcement_v;
		else	// The reward for the last tick is based on the ANN output this tick
		{
			float tmp = max(m_pInAction_v, m_iActionLength);
			if(tmp > 0)
				m_pOutTarget_v[iSelected] = *m_pInReinforcement_v + m_flDiscount * tmp;
			else
				m_pOutTarget_v[iSelected] = *m_pInReinforcement_v + (2.0f-m_flDiscount) * tmp;
		}
	}
	else
	{		
		if(*m_pInReinforcement_v > 0)	// Reward the action that lead to the rewarding state
			m_pOutTarget_v[iSelected] = *m_pInReinforcement_v;
		else	// The reward for the last tick is based on the ANN output this tick
			m_pOutTarget_v[iSelected] = *m_pInReinforcement_v + m_flDiscount * max(m_pInAction_v, m_iActionLength);
	}
	
	if(m_iEpochs > 0)
	{
		if(*m_pInReinforcement_v > 0)	// Arrived at goal state
		{
			printf("%i\n", m_iTick - m_iLastPrintTick);
			m_iLastPrintTick = m_iTick;
			m_iPrint++;
		}
		
		if(m_iPrint >= m_iEpochs)
			Notify(msg_terminate, "End\n");
		
		m_iTick++;
	}
}


static InitClass init("RLCore", &RLCore::Create, "Source/UserModules/CxQ/RLCore/");