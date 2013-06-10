//
//	LinearNet.h		This file is a part of the IKAROS project
//
//		Linear function approximator.
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

#include "LinearNet.h"

#define ARROWS 1


LinearNet::LinearNet(Parameter *pParam):Module(pParam)
{
	m_flLr = GetFloatValue("lr");
	if(m_flLr < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'lr' parameter is not set.\n", GetName());

	m_flMinWt = GetFloatValue("min_weight");
	m_flMaxWt = GetFloatValue("max_weight");
	m_flMaxSum = GetFloatValue("max_sum");
	m_flStartWt = GetFloatValue("startweights");
	m_flRandomWt = GetFloatValue("random");
	m_bLimit = GetBoolValue("limit");
}


LinearNet::~LinearNet(void)
{
	destroy_array(m_pTrainOutput_v);
	destroy_matrix(m_pWij_m);
}


void LinearNet::SetSizes(void)
{
	m_iInputLength  = GetInputSize("INPUT");
	m_iOutputLength = GetInputSize("T-OUTPUT");
	SetOutputSize("OUTPUT", m_iOutputLength);

#if ARROWS
	if(InputConnected("IMAGE"))
	{
		m_iImgSizeX = GetInputSizeX("IMAGE");
		m_iImgSizeY = GetInputSizeY("IMAGE");
	}
	else
	{
		m_iImgSizeX = m_iImgSizeY = 9;
	}

	SetOutputSize("XVALUE", m_iImgSizeX, m_iImgSizeY);
	SetOutputSize("YVALUE", m_iImgSizeX, m_iImgSizeY);
#endif
}


void LinearNet::Init(void)
{
	m_pInInput_v = GetInputArray("INPUT");
	m_pInTrainInput_v = GetInputArray("T-INPUT");
	m_pInTeach_v = GetInputArray("T-OUTPUT");
	
	if(InputConnected("LEARN"))
		m_pInLearn_v = GetInputArray("LEARN");
	else
		m_pInLearn_v = NULL;

	m_pOutOutput_v = GetOutputArray("OUTPUT");
	
	m_pTrainOutput_v = create_array(m_iOutputLength);
	m_pWij_m = create_matrix(m_iOutputLength, m_iInputLength);	// Store stimuli weights
	
	rand_gen = new TRanrotWGenerator((uint32)rand());	// random double

	if(m_flStartWt != 0)
	{
		for(int i = 0; i < m_iInputLength; i++)
		{
			for(int j = 0; j < m_iOutputLength; j++)
			{
				m_pWij_m[i][j] = m_flStartWt;
				if(m_flRandomWt > 0)
					m_pWij_m[i][j] += rand_gen->Random() * m_flRandomWt - m_flRandomWt * 0.5f;
			}
		}
	}

#if ARROWS
	if(InputConnected("IMAGE"))
		m_pInImage_m = GetInputMatrix("IMAGE");
	else
		m_pInImage_m = NULL;

	m_pOutValueX_m = GetOutputMatrix("XVALUE");
	m_pOutValueY_m = GetOutputMatrix("YVALUE");
#endif
}


void LinearNet::Tick(void)
{
	// Train	
	if(m_pInLearn_v == NULL || m_pInLearn_v[0] > 0)
	{
		float flError, 
			  flSum = 1.0f,
			  flScale = m_flLr;
		
		if(m_flMaxSum > 0)
			flSum = add(m_pInTrainInput_v, m_iInputLength) / m_flMaxSum;
		
		if(m_pInLearn_v != NULL)
			flScale *= m_pInLearn_v[0];

		for(int j = 0; j < m_iOutputLength; j++)
		{
			// Calculate old output
			m_pTrainOutput_v[j] = 0.0f;
			for(int i = 0; i < m_iInputLength; i++)
				m_pTrainOutput_v[j] += m_pInTrainInput_v[i] * m_pWij_m[i][j];

			flError = m_pInTeach_v[j] - m_pTrainOutput_v[j];	// Calculate error
			if(flError != 0)	// Adjust weights
			{
				if(m_bLimit)
				{
					for(int i = 0; i < m_iInputLength; i++)
						if(m_pInTrainInput_v[i] != 0)
							m_pWij_m[i][j] = max(min(m_pWij_m[i][j] + m_pInTrainInput_v[i] / flSum * flError * flScale, m_flMaxWt), m_flMinWt);
				}
				else
				{
					for(int i = 0; i < m_iInputLength; i++)
						if(m_pInTrainInput_v[i] != 0)
							m_pWij_m[i][j] = m_pWij_m[i][j] + m_pInTrainInput_v[i] / flSum * flError * flScale;
				}
			}
		}
	}

	// Calculate output
	for(int j = 0; j < m_iOutputLength; j++)
	{
		m_pOutOutput_v[j] = 0.0f;
		for(int i = 0; i < m_iInputLength; i++)
			m_pOutOutput_v[j] += m_pInInput_v[i] * m_pWij_m[i][j];
	}

#if ARROWS
	if(m_pInImage_m != NULL)
	{
		reset_array(m_pInTrainInput_v, m_iInputLength);
	
		int dir = 0;
		int pos = 0;
		int best = 0;
		for(int y = 0; y < m_iImgSizeY; y++)
		{
			for(int x = 0; x < m_iImgSizeX; x++)
			{
				if(m_pInImage_m[y][x] != 1)
				{
					m_pInTrainInput_v[pos] = 1;
					for(int j = 0; j < m_iOutputLength; j++)
					{
						m_pTrainOutput_v[j] = 0.0f;
						for(int i = 0; i < m_iInputLength; i++)
						{
							m_pTrainOutput_v[j] += m_pInTrainInput_v[i] * m_pWij_m[i][j];
						}
					}
				
					m_pInTrainInput_v[pos] = 0;
					//best = arg_max(m_pTrainOutput_v, m_iOutputLength);
					best = 0;
					float value = m_pTrainOutput_v[best];
					for(int i=1; i<m_iOutputLength; ++i)
					{
						if(m_pTrainOutput_v[i] > value)
						{
							value = m_pTrainOutput_v[i];
							best = i;
						}
					}
				
					if(best == 0)
					{
						if(m_pTrainOutput_v[0] == m_pTrainOutput_v[1] && m_pTrainOutput_v[0] == m_pTrainOutput_v[2] && m_pTrainOutput_v[0] == m_pTrainOutput_v[3])
						{
							m_pOutValueX_m[y][x] = 0;
							m_pOutValueY_m[y][x] = 0;
						}
						else
						{
							m_pOutValueX_m[y][x] = 0;
							m_pOutValueY_m[y][x] = -1;
						}
					}
					else if(best == 1)
					{
						m_pOutValueX_m[y][x] = 1;
						m_pOutValueY_m[y][x] = 0;
					}
					else if(best == 2)
					{
						m_pOutValueX_m[y][x] = 0;
						m_pOutValueY_m[y][x] = 1;
					}
					else if(best == 3)
					{
						m_pOutValueX_m[y][x] = -1;
						m_pOutValueY_m[y][x] = 0;
					}
				}
			
				pos++;
			}
		}
	}
#endif
}


static InitClass init("LinearNet", &LinearNet::Create, "Source/UserModules/CxQ/LinearNet/");