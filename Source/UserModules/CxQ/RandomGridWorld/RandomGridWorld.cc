//
//	RandomGridWorld.cc		This file is a part of the IKAROS project
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

#include "RandomGridWorld.h"

#include "ctype.h"


using namespace ikaros;

static int dx[4] = { 0, 1, 0, -1 };
static int dy[4] = { -1, 0, 1, 0 };



RandomGridWorld::~RandomGridWorld()
{
	destroy_matrix(lookup);
	delete rand_gen;
}



RandomGridWorld::RandomGridWorld(Parameter * p):Module(p)
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
RandomGridWorld::SetSizes()
{
	size_x = size_y = GetIntValue("size");

	mode	=	GetIntValueFromList("mode");
	noise	=	GetFloatValue("noise");
	step	=	min(GetFloatValue("step"), 0.0f);
	x_start =	max(min(GetIntValue("x_start"), size_x-2), 1);
	y_start =	max(min(GetIntValue("y_start"), size_y-2), 1);

	rand_gen = new TRanrotWGenerator((uint32)rand());	// random double

	if(mode == 1)
		while(!GeneratePerfectMaze());
	else if(mode == 2)
		while(!GenerateRoomMaze());
	else
		while(!GenerateRandomMaze());

	int states = 0;
	lookup = create_matrix(size_x, size_y);
	for(int j=1; j<size_y-1; j++)
	{
		for(int i=1; i<size_x-1; i++)
		{
			if(obstacles[j][i] < 1)
			{
				lookup[j][i] = states;
				++states;
			}
		}
	}

	SetOutputSize("IMAGE", size_x, size_y);

	if(mode > 1 && mode < 5)
		SetOutputSize("STATE", 4, states);	// with facing data
	else
		SetOutputSize("STATE", 1, states);	// without facing data

	SetOutputSize("LOCATION", size_x, size_y);
}




void
RandomGridWorld::Init()
{
	move		=	GetInputArray("MOVE");
	
	location	=	GetOutputMatrix("LOCATION");
	state		=	GetOutputMatrix("STATE");
	coordinate	=	GetOutputArray("COORDINATE");

	local_obstacles	=	GetOutputMatrix("LOCAL_OBSTACLES");
	local_values	=	GetOutputMatrix("LOCAL_VALUES");
 
	obstacles	=	GetInputMatrix("OBSTACLES");
	values		=	GetInputMatrix("VALUES");

	reward		=	GetOutputArray("REWARD");
	collision   =   GetOutputArray("COLLISION");
	image		=	GetOutputMatrix("IMAGE");

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
		//state[old_dir*(size_x*size_y)+old_y*size_x+old_x] = 0;
		//state[dir*(size_x*size_y)+y*size_x+x] = 1;

		state[(int)lookup[old_y][old_x]][old_dir] = 0;
		state[(int)lookup[y][x]][old_dir] = 1;
	}
	else
	{
		//state[old_y*size_x+old_x] = 0;
		//state[y*size_x+x] = 1;

		state[(int)lookup[old_y][old_x]][0] = 0;
		state[(int)lookup[y][x]][0] = 1;
	}

	SetSurrounding();
	Draw(x_start, y_start);
}



void
RandomGridWorld::Tick()
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
		state[(int)lookup[old_y][old_x]][old_dir] = 0;
		state[(int)lookup[y][x]][old_dir] = 1;
	}
	else
	{
		state[(int)lookup[old_y][old_x]][0] = 0;
		state[(int)lookup[y][x]][0] = 1;
	}

	old_x = x;
	old_y = y;
	old_dir = dir;

	SetSurrounding();
}



void
RandomGridWorld::Draw(int _x, int _y)
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
RandomGridWorld::SetSurrounding()
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



