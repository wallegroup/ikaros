//
//	Merge.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2014 Christian Balkenius
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

#ifndef Merge_
#define Merge_

#include "IKAROS.h"

class Merge: public Module
{
public:
    static Module * Create(Parameter * p) { return new Merge(p); }

    Merge(Parameter * p) : Module(p) {}
    virtual ~Merge() {};

    void 		Init();
    void 		Tick();

    float **    input1;
    float **    input2;
    
    float **    output;
    
    int         size_x;
    int         size_y1;
    int         size_y2;
    
    int         id_column;
    int         max_rows;
};

#endif

