// add header

#include "Merge.h"

Module* Merge::Create(Parameter *pParam)
{
	return new Merge(pParam);
}

Merge::Merge(Parameter *pParam):Module(pParam)
{
	AddInput("INPUT1");
	AddInput("INPUT2");
	AddInput("INPUT3");
	AddInput("INPUT4");
	AddInput("INPUT5");
	
	AddOutput("OUTPUT");
	
	m_pInput1_v = NULL;
	m_pInput2_v = NULL;
	m_pInput3_v = NULL;
	m_pInput4_v = NULL;
	m_pInput5_v = NULL;
	
	m_iInputSize1 = 0;
	m_iInputSize2 = 0;
	m_iInputSize3 = 0;
	m_iInputSize4 = 0;
	m_iInputSize5 = 0;
}

Merge::~Merge(void)
{
	
}

void Merge::SetSizes(void)
{
	int iSum = 0;
	
	if(GetInputSize("INPUT1") > 0)
	{
		m_iInputSize1 = GetInputSize("INPUT1");
		iSum += m_iInputSize1;
	}
	
	if(GetInputSize("INPUT2") > 0)
	{
		m_iInputSize2 = GetInputSize("INPUT2");
		iSum += m_iInputSize2;
	}

	if(GetInputSize("INPUT3") > 0)
	{
		m_iInputSize3 = GetInputSize("INPUT3");
		iSum += m_iInputSize3;
	}

	if(GetInputSize("INPUT4") > 0)
	{
		m_iInputSize4 = GetInputSize("INPUT4");
		iSum += m_iInputSize4;
	}

	if(GetInputSize("INPUT5") > 0)
	{
		m_iInputSize5 = GetInputSize("INPUT5");
		iSum += m_iInputSize5;
	}
	
	if(iSum > 0)
	{
		SetOutputSize("OUTPUT", iSum);
	}
}

void Merge::Init(void)
{
	if(m_iInputSize1 > 0)
	{
		m_pInput1_v = GetInputArray("INPUT1");
	}
	
	if(m_iInputSize2 > 0)
	{
		m_pInput2_v = GetInputArray("INPUT2");
	}

	if(m_iInputSize3 > 0)
	{
		m_pInput3_v = GetInputArray("INPUT3");
	}

	if(m_iInputSize4 > 0)
	{
		m_pInput4_v = GetInputArray("INPUT4");
	}

	if(m_iInputSize5 > 0)
	{
		m_pInput5_v = GetInputArray("INPUT5");
	}
	
	m_pOutput_v = GetOutputArray("OUTPUT");
}

void Merge::Tick(void)
{
	int i, k = 0;
	
	for(i = 0; i < m_iInputSize1; i++) 
	{
		m_pOutput_v[k] = m_pInput1_v[i]; 
		k++;
	}
	
	for(i = 0; i < m_iInputSize2; i++) 
	{
		m_pOutput_v[k] = m_pInput2_v[i];
		k++;
	}

	for(i = 0; i < m_iInputSize3; i++) 
	{
		m_pOutput_v[k] = m_pInput3_v[i];
		k++;
	}

	for(i = 0; i < m_iInputSize4; i++) 
	{
		m_pOutput_v[k] = m_pInput4_v[i];
		k++;
	}

	for(i = 0; i < m_iInputSize5; i++) 
	{
		m_pOutput_v[k] = m_pInput5_v[i];
		k++;
	}
}

static InitClass init("Merge", &Merge::Create, "Source/UserModules/CxQ/Merge/");