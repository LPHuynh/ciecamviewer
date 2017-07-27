/*
* CIECAM02 Viewer - Copyright © 2017 Long Huynh (longp.huynh@gmail.com)
*
*  This program is free software; you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by the Free
*  Software Foundation; either version 3 of the License, or (at your option)
*  any later version.
*
*  This program is distributed in the hope that it will be useful, but
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
*  for more details.
*/

#include <SFML/Graphics.hpp>

#include <cmath>
#include <cstdlib>

#include "ciecamviewer.h"
#include "color.h"

#include "json.hpp"
#include <fstream>

int main() 
{   
    CIECAMViewer mainProgram;

    mainProgram.Run();

    return 0;
}
