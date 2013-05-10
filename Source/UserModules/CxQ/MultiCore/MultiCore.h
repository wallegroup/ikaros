//
//	MultiCore.h		This file is a part of the IKAROS project
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

class MultiCore: public Module 
{
public:
	MultiCore(Parameter * param):Module(param) {};
	virtual ~MultiCore(void);
    static	Module* MultiCore::Create(Parameter * param) { return new MultiCore(param); };
	
	void	SetSizes(void);
    void	Init(void);
    void	Tick(void);

private:
	int		tick;
	int		print;
	int		average;
	int		last_print_tick;
	int		last_action;

	int		action_length;				// The length of the action vector
	int		infinite_horizon;
	int		select;
	int		epochs;
	
	float	discount;
	float	exploration;
	
	float	*	InGradient;				// From learning module
	float	*	InLastGradient;
	float	*	InBias;					// From learning module
	float	*	InLastBias;
	float	*	InPunish;				// From learning module
	float	*	InReward;				// From world module
	
	float	*	OutGradientTarget;
	float	*	OutBiasTarget;
	float	*	OutSelectedAction;

	TRanrotWGenerator * rand_gen;
};