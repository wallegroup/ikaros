//
//	RandomMazeGenerator.cc		This file is a part of the IKAROS project
//                          Implements a simple maze generator
//
//    Copyright (C) 2009 Christian Balkenius
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



#include "RandomMazeGenerator.h"

using namespace ikaros;


RandomMazeGenerator::~RandomMazeGenerator()
{
	delete rand_gen;
}



void
RandomMazeGenerator::SetSizes()
{
    size = GetIntValue("size");

	/*if((size < 2) || ((size & (size - 1)) != 0))
	{
		Notify(msg_fatal_error, "Module \"%s\": The 'size' parameter is a power of two.\n", GetName());
		return;
	}*/

    SetOutputSize("OUTPUT", size, size);
    SetOutputSize("GOAL", size, size);
}



int
RandomMazeGenerator::GeneratePerfectMaze()
{
    reset_matrix(output, size, size);
	
	// Fill in walls
    
    for(int j=0; j<size; j+=2)
        for(int i=0; i<size; i++)
            output[j][i] = 1;
            
    for(int i=0; i<size; i+=2)
        for(int j=0; j<size; j++)
            output[j][i] = 1;

	// Fill in outer walls
    
	for(int i=0; i<size; ++i)
	{
		output[0][i] = 1;
		output[size-1][i] = 1;
	}

	for(int j=0; j<size; ++j)
	{
		output[j][0] = 1;
		output[j][size-1] = 1;
	}

    // Generate the maze
    
	int half_size = int(size/2)-1;
	int total_cells = half_size*half_size;
    int * stack = new int [total_cells];
    int sp = 0;
    int xx, yy;
	
    int current_cell = (total_cells-1) * rand_gen->Random();
    int visited_cells = 1;

    while(visited_cells < total_cells)
    {
        int x = current_cell % half_size;
        int y = current_cell / half_size;

        bool cell[4] = {false, false, false, false};
        int cells = 0;
        
        yy = 2*y+1;
        xx = 2*(x-1)+1;
        if(x>0 && output[yy][xx-1] == 1  && output[yy][xx+1] == 1  && output[yy-1][xx] == 1  && output[yy+1][xx] == 1 )
        {
            cell[0] = true;
            cells++;
        }
        
        yy = 2*y+1;
        xx = 2*(x+1)+1;
        if(x<half_size-1 && output[yy][xx-1] == 1  && output[yy][xx+1] == 1  && output[yy-1][xx] == 1  && output[yy+1][xx] == 1 )
        {
            cell[1] = true;
            cells++;
        }
        
        yy = 2*(y-1)+1;
        xx = 2*x+1;
        if(y>0 && output[yy][xx-1] == 1  && output[yy][xx+1] == 1  && output[yy-1][xx] == 1  && output[yy+1][xx] == 1 )
        {
            cell[2] = true;
            cells++;
        }
        
        yy = 2*(y+1)+1;
        xx = 2*x+1;
        if(y<half_size-1 && output[yy][xx-1] == 1  && output[yy][xx+1] == 1  && output[yy-1][xx] == 1  && output[yy+1][xx] == 1 )
        {
            cell[3] = true;
            cells++;
        }

        if(cells > 0)
        {
            int r = min(4*rand_gen->Random(), 3);
            while(!cell[r])
                r = (r+1) % 4;
            
            stack[sp++] = current_cell;

            switch(r)
            {
                case 0:
                    yy = 2*y+1;
                    xx = 2*x+1-1;
                    output[yy][xx] = 0;
                    current_cell = y*half_size+(x-1);
                    break;

                case 1:
                    yy = 2*y+1;
                    xx = 2*x+1+1;
                    output[yy][xx] = 0;
                    current_cell = y*half_size+(x+1);
                    break;

                case 2:
                    yy = 2*y+1-1;
                    xx = 2*x+1;
                    output[yy][xx] = 0;
                    current_cell = (y-1)*half_size+x;
                    break;

                case 3:
                    yy = 2*y+1+1;
                    xx = 2*x+1;
                    output[yy][xx] = 0;
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

	if(output[starty][startx] > 0)
		return FALSE;

	free(stack);

	output[starty][startx] = 0;	// clear start pos

	// flood fill the maze to find the furthest accessible tile
	return FindMaxDistance();
}


int
RandomMazeGenerator::GenerateRandomMaze()
{
	// Fill in walls
	
	for(int i=0; i<size; ++i)
	{
		output[0][i] = 1;
		output[size-1][i] = 1;
	}

	for(int j=0; j<size; ++j)
	{
		output[j][0] = 1;
		output[j][size-1] = 1;
	}

	// Generate the maze

	for(int j=1; j<size-1; ++j)
	{
		for(int i=1; i<size-1; ++i)
		{
			if(rand_gen->Random() < noise)
				output[j][i] = 1;
			else
				output[j][i] = 0;
		}
	}

	output[starty][startx] = 0;	// clear start pos

	// flood fill the maze to find the furthest accessible tile
	return FindMaxDistance();
}



int
RandomMazeGenerator::GenerateRoomMaze()
{
    reset_matrix(output, size, size);

	// Fill in walls
	
	for(int i=0; i<size; ++i)
	{
		output[0][i] = 1;
		output[size-1][i] = 1;
	}

	for(int j=0; j<size; ++j)
	{
		output[j][0] = 1;
		output[j][size-1] = 1;
	}

	// Generate the rooms

	int j = 0;
	while(true)
	{
		j = j + rand_gen->IRandom(4, max(5, size*0.6));
		if(j < size-3)
		{
			float chance = 0.04;
			for(int i=1; i<size-1; ++i)
			{
				if(rand_gen->Random() < chance)
				{
					output[j][i] -= 1;
					chance = 0.04;
				}
				else
				{
					output[j][i] += 1;
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
		i = i + rand_gen->IRandom(4, max(5, size*0.6));
		if(i < size-3)
		{
			float chance = 0.04;
			for(int j=1; j<size-1; ++j)
			{
				if(rand_gen->Random() < chance)
				{
					output[j][i] -= 1;
					chance = 0.04;
				}
				else
				{
					output[j][i] += 1;
					chance *= 1.8;
				}
			}
		}
		else
		{
			break;
		}
	}

	for(int j=1; j<size-1; ++j)
	{
		for(int i=1; i<size-1; ++i)
		{	
			if(output[j][i] < 0)
				output[j][i] = 0;
			else if(output[j][i] > 1)
				output[j][i] = 1;
		}
	}

	output[starty][startx] = 0;	// clear start pos

	// flood fill the maze to find the furthest accessible tile
	return FindMaxDistance();
}



float
RandomMazeGenerator::FindMaxDistance()
{
	reset_matrix(goal, size, size);
	goal[starty][startx] = 1;
	
	float max_dist = -1;
	for(int k=0; k<size; ++k)	// repeat this just in case we have missed a tile
	{
		for(int i=1; i<size-1; ++i)
		{
			for(int j=1; j<size-1; ++j)
			{
				if(goal[j][i] < 1 && output[j][i] < 1)
				{
					if(goal[j-1][i] > 0 || goal[j+1][i] > 0 || goal[j][i-1] > 0 || goal[j][i+1] > 0)
					{
						// assign a distance if we have visited a neighbor
						goal[j][i] = 0xFFFFFF;
						if(goal[j-1][i] > 0 && goal[j-1][i] < goal[j][i])
							goal[j][i] = goal[j-1][i];
						if(goal[j+1][i] > 0 && goal[j+1][i] < goal[j][i])
							goal[j][i] = goal[j+1][i];
						if(goal[j][i-1] > 0 && goal[j][i-1] < goal[j][i])
							goal[j][i] = goal[j][i-1];
						if(goal[j][i+1] > 0 && goal[j][i+1] < goal[j][i])
							goal[j][i] = goal[j][i+1];

						goal[j][i] += 1;
						if(goal[j][i] > max_dist)
							max_dist = goal[j][i];
					}
				}
			}
		}
	}

	int goalx = -1;
	int goaly = -1;

    for(int j=1; j<size-1; ++j)
	{
        for(int i=1; i<size-1; ++i)
		{
			if(goal[j][i] >= max_dist-0.01)
			{
				goalx = i;
				goaly = j;
			}
			/*
			if(goal[j][i] < 0.01)
				output[j][i] = 1;	// fill inaccessible tiles
			*/
			goal[j][i] = step;
		}
	}

	if(goalx > 1 && goaly > 1)
		goal[goaly][goalx] = 1;
	else
		return FALSE;	// try again
	
	if(max_dist < size)
		return FALSE;	// try again
	
	printf("distance %i\n", (int)max_dist);

	return max_dist;
}



void
RandomMazeGenerator::Init()
{
	output	=	GetOutputMatrix("OUTPUT");
	goal	=	GetOutputMatrix("GOAL");

	mode	=	GetIntValueFromList("mode");
	noise	=	GetFloatValue("noise");
	step	=	GetFloatValue("step");
	startx	=	GetIntValue("x_start");
	starty	=	GetIntValue("y_start");
	seed	=	GetIntValue("seed");

	if(seed < 0)
	{
		rand_gen = new TRanrotWGenerator((uint32)GetTickCount());
	}
	else if(seed > 0)
	{
		rand_gen = new TRanrotWGenerator((uint32)seed);
	}
	else
	{
		rand_gen = new TRanrotWGenerator(rand());
		srand(GetTickCount());
	}

	if(mode == 0)
		while(!GenerateRandomMaze());
	else if(mode == 1)
		while(!GeneratePerfectMaze());
	else if(mode == 2)
		while(!GenerateRoomMaze());
}



static InitClass init("RandomMazeGenerator", &RandomMazeGenerator::Create, "Source/UserModules/CxQ/RandomMazeGenerator/");