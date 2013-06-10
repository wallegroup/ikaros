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
	m_LrW_STR  = GetFloatValue("str");
	m_LrW_WEA  = GetFloatValue("wea");
	m_LrU_INH  = GetFloatValue("inh");
	m_LrU_DIS  = GetFloatValue("dis");
	m_StartWt  = GetFloatValue("startweights");
	m_StartInh = GetFloatValue("startinhibition");

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

	m_limit = GetBoolValue("limit");
}



CSFA::~CSFA(void)
{
	destroy_array(m_TrainOutput);
	destroy_array(m_Error);
	destroy_matrix(m_Weights);
	destroy_matrix(m_OutputLimit);
	destroy_matrix(m_Inhibition);
}



void CSFA::SetSizes(void)
{
	m_InputLength   = GetInputSize("INPUT");
	m_ContextLength = GetInputSize("CONTEXT");
	m_OutputLength  = GetInputSize("T-OUTPUT");

	SetOutputSize("OUTPUT", m_OutputLength);

#if ARROWS
	if(InputConnected("IMAGE"))
	{
		m_ImgSizeX = GetInputSizeX("IMAGE");
		m_ImgSizeY = GetInputSizeY("IMAGE");
	}
	else
	{
		m_ImgSizeX = m_ImgSizeY = 9;
	}

	SetOutputSize("XVALUE", m_ImgSizeX, m_ImgSizeY);
	SetOutputSize("YVALUE", m_ImgSizeX, m_ImgSizeY);
#endif
}



void CSFA::Init(void)
{
	m_InInput		 = GetInputArray("INPUT");
	m_InContext		 = GetInputArray("CONTEXT");
	m_InTrainInput	 = GetInputArray("T-INPUT");
	m_InTrainContext = GetInputArray("T-CONTEXT");
	m_InTeach		 = GetInputArray("T-OUTPUT");
	
	if(InputConnected("LEARN"))
		m_InLearn = GetInputArray("LEARN");
	else
		m_InLearn = NULL;
	
	m_OutOutput = GetOutputArray("OUTPUT");
	
	m_TrainOutput = create_array(m_OutputLength);
	m_Error = create_array(m_OutputLength);											// Store output error
	m_Weights = create_matrix(m_OutputLength, m_InputLength);						// Store stimuli weights
	m_OutputLimit = create_matrix(m_OutputLength, m_ContextLength);					// Store maximum allowed output
	m_Inhibition = create_matrix(m_ContextLength, m_OutputLength, m_InputLength);	// Store context weights

	//m_Test = create_matrix(m_OutputLength, m_ContextLength);	// Context to output weights (degrades performance)
	m_OldTrainInput = create_array(m_InputLength);

	if(m_StartWt > 0 || m_StartInh > 0)
	{
		for(int i = 0; i < m_InputLength; i++)
		{
			for(int j = 0; j < m_OutputLength; j++)
			{
				m_Weights[i][j] = m_StartWt;
				for(int k = 0; k < m_ContextLength; k++)
					m_Inhibition[i][j][k] = m_StartInh;
			}
		}
	}

	m_OldContext = -1;

	// DEBUG
#if ARROWS
	m_OutValueX_m = GetOutputMatrix("XVALUE");
	m_OutValueY_m = GetOutputMatrix("YVALUE");
	if(InputConnected("IMAGE"))
		m_InImage_m = GetInputMatrix("IMAGE");
	else
		m_InImage_m = NULL;
#endif
}



void CSFA::Tick(void)
{
	int cx = -1;
	for(int k = 0; k < m_ContextLength; k++)
	{
		if(m_InTrainContext[k] > 0)
		{
			cx = k;
			break;
		}
	}

	if(m_OldContext > -1 && cx != m_OldContext) PropagateContext(m_OldTrainInput, m_InTrainInput, m_OldContext, cx);
	m_OldContext = cx;
	copy_array(m_OldTrainInput, m_InTrainInput, m_InputLength);

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
		float * tmpInput = create_array(m_InputLength);
		float * tmpContext = create_array(m_ContextLength);
		
		int dir = 0;
		int pos = 0;
		int best = 0;
		
		for(int y=1; y<m_ImgSizeY-1; ++y)
		{
			for(int x=1; x<m_ImgSizeX-1; ++x)
			{
				if(m_InImage_m[y][x] < 1.0f)
				{
					m_OutValueX_m[y][x] = m_OutValueY_m[y][x] = 0;
					SetSurrounding(x, y, tmpInput);
				
					pos = y * m_ImgSizeX + x;
					tmpContext[pos] = 1;
					Ask(tmpInput, tmpContext, m_TrainOutput);
					tmpContext[pos] = 0;

					best = arg_max(m_TrainOutput, m_OutputLength);
					if(m_TrainOutput[best] > 0 && (m_TrainOutput[best] != m_TrainOutput[arg_min(m_TrainOutput, m_OutputLength)]))
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
						else	// Error
						{
							m_OutValueX_m[y][x] = -1;
							m_OutValueY_m[y][x] = -1;
						}
					}
				}
			}
		}

		destroy_array(tmpInput);
		destroy_array(tmpContext);
	}
