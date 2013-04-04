//
//	Punish.h		This file is a part of the IKAROS project
//
// 		Reinforcement learning module with separate learning module and action selection.
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
//	Created: 2007-07-08
//
//	A linear artificial neural network trained with a single error signal 
//	representing a punishment, instead of a target vector.

#include "IKAROS.h"

using namespace ikaros;

class Punish: public Module 
{
public:
			Punish(Parameter*);
    virtual ~Punish(void);
    
    static	Module* Create(Parameter*);
	
	void	SetSizes(void);
    void	Init(void);
    void	Tick(void);
	
private:	
	int		m_iObstacleLength;
	int		m_iActionLength;
	
	float	m_flLr;
	
	float	*m_pInObstacles_v;
	float	*m_pInTrainObstacles_v;
	float	*m_pInSelectedAction_v;
	float	*m_pInPunishment_v;
	float	*m_pOutAction_v;
	
	float	*m_pTeach_v;
	float	*m_pTrainOutput_v;
	float	**m_pWij_m;
};