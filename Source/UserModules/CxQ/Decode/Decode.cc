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
	
	if(m_iInputSize > 0)
	{
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
	int i, j = 0;
	
	for(i = 0; i < m_iInputSize; i++)
	{
		if(m_pInInput_v[i] > 0)
		{
			j += pow(2, i);
		}
	}
	
	m_pOutOutput_v[m_iLastValue] = 0.0f;
	m_pOutOutput_v[j] = 1.0f;
	m_iLastValue = j;
}

static InitClass init("Decode", &Decode::Create, "Source/UserModules/CxQ/Decode/");