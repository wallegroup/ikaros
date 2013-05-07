//
//	HashCoder.h		This file is a part of the IKAROS project
//
// 		Reinforcement learning module with separate learning module and action selection.
//
//    Copyright (C) 2013 Stefan Winberg
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

#include "HashCoder.h"


HashCoder::HashCoder(Parameter *pParam):Module(pParam)
{
	AddInput("INPUT");
	AddOutput("OUTPUT");
}


HashCoder::~HashCoder(void)
{
	destroy_array(float_array);
	delete [] tiles_array;
}


void HashCoder::SetSizes(void)
{
	tile_size = GetIntValue("tile_size");
	tilings = GetIntValue("tilings");
	output_length = tile_size * tile_size * tilings;

	SetOutputSize("OUTPUT", output_length);
}


void HashCoder::Init(void)
{
	input = GetInputArray("INPUT");
	output = GetOutputArray("OUTPUT");

	input_length = GetInputSize("INPUT");

	float_array = create_array(input_length);
	tiles_array = new int[output_length];
}


void HashCoder::Tick(void)
{
	float div = 1.0f / float(tile_size);
	for(int i=0; i<input_length; ++i)
		float_array[i] = input[i] / div;

	tiles(tiles_array, tilings, output_length, float_array, input_length);

	reset_array(output, output_length);
	for(int i=0; i<tilings; ++i)
	{
		output[tiles_array[i]] = 1.0f;
	}
}


static InitClass init("HashCoder", &HashCoder::Create, "Source/UserModules/CxQ/HashCoder/");