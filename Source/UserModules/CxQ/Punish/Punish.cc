//
//	Punish.cc		This file is a part of the IKAROS project
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
//	Created: 2007-07-08
//
//	A linear artificial neural network trained with a single error signal 
//	representing a punishment, instead of a target vector.

#include "Punish.h"

Module* Punish::Create(Parameter *pParam)
{
	return new Punish(pParam);
}

Punish::Punish(Parameter *pParam):Module(pParam)
{
	AddInput("OBSTACLES");
	AddInput("TRAIN_OBSTACLES");
	AddInput("SELECTED_ACTION");
	AddInput("PUNISHMENT");

	AddOutput("ACTION");
	
	m_pInObstacles_v = NULL;
	m_pInTrainObstacles_v = NULL;
	m_pInSelectedAction_v = NULL;
	m_pInPunishment_v = NULL;
	m_pOutAction_v = NULL;
	
	m_flLr = GetFloatValue("lr", 1.0f);
}

Punish::~Punish(void)
{
	destroy_array(m_pTeach_v);
	destroy_array(m_pTrainOutput_v);
	destroy_matrix(m_pWij_m);
}

void Punish::SetSizes(void)
{
	m_iObstacleLength = GetInputSize("OBSTACLES");
	m_iActionLength = GetInputSize("SELECTED_ACTION");
	SetOutputSize("ACTION", m_iActionLength);
}

void Punish::Init(void)
{
	m_pInObstacles_v = GetInputArray("OBSTACLES");
	m_pInTrainObstacles_v = GetInputArray("TRAIN_OBSTACLES");
	m_pInSelectedAction_v = GetInputArray("SELECTED_ACTION");
	m_pInPunishment_v = GetInputArray("PUNISHMENT");
	m_pOutAction_v = GetOutputArray("ACTION");
	
	m_pTeach_v = create_array(m_iActionLength);
	m_pTrainOutput_v = create_array(m_iActionLength);
	m_pWij_m = create_matrix(m_iActionLength, m_iObstacleLength);
}

void Punish::Tick(void)
{
	int i, j, iSlct;
	float flError, flSum;
	
	// Calculate output
	for(j = 0; j < m_iActionLength; j++)
	{
		m_pOutAction_v[j] = 0.0f;
		
		for(i = 0; i < m_iObstacleLength; i++)
		{
			m_pOutAction_v[j] += m_pInObstacles_v[i] * m_pWij_m[i][j];
		}
	}
	
	// Calculate old output
	for(j = 0; j < m_iActionLength; j++)
	{
		m_pTrainOutput_v[j] = 0.0f;
		
		for(i = 0; i < m_iObstacleLength; i++)
		{
			m_pTrainOutput_v[j] += m_pInTrainObstacles_v[i] * m_pWij_m[i][j];
		}
	}
	
	iSlct = arg_max(m_pInSelectedAction_v, m_iActionLength);
	copy_array(m_pTeach_v, m_pTrainOutput_v, m_iActionLength);
	m_pTeach_v[iSlct] = -m_pInPunishment_v[0];
	flSum = add(m_pInTrainObstacles_v, m_iObstacleLength);
	
	// Adjust weights
	for(j = 0; j < m_iActionLength; j++)
	{
		flError = m_pTeach_v[j] - m_pTrainOutput_v[j];	// Calculate error
		
		if(flError != 0)
		{
			for(i = 0; i < m_iObstacleLength; i++)
			{
				if(m_pInTrainObstacles_v[i] > 0)
				{
					m_pWij_m[i][j] = max(min(m_pWij_m[i][j] + m_pInTrainObstacles_v[i] / flSum * flError * m_flLr, 1.0f), -1.0f);
				}
			}
		}
	}
}

static InitClass init("Punish", &Punish::Create, "Source/UserModules/CxQ/Punish/");