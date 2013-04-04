//
//	HierarchicalCoder.h		This file is a part of the IKAROS project
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

#include "HierarchicalCoder.h"

HierarchicalCoder::HierarchicalCoder(Parameter *pParam):Module(pParam)
{
	AddInput("COORDINATE");
	AddOutput("LOCATION");
}

void HierarchicalCoder::SetSizes(void)
{
	value = GetFloatValue("value");
	size = GetIntValue("size");
	SetOutputSize("LOCATION", size, size);
}

void HierarchicalCoder::Init(void)
{
	coordinate = GetInputArray("COORDINATE");
	location = GetOutputMatrix("LOCATION");
}

void HierarchicalCoder::Tick(void)
{
	reset_matrix(location, size, size);
	location[int(coordinate[1]*size)][int(coordinate[0]*size)] = value;
}

static InitClass init("HierarchicalCoder", &HierarchicalCoder::Create, "Source/UserModules/CxQ/HierarchicalCoder/");