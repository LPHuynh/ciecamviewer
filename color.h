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

#ifndef COLOR_H
#define COLOR_H

//Ignoring camel-casing for any math variables defined in "Color Appearance Models" by Mark D. Fairchild for better readability. 
//Note case-sensitivity where applicable (subscripts are written as lowercase).
//XYZ, JCh, QMh, and Jab values are Scaled approximately between 0 to (+/-)100, rather than 0 to (+/-)1. h is scaled between 0 to 360.

#include <vector>

#include "viewingcondition.h"

enum class ColorSpace {sRGB, XYZ, JCh, QMh, Jab, Ljg};

class Color
{
public:
    Color();
    Color(ViewingCondition& viewingCondition);
    Color(const Color& color);

    //Note: There are no easy reverse models for OSA-UCS; Therefore conversion to other colorspace will not occur when setColor is triggered with ColorSpace::Ljg
    void setColor(const float& colorAIn, const float& colorBIn, const float& colorCIn, const ColorSpace& colorSpace = ColorSpace::Jab, const bool& isColorUpdated = true);
    void updateColor(const ColorSpace& colorSpace = ColorSpace::Jab);
    
    float trueR, trueG, trueB;                                                                      //sRGB
    float R, G, B;                                                                                  //sRGB (bound to 0-255)    
    float X, Y, Z;                                                                                  //CIE 1931 XYZ
    float J, C, h, ac, bc, Q, M, s;                                                                 //CIECAM02 JCh, Jab, and QMh
    float L, j, g;                                                                                  //OSA-UCS Ljg

    //sRGB Forward/Reverse Model
    void RGB2XYZ();
    void XYZ2RGB();
    
    //CIECAM02 Forward Model
    void XYZ2JCh();
    void JCh2QMh();
    void JCh2Jab();

    //CIECAM02 Reverse Model
    void Jab2JCh();
    void QMh2JCh();
    void JCh2XYZ();
    
    ViewingCondition& vc;
    bool isInRGBRange;
    
    //other useful functions
    float findDistanceFromColor(const float& inputJ, const float& inputa, const float& inputb);
};

#endif // COLOR_H
