// add header

#include "Decode.h"

Module* Decode::Create(Parameter *pParam)
{
	return new Decode(pParam);
}

Decode::Decode(Parameter *pParam):Module(pParam)
{
	AddInput("INPUT");
	AddOutput("OUTPUT");
}

Decode::~Decode(void)
{
	
}

void Decode::SetSizes(void)
{
	m_iInputSize = GetInputSize("INPUT");
	m_iSkipIndex = GetIntValue("skip_index");
	
	if(m_iInputSize > 0)
	{
		if(m_iSkipIndex > -1)
			SetOutputSize("OUTPUT", pow(2, m_iInputSize-1));
		else
			SetOutputSize("OUTPUT", pow(2, m_iInputSize));
	}
}

void Decode::Init(void)
{
	m_pInInput_v = GetInputArray("INPUT");
	m_pOutOutput_v = GetOutputArray("OUTPUT");
	
	m_iLastValue = 0;
}

void Decode::Tick(void)
{
	int j = 0,
		k = 0;

	for(int i = 0; i < m_iInputSize; ++i)
	{
		if(i != m_iSkipIndex)
		{
			if(m_pInInput_v[i] > 0)
			{
				j += pow(2, k);
			}

			++k;
		}
	}
	
	m_pOutOutput_v[m_iLastValue] = 0.0f;
	m_pOutOutput_v[j] = 1.0f;
	m_iLastValue = j;
}

static InitClass init("Decode", &Decode::Create, "Source/UserModules/CxQ/Decode/");