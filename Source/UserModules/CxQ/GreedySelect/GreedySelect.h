//
//	GreedySelect.h		This file is a part of the IKAROS project
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
//	Created: 2007-05-08
//

#include "IKAROS.h"
#include "../randomc.h"

using namespace ikaros;

class GreedySelect: public Module 
{
public:
			GreedySelect(Parameter*);
	virtual ~GreedySelect(void) {};
	static	Module* Create(Parameter * p) {return new GreedySelect(p);};
	
	void	SetSizes(void);
	void	Init(void);
	void	Tick(void);
	
private:
	int		HitWall(int);
	
	int		m_iActionLength;	// The length of the action vector
	
	float	m_flEpsilon;
	float	m_flDelta;
	
	float	*m_pInAction_v;		// From learning module
	float	*m_pInReward_v;
	float	*m_pOutAction_v;	// To selection module

	TRanrotWGenerator * m_pRand;
};