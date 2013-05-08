//
//	GridWorld.cc		This file is a part of the IKAROS project
//					Implements a simple grid world with obstacles and rewards
//
//    Copyright (C) 2003-2007 Christian Balkenius
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
//	Created: 2003-07-14
//
//	surrounding: variable size + rotation, direction in image

#include "GridWorld.h"

#include "ctype.h"


using namespace ikaros;

static int dx[4] = { 0, 1, 0, -1 };
static int dy[4] = { -1, 0, 1, 0 };



GridWorld::GridWorld(Parameter * p):Module(p)
{
	AddInput("OBSTACLES");
	AddInput("VALUES");

	AddInput("MOVE");

	AddOutput("REWARD", 1);
	AddOutput("COLLISION", 1);
	AddOutput("COORDINATE", 2);
	AddOutput("LOCATION");
	AddOutput("STATE");
	AddOutput("LOCAL_OBSTACLES", 3, 3);
	AddOutput("LOCAL_VALUES", 3, 3);

	AddOutput("IMAGE");

	normalize_coordinate = GetBoolValue("normalize_coordinate");
	mode = GetIntValueFromList("mode");
}



void
GridWorld::SetSizes()
{
	image_size_x = size_x = GetInputSizeX("OBSTACLES");
	image_size_y = size_y = GetInputSizeY("OBSTACLES");

	if(size_x == unknown_size || size_y == unknown_size)
		return;

	SetOutputSize("IMAGE", size_x, size_y);

	if(mode > 1 && mode < 5)
		SetOutputSize("STATE", size_x * size_y * 4);	// with facing data
	else
		SetOutputSize("STATE", size_x * size_y);		// without facing data

	SetOutputSize("LOCATION", size_x, size_y);
}




void
GridWorld::Init()
{
	move		=	GetInputArray("MOVE");
	
	location	=	GetOutputMatrix("LOCATION");
	state		=	GetOutputArray("STATE");
	coordinate	=	GetOutputArray("COORDINATE");

	local_obstacles	=	GetOutputMatrix("LOCAL_OBSTACLES");
	local_values	=	GetOutputMatrix("LOCAL_VALUES");
 
	obstacles	=	GetInputMatrix("OBSTACLES");
	values		=	GetInputMatrix("VALUES");

	reward		=	GetOutputArray("REWARD");
	collision   =   GetOutputArray("COLLISION");
	image		=	GetOutputMatrix("IMAGE");

	x_start = max(min(GetIntValue("x_start"), size_x-2), 1);
	y_start = max(min(GetIntValue("y_start"), size_y-2), 1);

	x = x_start;
	y = y_start;
	old_x = x_start;
	old_y = y_start;
	dir = 0;
	old_dir = 0;

	location[old_y][old_x] = 0;
	location[y][x] = 1;

	if(mode > 1 && mode < 5)
	{
		state[old_dir*(size_x*size_y)+old_y*size_x+old_x] = 0;
		state[dir*(size_x*size_y)+y*size_x+x] = 1;
	}
	else
	{
		state[old_y*size_x+old_x] = 0;
		state[y*size_x+x] = 1;
	}

	SetSurrounding();
	Draw(x_start, y_start);
}