int
RandomGridWorld::GeneratePerfectMaze()
{
    reset_matrix(obstacles, size_x, size_y);
	
	// Fill in walls
    
    for(int j=0; j<size_y; j+=2)
        for(int i=0; i<size_x; i++)
            obstacles[j][i] = 1;
            
    for(int i=0; i<size_x; i+=2)
        for(int j=0; j<size_y; j++)
            obstacles[j][i] = 1;

	// Fill in outer walls
    
	for(int i=0; i<size_x; ++i)
	{
		obstacles[0][i] = 1;
		obstacles[size_y-1][i] = 1;
	}

	for(int j=0; j<size_y; ++j)
	{
		obstacles[j][0] = 1;
		obstacles[j][size_x-1] = 1;
	}

    // Generate the maze
    int s = size_x;
	if(s % 2 == 0)
		s -= 1;

	int half_size = int(s/2);
	int total_cells = half_size*half_size;
    int * stack = new int [total_cells];
    int sp = 0;
    int xx, yy;
	
    int current_cell = (total_cells-1) * rand_gen->Random();
    int visited_cells = 1;
	int old_r = -1;

    while(visited_cells < total_cells)
    {
        int x = current_cell % half_size;
        int y = current_cell / half_size;

        bool cell[4] = {false, false, false, false};
        int cells = 0;
        
        yy = 2*y+1;
        xx = 2*(x-1)+1;
        if(x>0 && obstacles[yy][xx-1] == 1  && obstacles[yy][xx+1] == 1  && obstacles[yy-1][xx] == 1  && obstacles[yy+1][xx] == 1 )
        {
            cell[0] = true;
            cells++;
        }
        
        yy = 2*y+1;
        xx = 2*(x+1)+1;
        if(x<half_size-1 && obstacles[yy][xx-1] == 1  && obstacles[yy][xx+1] == 1  && obstacles[yy-1][xx] == 1  && obstacles[yy+1][xx] == 1 )
        {
            cell[1] = true;
            cells++;
        }
        
        yy = 2*(y-1)+1;
        xx = 2*x+1;
        if(y>0 && obstacles[yy][xx-1] == 1  && obstacles[yy][xx+1] == 1  && obstacles[yy-1][xx] == 1  && obstacles[yy+1][xx] == 1 )
        {
            cell[2] = true;
            cells++;
        }
        
        yy = 2*(y+1)+1;
        xx = 2*x+1;
        if(y<half_size-1 && obstacles[yy][xx-1] == 1  && obstacles[yy][xx+1] == 1  && obstacles[yy-1][xx] == 1  && obstacles[yy+1][xx] == 1 )
        {
            cell[3] = true;
            cells++;
        }

        if(cells > 0)
        {
            int r = min(4*rand_gen->Random(), 3);

			if(old_r > -1 && rand_gen->Random() < 0.333)
				r = old_r;
			old_r = r;

            while(!cell[r])
                r = (r+1) % 4;
            
            stack[sp++] = current_cell;

            switch(r)
            {
                case 0:
                    yy = 2*y+1;
                    xx = 2*x+1-1;
                    obstacles[yy][xx] = 0;
                    current_cell = y*half_size+(x-1);
                    break;

                case 1:
                    yy = 2*y+1;
                    xx = 2*x+1+1;
                    obstacles[yy][xx] = 0;
                    current_cell = y*half_size+(x+1);
                    break;

                case 2:
                    yy = 2*y+1-1;
                    xx = 2*x+1;
                    obstacles[yy][xx] = 0;
                    current_cell = (y-1)*half_size+x;
                    break;

                case 3:
                    yy = 2*y+1+1;
                    xx = 2*x+1;
                    obstacles[yy][xx] = 0;
                    current_cell = (y+1)*half_size+x;
                    break;
            }
            visited_cells++;
        }
        
        else // pop
        {
            current_cell = stack[--sp];
        }
    }

	if(obstacles[y_start][x_start] > 0)
		return FALSE;

	free(stack);

	obstacles[y_start][x_start] = 0;	// clear start pos

	// flood fill the maze to find the furthest accessible tile
	return FindMaxDistance();
}


int
RandomGridWorld::GenerateRandomMaze()
{
	// Fill in walls
	
	for(int i=0; i<size_x; ++i)
	{
		obstacles[0][i] = 1;
		obstacles[size_y-1][i] = 1;
	}

	for(int j=0; j<size_y; ++j)
	{
		obstacles[j][0] = 1;
		obstacles[j][size_x-1] = 1;
	}

	// Generate the maze

	for(int j=1; j<size_y-1; ++j)
	{
		for(int i=1; i<size_x-1; ++i)
		{
			if(rand_gen->Random() < noise)
				obstacles[j][i] = 1;
			else
				obstacles[j][i] = 0;
		}
	}

	obstacles[y_start][x_start] = 0;	// clear start pos

	// flood fill the maze to find the furthest accessible tile
	return FindMaxDistance();
}



