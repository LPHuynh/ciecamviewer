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

#include "colorwheel.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

ColorWheel::ColorWheel() : color (241, std::vector<Color>(241, vc))
{
    update(GridType::Circle, WheelType::JCh);
}


void ColorWheel::update(const GridType& gridType, const WheelType& wheelType)
{
    int gridXResolution{10};
    int gridYResolution{10};
    int startingY{0};
    
    if (gridType == GridType::Hex)
    {
        gridXResolution = 5;
        gridYResolution = 9;
        startingY = 1;
    }
    else if(gridType == GridType::Solid)
    {
        gridXResolution = 1;
        gridYResolution = 1;
    }
    
    
    
    if (wheelType == WheelType::JCh)
    {
        float J, a, b;
        
        for (int x = 0; x < 241; x = x + gridXResolution)
        {
            for (int y = 20 + startingY; y < 220; y = y + gridYResolution)
            {
                a = x - 120.0;
                b = - (y - 120.0) * cos(tiltDegree * 3.14159 / 180.0) + chroma;
                J = - (y - 120.0) * cos((90 - tiltDegree) * 3.14159 / 180.0) + lightness;
                if (J < 0.0)                                                                            //sRGB space gets wonky when J falls below 0; we need J above 100 since titanium white to too near this limit
                    J = 0.0;
                
                color.at(y).at(x).setColor(J, a, b, ColorSpace::Jab, false);
                color.at(y).at(x).Jab2JCh();
                
                color.at(y).at(x).h += rotateDegree;
                
                if (color.at(y).at(x).h > 360.0)
                    color.at(y).at(x).h -= 360.0;
                if (color.at(y).at(x).h < 0.0)
                    color.at(y).at(x).h += 360.0;
                
                color.at(y).at(x).updateColor(ColorSpace::JCh);
            }
        }
    }
    else if (wheelType == WheelType::Jsh) 
    {
        
        float J, Q, M, h, s;
        
        for (int x = 0; x < 241; x = x + gridXResolution)
        {
            for (int y = 20 + startingY; y < 220; y = y + gridYResolution)
            {
                J = sqrt( pow (x - 120, 2) + pow (y - 120 + lightness,2));
                
                Q = 4.0 / vc.c * sqrt((J) / 100.0) * (vc.Aw + 4.0) * pow(vc.Fl, 0.25);
                s = - (y - 120.0) * cos((90 - tiltDegree) * 3.14159 / 180.0) + chroma;
                
                h = atan2(-y + 120 - lightness, x - 120) / 3.14159 * 180.0;
                
                if (h < 0.0)
                {
                    h += 360.0;
                }
                if (h >= 360.0)
                {
                    h -= 360.0;
                }            
                
                M = pow(s / 100, 2) * Q;
                
                color.at(y).at(x).setColor(Q, M, h, ColorSpace::QMh);
                
                color.at(y).at(x).h += rotateDegree;
                
                if (color.at(y).at(x).h > 360.0)
                    color.at(y).at(x).h -= 360.0;
                if (color.at(y).at(x).h < 0.0)
                    color.at(y).at(x).h += 360.0;
                
                color.at(y).at(x).updateColor(ColorSpace::JCh);
            }
        }
    }
}
