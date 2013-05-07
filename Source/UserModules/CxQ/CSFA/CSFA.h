//
//	CSFA.h		This file is a part of the IKAROS project
//
//		Context-sensitive function approximator. Basicly a linear 
//		network with context functionallity.
//
//    Copyright (C) 2005 Stefan Winberg
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

#include "IKAROS.h"

using namespace ikaros;

#define ARROWS 0

class CSFA: public Module
{
public:
			CSFA(Parameter*);	
    virtual ~CSFA(void);
	static	Module* Create(Parameter* pParam) {return new CSFA(pParam);};
	
	void 	Init(void);
	void	Tick(void);
	
private:
	inline void	 Ask(float*, float*, float*);
	inline void	 Train(float*, float*, float*, float*, float flScale = 1.0f);
	
	// IKAROS
	float	*m_InInput;
	float	*m_InContext;
	float	*m_InTrainInput;
	float	*m_InTrainContext;
	float	*m_InTeach;
	float	*m_InLearn;
	float	*m_OutOutput;
	
	// ANN
	int		m_InputLength;
	int		m_ContextLength;
	int		m_OutputLength;

	bool	m_limit;
	
	float	m_LrW_STR;
	float	m_LrW_WEA;
	float	m_LrU_INH;
	float	m_LrU_DIS;
	float	m_StartWt;
	float	m_StartInh;
	
	float	*m_TrainOutput;
	float	*m_Error;
	float	**m_Weights;
	float	**m_OutputLimit;
	float	***m_Inhibition;
	
	float *** create_cube(int iSizeX, int iSizeY, int iSizeZ)
	{
		float ***a = new float** [iSizeZ];
		for(int i = 0; i < iSizeZ; i++)
			a[i] = create_matrix(iSizeX, iSizeY);
		return a;
	}
	
	void destroy_cube(float ***a)
	{
		if(a == NULL)
			return;
		free(**a);
		free(*a);
		free(a);
	}

	// DEBUG
#if ARROWS
	void	SetSurrounding(int, int);

	float	**m_InImage_m;
	float	**m_OutValueX_m;
	float	**m_OutValueY_m;
#endif
};