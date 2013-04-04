// add header

#include "BoltzmannSelect.h"

Module* BoltzmannSelect::Create(Parameter *pParam)
{
	return new BoltzmannSelect(pParam);
}

BoltzmannSelect::BoltzmannSelect(Parameter *pParam):Module(pParam)
{
	AddInput("ACTION");	// Action output from the core module
	AddInput("REWARD");
	AddOutput("ACTION");	// Selected action to the core module
	
	m_pInAction_v = NULL;
	m_pInReward_v = NULL;
	m_pOutAction_v = NULL;
	
	m_flTemp = GetFloatValue("temp", 0.1f);
	m_flMinTemp = GetFloatValue("min_temp", 0.0f);
	m_flMinVal = GetFloatValue("min_val", 0.0f);
	m_flDelta = GetFloatValue("delta", 0.0f);
}

BoltzmannSelect::~BoltzmannSelect(void)
{
	destroy_array(m_pNormAction_v);
}

void BoltzmannSelect::SetSizes(void)
{
	m_iActionLength = GetInputSize("ACTION");
	
	if(m_iActionLength != unknown_size)
	{
		SetOutputSize("ACTION", m_iActionLength);
	}
}

void BoltzmannSelect::Init(void)
{
	m_pInAction_v  = GetInputArray("ACTION");
	m_pOutAction_v = GetOutputArray("ACTION");
	
	if(InputConnected("REWARD"))
	{
		m_pInReward_v = GetInputArray("REWARD");
	}
	
	m_pNormAction_v = create_array(m_iActionLength);
	
	m_pRandGen = new TRanrotWGenerator((uint32)GetTickCount());	// random double
}

void BoltzmannSelect::Tick(void)
{
	int iMax = arg_max(m_pInAction_v, m_iActionLength);
	int iMin = arg_min(m_pInAction_v, m_iActionLength);
	reset_array(m_pOutAction_v, m_iActionLength);
	
	if(iMax == iMin)	// All outputs are the same
	{
		m_pOutAction_v[m_pRandGen->IRandom(0, m_iActionLength - 1)] = 0.25f;
	}
	else if(m_flTemp > 0.0015f) // Temperature is valid
	{
		int i, iSlct;
		copy_array(m_pNormAction_v, m_pInAction_v, m_iActionLength);
		add(m_pNormAction_v, m_flMinVal, m_iActionLength);
		normalize(m_pNormAction_v, m_iActionLength);
		
		double dSum = 0;
		double dAlpha = 1 / m_flTemp;
		for(i = 0; i < m_iActionLength; i++)
		{
			dSum += ::exp(dAlpha * m_pNormAction_v[i]);
		}
		
		if(dSum == 0)
		{
			iSlct = m_pRandGen->IRandom(0, m_iActionLength - 1);
		}
		else
		{
			double dRandom = m_pRandGen->Random() * dSum;
			dSum = 0;
			for(int i = 0; i < m_iActionLength; i++)
			{
				dSum += ::exp(dAlpha * m_pNormAction_v[i]);
				if(dSum >= dRandom)
				{
					iSlct = i;
					break;
				}
			}
		}
		
		if(iSlct == iMax)
		{
			m_pOutAction_v[iSlct] = 1.0f;
		}
		else
		{
			m_pOutAction_v[iSlct] = 0.5f;	// Random selection
		}
		
		if(m_pInReward_v != NULL && m_pInReward_v[0] > 0 && m_flDelta > 0)
		{
			m_flTemp = max(m_flTemp * m_flDelta, m_flMinTemp);	//  Lower temperature if a reinforcement is recived from the world
		}
	}
	else	// Greedy select
	{
		m_pOutAction_v[iMax] = 0.75f;
	}
}

static InitClass init("BoltzmannSelect", &BoltzmannSelect::Create, "Source/UserModules/CxQ/BoltzmannSelect/");