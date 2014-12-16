//
//		Arbiter.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2006 Christian Balkenius
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

// input: 2 x (vector weight)
// output: (vector with max weight (output) and the max veight (value))
// allows cascading

#ifndef ARBITER
#define ARBITER

#include "IKAROS.h"

class Arbiter: public Module
{
public:
    static Module * Create(Parameter * p) { return new Arbiter(p); }

    float *	input1;
    float *	input2;
    float *	value1;
    float *	value2;
    float *	output;
    float *	value;
    int		size;

    Arbiter(Parameter * p) : Module(p) {};
    virtual	~Arbiter() {};

    void		Init();
    void		Tick();
};

#endif
