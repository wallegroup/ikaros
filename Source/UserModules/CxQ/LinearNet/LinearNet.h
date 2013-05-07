//
//	LinearNet.h		This file is a part of the IKAROS project
//
//		Linear function approximator. Accepts context data but will only add it to the input vector.
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

#include "IKAROS.h"
#include "../randomc.h"
#include <windows.h>

using namespace ikaros;

class LinearNet: public Module
{
public:
				LinearNet(Parameter*);	
        virtual ~LinearNet(void);
		
		static	Module* Create(Parameter *pParam) {	return new LinearNet(pParam); };
		
		void 	Init(void);
		void 	Tick(void);

private:
		// IKAROS
		float	*m_pInInput_v;
		float	*m_pInTrain_v;
		float	*m_pInTrainInput_v;
		float	*m_pInTeach_v;
		float	*m_pInLearn_v;
		float	**m_pInImage_m;
		
		float	*m_pOutOutput_v;
		float	**m_pOutValueX_m;
		float	**m_pOutValueY_m;
		
		// ANN
		int		m_iInputLength;
		int		m_iOutputLength;

		bool	m_bLimit;
		
		float	m_flLr;
		float	m_flMinWt;
		float	m_flMaxWt;
		float	m_flMaxSum;
		float	m_flStartWt;
		float	m_flRandomWt;
		
		float	*m_pTrainOutput_v;
		float	**m_pWij_m;

		TRanrotWGenerator * rand_gen;
};