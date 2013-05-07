//
//	CSFA.h		This file is a part of the IKAROS project
//
//		Context-sensitive function approximator. Basicly a linear 
//		network with context functionality.
//
//	Copyright (C) 2005 Stefan Winberg
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//	
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//	See http://www.ikaros-project.org/ for more information.
//

#include "CSFA.h"

CSFA::CSFA(Parameter *pParam):Module(pParam)
{
	AddInput("INPUT");
	AddInput("CONTEXT");
	AddInput("T-INPUT");
	AddInput("T-CONTEXT");
	AddInput("T-OUTPUT");
	AddInput("LEARN");
	
	AddOutput("OUTPUT");

#if ARROWS
	AddInput("IMAGE");
	AddOutput("XVALUE");
	AddOutput("YVALUE");

	m_OutValueX_m = NULL;
	m_OutValueY_m = NULL;
#endif
	
	m_InInput			= NULL;
	m_InContext			= NULL;
	m_InTrainInput		= NULL;
	m_InTrainContext	= NULL;
	m_InTeach			= NULL;
	m_InLearn			= NULL;
	m_OutOutput			= NULL;
	
	m_LrW_STR  = GetFloatValue("str", -1.0f);
	m_LrW_WEA  = GetFloatValue("wea", -1.0f);
	m_LrU_INH  = GetFloatValue("inh", -1.0f);
	m_LrU_DIS  = GetFloatValue("dis", -1.0f);
	m_StartWt  = GetFloatValue("startweights", -1.0f);
	m_StartInh = GetFloatValue("startinhibition", -1.0f);

	m_limit = GetBoolValue("limit");
}


CSFA::~CSFA(void)
{
	destroy_array(m_TrainOutput);
	destroy_array(m_Error);
	destroy_matrix(m_Weights);
	destroy_matrix(m_OutputLimit);
	destroy_cube(m_Inhibition);
}