int
RandomGridWorld::GenerateRoomMaze()
{
    reset_matrix(obstacles, size_x, size_y);

	// Fill in walls
	
	for(int i=0; i<size_x; ++i)
	{
		obstacles[0][i] = 1;
		obstacles[size_y-1][i] = 1;
	}

	for(int j=0; j<size_y; ++j)
	{
		obstacles[j][0] = 1;
		obstacles[j][size_x-1] = 1;
	}

	// Generate the rooms

	int j = 0;
	while(true)
	{
		j = j + rand_gen->IRandom(4, max(5, size_y*0.6));
		if(j < size_y-3)
		{
			float chance = 0.04;
			for(int i=1; i<size_x-1; ++i)
			{
				if(rand_gen->Random() < chance)
				{
					obstacles[j][i] -= 1;
					chance = 0.04;
				}
				else
				{
					obstacles[j][i] += 1;
					chance *= 1.8;
				}
			}
		}
		else
		{
			break;
		}
	}

	int i = 0;
	while(true)
	{
		i = i + rand_gen->IRandom(4, max(5, size_x*0.6));
		if(i < size_x-3)
		{
			float chance = 0.04;
			for(int j=1; j<size_y-1; ++j)
			{
				if(rand_gen->Random() < chance)
				{
					obstacles[j][i] -= 1;
					chance = 0.04;
				}
				else
				{
					obstacles[j][i] += 1;
					chance *= 1.8;
				}
			}
		}
		else
		{
			break;
		}
	}

	for(int j=1; j<size_y-1; ++j)
	{
		for(int i=1; i<size_x-1; ++i)
		{	
			if(obstacles[j][i] < 0)
				obstacles[j][i] = 0;
			else if(obstacles[j][i] > 1)
				obstacles[j][i] = 1;
		}
	}

	obstacles[y_start][x_start] = 0;	// clear start pos

	// flood fill the maze to find the furthest accessible tile
	return FindMaxDistance();
}



float
RandomGridWorld::FindMaxDistance()
{
	reset_matrix(values, size_x, size_y);
	values[y_start][x_start] = 1;

	int tiles = -1;
	for(int i=1; i<size_x-1; ++i)
		for(int j=1; j<size_y-1; ++j)
			if(obstacles[j][i] < 1)
				++tiles;
	
	float max_dist = -1;
	for(int k=0; k<tiles; ++k)	// repeat this just in case we missed a tile
	{
		for(int i=1; i<size_x-1; ++i)
		{
			for(int j=1; j<size_y-1; ++j)
			{
				if(values[j][i] < 1 && obstacles[j][i] < 1)
				{
					if(values[j-1][i] > 0 || values[j+1][i] > 0 || values[j][i-1] > 0 || values[j][i+1] > 0)
					{
						// assign a distance if we have visited a neighbor
						values[j][i] = 0xFFFFFF;
						if(values[j-1][i] > 0 && values[j-1][i] < values[j][i])
							values[j][i] = values[j-1][i];
						if(values[j+1][i] > 0 && values[j+1][i] < values[j][i])
							values[j][i] = values[j+1][i];
						if(values[j][i-1] > 0 && values[j][i-1] < values[j][i])
							values[j][i] = values[j][i-1];
						if(values[j][i+1] > 0 && values[j][i+1] < values[j][i])
							values[j][i] = values[j][i+1];

						values[j][i] += 1;
						if(values[j][i] > max_dist)
							max_dist = values[j][i];
					}
				}
			}
		}
	}

	int goalx = -1;
	int goaly = -1;

    for(int j=1; j<size_y-1; ++j)
	{
        for(int i=1; i<size_x-1; ++i)
		{
			if(values[j][i] >= max_dist-0.01)
			{
				goalx = i;
				goaly = j;
			}
			
			if(values[j][i] < 0.01)
				obstacles[j][i] = 1;	// fill inaccessible tiles
			
			values[j][i] = step;
		}
	}

	if(goalx > 1 && goaly > 1)
		values[goaly][goalx] = 1;
	else
		return FALSE;	// try again
	
	if(max_dist < max(size_x, size_y))
		return FALSE;	// try again
	
	printf("distance %i\n", (int)max_dist-1);

	return max_dist;
}



static InitClass init("RandomGridWorld", &RandomGridWorld::Create, "Source/UserModules/CxQ/RandomGridWorld/");