void
GridWorld::Tick()
{
	if(move == NULL || zero(move, ((mode == 4 || mode < 2) ? 4 : 3)))
	{
		reward[0] = 0;
		Draw(x, y);
		return;
	}

	int xr = 0;
	int yr = 0;
	int mv = 0;

	switch (mode)
	{
		case 0: // sum
		{
			float m[4] = {move[0]-move[2], move[1]-move[3], move[2]-move[0], move[3]-move[1]};
			mv = arg_max(m, 4);
			xr = dx[mv];
			yr = dy[mv];
		}	break;

		case 1: // max
		{
			mv = arg_max(move, 4);
			xr = dx[mv];
			yr = dy[mv];
		}	break;

		case 2: // relative sum
		{
			mv = arg_max(move, 3);
			if(mv == 2)
			{
				xr = dx[dir];
				yr = dy[dir];
			}
			else if(mv == 1)
				dir = (dir + 1) % 4;
			else
				dir = (dir - 1 + 4) % 4;
		}	break;

		case 3: // relative max
		{
			float m[3] = {move[0]-move[1], move[1]-move[0], move[3]};
			mv = arg_max(m, 3);
			if(mv == 2)
			{
				xr = dx[dir];
				yr = dy[dir];
			}
			else if(mv == 1)
				dir = (dir + 1) % 4;
			else
				dir = (dir - 1 + 4) % 4;
		}	break;

		case 4: // relative turn & move
		case 5:
		{
			mv = arg_max(move, 4);
			if(mv == 1)			// move right
				dir = (dir + 1) % 4;
			else if(mv == 2)	// move left
				dir = (dir - 1 + 4) % 4;
			else if(mv == 3)	// move back
				dir = (dir - 2 + 4) % 4;

			xr = dx[dir];
			yr = dy[dir];
		}	break;

		default:
			;
	}

	// Agent can not occupy the outer border of the grid

	if(x+xr > 0 && x+xr < size_x-1 && y+yr > 0 && y+yr < size_y-1 && obstacles[y+yr][x+xr] != 1)
	{
		x += xr;
		y += yr;
		*collision = 1;
	}
	else
	{
		*collision = 0;
	}
	
	Draw(x, y);

	reward[0] = values[y][x];

	if(reward != NULL && reward[0] > 0)
	{
		x = x_start;
		y = y_start;
		dir = 0;
	}

	if(normalize_coordinate)
	{
		coordinate[0] = 1.0f / (2.0f*float(image_size_x)) + float(x) / float(image_size_x);
		coordinate[1] = 1.0f / (2.0f*float(image_size_y)) + float(y) / float(image_size_y);
	}
	else
	{
		coordinate[0] = x;
		coordinate[1] = y;
	}

	location[old_y][old_x] = 0;
	location[y][x] = 1;

	if(mode > 1 && mode < 5)
	{
		state[old_dir*(size_x*size_y)+old_y*size_x+old_x] = 0;
		state[dir*(size_x*size_y)+y*size_x+x] = 1;
	}
	else
	{
		state[old_y*size_x+old_x] = 0;
		state[y*size_x+x] = 1;
	}

	old_x = x;
	old_y = y;
	old_dir = dir;

	SetSurrounding();
}



void
GridWorld::Draw(int _x, int _y)
{
	copy_matrix(image, obstacles, image_size_x, image_size_x);

	for (int j=0; j<image_size_x; j++)
		for (int i=0; i<image_size_x; i++)
			if(values[j][i] > 0)
				image[j][i] = 3;

	image[_y][_x] = 2;

	// Draw facing

	if(mode > 1)
		image[_y+dy[dir]][_x+dx[dir]] = 2;
}



void
GridWorld::SetSurrounding()
{
	if(dir == 0)	// N
	{
		for (int i=0; i<3; i++)
			for (int j=0; j<3; j++)
			   local_obstacles[j][i] = obstacles[y+j-1][x+i-1];

		for (int i=0; i<3; i++)
			for (int j=0; j<3; j++)
				local_values[j][i] = values[y+j-1][x+i-1];
	}
	else if(dir == 1)	// E
	{
		local_obstacles[0][0] = obstacles[y-1][x+1];
		local_obstacles[0][1] = obstacles[y][x+1];
		local_obstacles[0][2] = obstacles[y+1][x+1];

		local_obstacles[1][0] = obstacles[y-1][x];
		local_obstacles[1][1] = obstacles[y][x];
		local_obstacles[1][2] = obstacles[y+1][x];

		local_obstacles[2][0] = obstacles[y-1][x-1];
		local_obstacles[2][1] = obstacles[y][x-1];
		local_obstacles[2][2] = obstacles[y+1][x-1];

	}
	else if(dir == 2)	// S
	{
		local_obstacles[0][0] = obstacles[y+1][x+1];
		local_obstacles[0][1] = obstacles[y+1][x];
		local_obstacles[0][2] = obstacles[y+1][x-1];

		local_obstacles[1][0] = obstacles[y][x+1];
		local_obstacles[1][1] = obstacles[y][x];
		local_obstacles[1][2] = obstacles[y][x-1];

		local_obstacles[2][0] = obstacles[y-1][x+1];
		local_obstacles[2][1] = obstacles[y-1][x];
		local_obstacles[2][2] = obstacles[y-1][x-1];
	}
	else	// W
	{
		local_obstacles[0][0] = obstacles[y+1][x-1];
		local_obstacles[0][1] = obstacles[y][x-1];
		local_obstacles[0][2] = obstacles[y-1][x-1];

		local_obstacles[1][0] = obstacles[y+1][x];
		local_obstacles[1][1] = obstacles[y][x];
		local_obstacles[1][2] = obstacles[y-1][x];

		local_obstacles[2][0] = obstacles[y+1][x+1];
		local_obstacles[2][1] = obstacles[y][x+1];
		local_obstacles[2][2] = obstacles[y-1][x+1];
	}
}



static InitClass init("GridWorld", &GridWorld::Create, "Source/Modules/EnvironmentModules/GridWorld/");