#endif
}



inline void CSFA::Ask(float *Input, float *Context, float *Output)
{
	reset_array(Output, m_OutputLength);
	/*
	// Context to output weights (degrades performance)
	for(int k = 0; k < m_ContextLength; k++)
		if(Context[k] != 0)
			for(int j = 0; j < m_OutputLength; j++)
				Output[j] += Context[k] * m_Test[k][j];
	*/
	// Calculate output
	for(int i = 0; i < m_InputLength; i++)
	{
		if(Input[i] != 0)
		{
			for(int j = 0; j < m_OutputLength; j++)
			{
				float Iij = 1.0f;
				for(int k = 0; k < m_ContextLength; k++)
					Iij *= 1.0f - Context[k] * m_Inhibition[i][j][k];
				
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
					Output[j] = min(1.0f - m_OutputLimit[k][j] * Context[k], Output[j]);
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
			m_Error[j] = Target[j] - Output[j];

		// Adjust output limit
		if(m_limit)
		{
			for(k = 0; k < m_ContextLength; k++)
			{
				if(Context[k] > 0)
				{
					for(j = 0; j < m_OutputLength; j++)
						m_OutputLimit[k][j] = min(0.5f * (1.0f-Target[j]) + 0.5f * m_OutputLimit[k][j], 1.0f);
				}
			}
		}
		/*
		// Adjust context to output weights (degrades performance)
		for(j = 0; j < m_OutputLength; j++)
		{
			if(m_Error[j] != 0)
			{
				for(k = 0; k < m_ContextLength; k++)
					if(Context[k] != 0)
						m_Test[k][j] += Context[k] * m_Error[j] * m_LrW_STR * Scale;
			}
		}*/

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
									m_Inhibition[i][j][k] = max((m_LrU_DIS * Scale * Context[k] * m_Error[j] * 
										(1.0f-m_Inhibition[i][j][k])) / (m_Weights[i][j] * InputSum), 0.0f);
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
							m_Weights[i][j] = max(m_Weights[i][j] + Input[i] / InputSum * m_Error[j] * m_LrW_WEA * Scale, 0.0f);
					}
				}
			}
		}
	}
}



inline void	CSFA::PropagateContext(float * InputOld, float * InputNew, int OldCx, int Cx)
{
	for(int i = 0; i < m_InputLength; i++)
	{
		if(InputOld[i] > 0 && InputNew[i] > 0)
		{
			for(int j = 0; j < m_OutputLength; j++)
			{
				m_Inhibition[i][j][Cx] = m_Inhibition[i][j][Cx] * 0.8 + m_Inhibition[i][j][OldCx] * 0.2;
			}
		}
	}
}


#if ARROWS
void CSFA::SetSurrounding(int xpos, int ypos, float * input)
{
	if(m_InputLength == 256)
	{
		int obst[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		int j = 0;
		int k = 0;
		for(int dy=0; dy<3; ++dy)
		{
			for(int dx=0; dx<3; ++dx)
			{
				if(m_InImage_m[ypos+dy-1][xpos+dx-1] > 0.99f && m_InImage_m[ypos+dy-1][xpos+dx-1] < 1.01f)
					obst[k] = 1;

				k++;
			}
		}

		k = 0;
		for(int i=0; i<9; i++)
		{
			if(i != 4)
			{
				if(obst[i] > 0)
					j += pow(2, k);

				++k;
			}
		}
	
		input[j] = 1.0f;
	}
	else if(m_InputLength == 18)
	{
		int j = 0;
		int k = 0;
		for(int dy=0; dy<3; ++dy)
		{
			for(int dx=0; dx<3; ++dx)
			{
				if(m_InImage_m[ypos+dy-1][xpos+dx-1] > 0.99f && m_InImage_m[ypos+dy-1][xpos+dx-1] < 1.01f)
					input[k] = 1;
				else
					input[k+9] = 1;

				k++;
			}
		}
	}
}
#endif



static InitClass init("CSFA", &CSFA::Create, "Source/UserModules/CxQ/CSFA/");