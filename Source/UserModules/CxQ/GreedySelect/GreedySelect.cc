// add header

#include "GreedySelect.h"


GreedySelect::GreedySelect(Parameter *pParam):Module(pParam)
{
	AddInput("ACTION");		// Action output from the core module
	AddInput("REWARD");
	AddOutput("ACTION");	// Selected action to the core module
	
	m_pInAction_v = NULL;
	m_pInReward_v = NULL;
	m_pOutAction_v = NULL;
	
	m_flEpsilon = GetFloatValue("epsilon");
	m_flDelta = GetFloatValue("delta");
}


void GreedySelect::SetSizes(void)
{
	m_iActionLength = GetInputSize("ACTION");

	if(m_iActionLength != unknown_size)
		SetOutputSize("ACTION", m_iActionLength);
}


void GreedySelect::Init(void)
{
	m_pInAction_v  = GetInputArray("ACTION");
	m_pOutAction_v = GetOutputArray("ACTION");

	if(InputConnected("REWARD"))
		m_pInReward_v = GetInputArray("REWARD");

	m_pRand = new TRanrotWGenerator((uint32)rand());	// random double
}


void GreedySelect::Tick(void)
{
	int i = arg_max(m_pInAction_v, m_iActionLength);
	int j = arg_min(m_pInAction_v, m_iActionLength);
	reset_array(m_pOutAction_v, m_iActionLength);
	
	if(i == j)
	{
		int iSlct = j = m_pRand->IRandom(0, m_iActionLength - 1);
		m_pOutAction_v[iSlct] = 0.33f;
	}
	else // Input is valid
	{
		if(m_pRand->Random() < m_flEpsilon)
		{
			do
			{
				j = m_pRand->IRandom(0, m_iActionLength - 1);
			}
			while(i == j);
			
			m_pOutAction_v[j] = 0.67f;
			
			if(m_flDelta > 0 && m_pInReward_v == NULL)
			{
				m_flEpsilon *= m_flDelta;
			}
		}
		else
		{
			int n = 0;
			for(j = 0; j < m_iActionLength; j++)
			{
				if(m_pInAction_v[j] == m_pInAction_v[i])
				{
					n++;
				}
			}
			
			if(n > 1)
			{
				do
				{
					j = m_pRand->IRandom(0, m_iActionLength - 1);
				}
				while(m_pInAction_v[j] < m_pInAction_v[i]);
				
				m_pOutAction_v[j] = 0.667f;
			}
			else
			{
				m_pOutAction_v[i] = 1.0f;
			}
		}
	}
	
	if(m_pInReward_v != NULL && m_flDelta > 0 && m_pInReward_v[0] > 0)
		m_flEpsilon *= m_flDelta;
}


static InitClass init("GreedySelect", &GreedySelect::Create, "Source/UserModules/CxQ/GreedySelect/");