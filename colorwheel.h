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

#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include "color.h"

enum class GridType {Circle, Hex, Solid};
enum class WheelType {JCh, Jsh};

class ColorWheel
{
public:
    ColorWheel();

    void update(const GridType& gridType, const WheelType& wheelType);
    
    ViewingCondition vc;

    int tiltDegree{0}, rotateDegree{0};
    int lightness{50};
    int chroma{0};
    std::vector<std::vector<Color>> color;
};

#endif // COLORWHEEL_H
