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

#ifndef VIEWINGCONDITION_H
#define VIEWINGCONDITION_H

#include <iostream> // for debugging purpose

//Ignoring camel-casing for any math variables defined in "Color Appearance Models" by Mark D. Fairchild for better readability. 
//Note case-sensitivity where applicable (subscripts are written as lowercase).
//XYZ, JCh, QMh, and Jab values are Scaled approximately between 0 to (+/-)100, rather than 0 to (+/-)1. h is scaled between 0 to 360.

enum class Surrounding {Average, Dim, Dark};
enum class Illuminant {A, B, C, D50, D55, D65, D75, E, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12};
enum class TransformMatrix {MRGB, InversedMRGB, Mcat02, InversedMcat02, Mhpe, InversedMhpe, Mljg};


class ViewingCondition
{
public:
    ViewingCondition();
    void setCIECAMParam(const float& surroundLuminance = 4.0, const float& backgroundLuminance = 20.0, const Surrounding& surrounding = Surrounding::Average, 
                        const Illuminant& illuminant = Illuminant::D65, const int& standardObserver = 2);
    void setCIECAMSurrounding(const Surrounding& surrounding);
    void setCIECAMSurrounding(const float& exponentC, const float& contrastFactorNc, const float& contrastFactorF);
    void setCIECAMWhitePoint(const Illuminant& illuminant, const int& standardObserver);

    void transformUsingLinearMatrix (float& colorAOut, float& colourBOut, float& colourCOut, TransformMatrix transformMatrix, 
                                     const float& colourAIn, const float& colourBIn, const float& colourCIn);
    
    //Viewing condition for CIECAM02
    float Xw, Yw, Zw;                                                                              //CIE 1931 XYZ_w (Whtie Point)
    float La, Yb;                                                                                  //L_a = Surround Luminance, Y_b = Background Luminance
    float c, Nc, F;                                                                                //F = maximum degree of chromatic adaptation, c = exponential nonlinearity, N_c = chromatic induction factor
    float D, Fl;                                                                                   //D = degree of adaptation, F_L = luminance-level adaptation factor
    float n, Ncb, Nbb, z;                                                                          //n = adjustment for background, N_bb = N_cb = induction factor, z = base exponential nonlinearity
    float Aw;
};

#endif // VIEWINGCONDITION_H
