//
//	RandomMazeGenerator.h		This file is a part of the IKAROS project
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
 
#ifndef RandomMazeGenerator_
#define RandomMazeGenerator_


#include "IKAROS.h"
#include "../randomc.h"
#include <windows.h>

class RandomMazeGenerator: public Module
{
public:
	int     size;
	int     startx;
	int     starty;
	int     mode;
	int     seed;

	float   noise;
	float   step;

	float	**	output;
	float	**	goal;

	static Module * Create(Parameter * p) { return new RandomMazeGenerator(p); }

	RandomMazeGenerator(Parameter * p) : Module(p) {}
	virtual ~RandomMazeGenerator();

	void    SetSizes();
	void 	Init();
	void 	Tick() {};
	int		GenerateRandomMaze();
	int		GeneratePerfectMaze();
	int		GenerateRoomMaze();
	float	FindMaxDistance();

	TRanrotWGenerator	*	rand_gen;
};

#endif

