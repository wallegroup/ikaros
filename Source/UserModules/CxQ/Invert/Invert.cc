//
//	Invert.cc	This file is a part of the IKAROS project
//
//
//    Copyright (C) 2004  Christian Balkenius
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

#include "Invert.h"


void
Invert::Init()
{
    input  = GetInputArray("INPUT");
    output = GetOutputArray("OUTPUT");
}


void
Invert::SetSizes()
{
	input_size = GetInputSize("INPUT");
	SetOutputSize("OUTPUT", input_size);
}


void
Invert::Tick()
{
    for(int i = 0; i < input_size; ++i)
	{
		if(input[i] > 0.5f)
			output[i] = 0.0f;
		else
			output[i] = 1.0f;
	}
}


static InitClass init("Invert", &Invert::Create, "Source/UserModules/CxQ/Invert/");