void CSFA::Init(void)
{	
	m_InInput			= GetInputArray("INPUT");
	m_InContext			= GetInputArray("CONTEXT");
	m_InTrainInput		= GetInputArray("T-INPUT");
	m_InTrainContext	= GetInputArray("T-CONTEXT");
	m_InTeach			= GetInputArray("T-OUTPUT");
	
	if(InputConnected("LEARN"))
	{
		m_InLearn = GetInputArray("LEARN");
	}
	else
	{
		m_InLearn = NULL;
	}
	
	m_OutOutput = GetOutputArray("OUTPUT");

#if ARROWS
	m_InImage_m   = GetInputMatrix("IMAGE");
	m_OutValueX_m = GetOutputMatrix("XVALUE");
	m_OutValueY_m = GetOutputMatrix("YVALUE");
#endif

	m_InputLength   = GetInputSize("INPUT");
	m_ContextLength = GetInputSize("CONTEXT");
	m_OutputLength  = GetInputSize("T-OUTPUT");
	
	m_TrainOutput = create_array(m_OutputLength);
	m_Error = create_array(m_OutputLength);										// Store output error
	m_Weights = create_matrix(m_OutputLength, m_InputLength);					// Store stimuli weights
	m_OutputLimit = create_matrix(m_OutputLength, m_ContextLength);				// Store maximum allowed output
	m_Inhibition = create_cube(m_ContextLength, m_OutputLength, m_InputLength);	// Store context weights

	set_matrix(m_OutputLimit, 0.0f, m_OutputLength, m_ContextLength);	// Allow max output
	
	if(m_StartWt > 0 || m_StartInh > 0)
	{
		for(int i = 0; i < m_InputLength; i++)
		{
			for(int j = 0; j < m_OutputLength; j++)
			{
				m_Weights[i][j] = m_StartWt;
				
				for(int k = 0; k < m_ContextLength; k++)
				{
					m_Inhibition[i][j][k] = m_StartInh;
				}
			}
		}
	}

	if(m_LrW_STR < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'str' parameter is not set.\n", GetName());

	if(m_LrW_WEA < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'wea' parameter is not set.\n", GetName());

	if(m_LrU_INH < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'inh' parameter is not set.\n", GetName());

	if(m_LrU_DIS < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'dis' parameter is not set.\n", GetName());

	if(m_StartWt < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'startweights' parameter is not set.\n", GetName());

	if(m_StartInh < 0)
		Notify(msg_fatal_error, "Module \"%s\": The 'startinhibition' parameter is not set.\n", GetName());
}


void CSFA::Tick(void)
{
	if(m_InLearn == NULL)
	{
		Ask(m_InTrainInput, m_InTrainContext, m_TrainOutput);		// Recreate old output

		/*for(int j = 0; j < m_OutputLength; j++)
			m_InTeach[j] = 0.5f * m_InTeach[j] + 0.5f;*/

		Train(m_InTrainInput, m_InTrainContext, m_TrainOutput, m_InTeach);	// Train network
	}
	else if(m_InLearn[0] > 0)
	{
		Ask(m_InTrainInput, m_InTrainContext, m_TrainOutput);		// Recreate old output

		/*for(int j = 0; j < m_OutputLength; j++)
			m_InTeach[j] = 0.5f * m_InTeach[j] + 0.5f;*/

		Train(m_InTrainInput, m_InTrainContext, m_TrainOutput, m_InTeach, m_InLearn[0]);	// Train network
	}

	Ask(m_InInput, m_InContext, m_OutOutput);	// Create output

	/*for(int j = 0; j < m_OutputLength; j++)
		m_OutOutput[j] = (m_OutOutput[j] - 0.5f) * 2.0f;*/
	
#if ARROWS
	if(m_InImage_m != NULL)
	{
		reset_array(m_InTrainInput, m_InputLength);
		reset_array(m_InTrainContext, m_ContextLength);
		
		int dir = 0;
		int pos = 0;
		int best = 0;
		int xlen = GetInputSizeX("IMAGE");
		int ylen = GetInputSizeY("IMAGE");
		
		for(int y = 1; y < ylen-1; y++)
		{
			for(int x = 1; x < xlen-1; x++)
			{
				SetSurrounding(x, y);
				if(m_InTrainInput[4] < 1.01f && m_InTrainInput[4] > 0.99f)
				{
					pos = y*xlen + x;
					m_InTrainContext[pos] = 1;
					Ask(m_InTrainInput, m_InTrainContext, m_TrainOutput);
					m_InTrainContext[pos] = 0;
					best = arg_max(m_TrainOutput, m_OutputLength);
					if(m_TrainOutput[best] > 0)
					{
						if(best == 0)	// Up
						{
							m_OutValueX_m[y][x] = 0;
							m_OutValueY_m[y][x] = -1;
						}
						else if(best == 1)	// Right
						{
							m_OutValueX_m[y][x] = 1;
							m_OutValueY_m[y][x] = 0;
						}
						else if(best == 2)	// Down
						{
							m_OutValueX_m[y][x] = 0;
							m_OutValueY_m[y][x] = 1;
						}
						else if(best == 3)	// Left
						{
							m_OutValueX_m[y][x] = -1;
							m_OutValueY_m[y][x] = 0;
						}
					}
					else
					{
						m_OutValueX_m[y][x] = 0;
						m_OutValueY_m[y][x] = 0;
					}
				}
			}
		}
	}
#endif
}


inline void CSFA::Ask(float *Input, float *Context, float *Output)
{
	reset_array(Output, m_OutputLength);
	
	// Calculate output
	for(int i = 0; i < m_InputLength; i++)
	{
		if(Input[i] != 0)
		{
			for(int j = 0; j < m_OutputLength; j++)
			{
				float Iij = 1.0f;
				
				for(int k = 0; k < m_ContextLength; k++)
				{
					Iij *= 1.0f - Context[k] * m_Inhibition[i][j][k];
				}
				
				Output[j] += Input[i] * m_Weights[i][j] * Iij;
			}
		}
	}

	// Limit output by context
	if(m_limit)
	{
		for(int k = 0; k < m_ContextLength; k++)
		{
			if(Context[k] > 0)
			{
				for(int j = 0; j < m_OutputLength; j++)
				{
					Output[j] = min(1.0f - m_OutputLimit[k][j] * Context[k], Output[j]);
				}
			}
		}
	}
}


inline void CSFA::Train(float *Input, float *Context, float *Output, float *Target, float Scale)
{
	int i, j, k;
	float InputSum = add(Input, m_InputLength);
	
	if(0 != InputSum)
	{
		// Calculate errors
		for(j = 0; j < m_OutputLength; j++)
		{
			m_Error[j] = Target[j] - Output[j];
		}

		// Adjust output limit
		if(m_limit)
		{
			for(k = 0; k < m_ContextLength; k++)
			{
				if(Context[k] > 0)
				{
					for(j = 0; j < m_OutputLength; j++)
					{
						m_OutputLimit[k][j] = min(0.5f * (1.0f-Target[j]) + 0.5f * m_OutputLimit[k][j], 1.0f);
					}
				}
			}
		}

		// Adjust weights
		for(i = 0; i < m_InputLength; i++)
		{
			if(Input[i] > 0)
			{
				for(j = 0; j < m_OutputLength; j++)
				{
					if(m_Error[j] > 0)
					{
						if(m_Weights[i][j] != 0)
						{
							for(k = 0; k < m_ContextLength; k++)
							{
								if(Context[k] > 0)
								{
									m_Inhibition[i][j][k] = max((m_LrU_DIS * Scale * Context[k] * m_Error[j] * (1.0f-m_Inhibition[i][j][k])) / (m_Weights[i][j] * InputSum), 0.0f);
								}
							}
						}

						m_Weights[i][j] = min(m_Weights[i][j] + Input[i] / InputSum * m_Error[j] * m_LrW_STR * Scale, 1.0f);
					}
					else if(m_Error[j] < 0)
					{
						if(m_Weights[i][j] != 0)
						{
							for(k = 0; k < m_ContextLength; k++)
							{
								if(Context[k] > 0)
								{
									m_Inhibition[i][j][k] = min(m_Inhibition[i][j][k] - m_LrU_INH * Scale * ((Context[k] * m_Error[j] * 
										(1.0f-m_Inhibition[i][j][k])) / (m_Weights[i][j] * InputSum)), 1.0f);
								}
							}
						}

						if(m_LrW_WEA > 0)
						{
							m_Weights[i][j] = max(m_Weights[i][j] + Input[i] / InputSum * m_Error[j] * m_LrW_WEA * Scale, 0.0f);
						}
					}
				}
			}
		}
	}
}


#if ARROWS
void CSFA::SetSurrounding(int xpos, int ypos)
{
	int k = 0;
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			if(m_InImage_m[ypos+y-1][xpos+x-1] != 1)
			{
				m_InTrainInput[k] = 1.0f;
			}
			else
			{
				m_InTrainInput[k] = 0.0f;
			}

			k++;
		}
	}

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			if(m_InImage_m[ypos+y-1][xpos+x-1] != 1)
			{
				m_InTrainInput[k] = 0.0f;
			}
			else
			{
				m_InTrainInput[k] = 1.0f;
			}

			k++;
		}
	}
}
#endif


static InitClass init("CSFA", &CSFA::Create, "Source/UserModules/CxQ/CSFA/");