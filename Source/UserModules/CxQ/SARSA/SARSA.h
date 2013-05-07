//
//	SARSA.h		This file is a part of the IKAROS project
//                      Implements a simple maze generator
//
//    Copyright (C) 2009 Christian Balkenius
///
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
 
#ifndef SARSA_H
#define SARSA_H


#include "IKAROS.h"
#include "../randomc.h"
#include <windows.h>

class SARSA: public Module
{
public:
	static Module * Create(Parameter * p) { return new SARSA(p); }

	SARSA(Parameter*);
	virtual ~SARSA();

	void    SetSizes();
	void 	Init();
	void 	Tick();

private:
	int 	Select(float*);

	int		action_length;
	int		epochs;
	int		tick;
	int		print;
	int		last_print_tick;

	float	epsilon;
	float	gamma;

	float	*	new_actions;
	float	*	old_actions;
	float	*	old_reward;
	float	*	actions_out;
	float	*	target_out;
	
	TRanrotWGenerator * rand_gen;
};

#endif

