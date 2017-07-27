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

#include "viewingcondition.h"

#include <cmath>
#include <cstdlib>

#include <iostream>

ViewingCondition::ViewingCondition()
{
    setCIECAMParam();
}

void ViewingCondition::setCIECAMParam(const float& surroundLuminance, const float& backgroundLuminance, const Surrounding& surrounding, const Illuminant& illuminant, const int& standardObserver)
{
    La = surroundLuminance;
    Yb = backgroundLuminance;

    setCIECAMWhitePoint(illuminant, standardObserver);
    setCIECAMSurrounding(surrounding);
}

void ViewingCondition::setCIECAMSurrounding(const Surrounding& surrounding)
{
    // Table 16.1 Input parameters for the CIECAM02 model (Mark D. Fairchild, "Color Appearance Models", 2005)
    //     Viewing condition:      c           Nc          F
    //     Average surround:       0.69        1.0         1.0
    //     Dim surround:           0.59        0.9         0.9
    //     Dark surround:          0.525       0.8         0.8

    if (surrounding == Surrounding::Average)
    {
            setCIECAMSurrounding(0.69, 1.0, 1.0);
    }
    else if (surrounding == Surrounding::Dim)
    {
            setCIECAMSurrounding(0.59, 0.9, 0.9);
    }
    else if (surrounding == Surrounding::Dark)
    {
            setCIECAMSurrounding(0.525, 0.8, 0.8);
    }
}


void ViewingCondition::setCIECAMSurrounding(const float& exponentC, const float& contrastFactorNc, const float& contrastFactorF)
{
    //viewing-condition-dependent components 
    c = exponentC;
    Nc = contrastFactorNc;
    F = contrastFactorF;
    float k {(static_cast<float>(1.0 / (5.0 * La + 1.0)))};
    D = F * (1.0 - (1.0 / 3.6) * pow(2.71828, -(La + 42.0) / 92.0));
    Fl = 0.2 * pow(k, 4.0) * (5.0 * La) + 0.1 * pow((1.0 - pow(k,4.0)), 2.0) * pow(5.0 * La, 1.0 / 3.0);
    n = Yb / Yw;
    Ncb = 0.725 * pow(1.0 / n, 0.2);
    Nbb = Ncb;
    z = 1.48 + sqrt(n);

    //CAT02 Adapted RGB tristimulus response for white point
    float Rw, Gw, Bw;    
    transformUsingLinearMatrix(Rw, Gw, Bw,TransformMatrix::Mcat02, Xw, Yw, Zw);
    
    float Rwc {((Yw * D / Rw) + (1 - D)) * Rw};
    float Gwc {((Yw * D / Gw) + (1 - D)) * Gw};
    float Bwc {((Yw * D / Bw) + (1 - D)) * Bw};
   
    //Hunt–Pointer–Estevez (HPE) Post-Adaptation Nonlinear Compression
    float Rwpc, Gwpc, Bwpc;
    transformUsingLinearMatrix(Rwpc, Gwpc, Bwpc, TransformMatrix::InversedMcat02, Rwc, Gwc, Bwc);

    float Rwp, Gwp, Bwp;
    transformUsingLinearMatrix(Rwp, Gwp, Bwp, TransformMatrix::Mhpe, Rwpc, Bwpc, Gwpc);
    
    float Rwpa {static_cast<float>(400.0 * pow(Fl * Rwp / 100.0, 0.42) / (27.13 + pow(Fl * Rwp / 100.0, 0.42)) + 0.1)};
    float Gwpa {static_cast<float>(400.0 * pow(Fl * Gwp / 100.0, 0.42) / (27.13 + pow(Fl * Gwp / 100.0, 0.42)) + 0.1)};
    float Bwpa {static_cast<float>(400.0 * pow(Fl * Bwp / 100.0, 0.42) / (27.13 + pow(Fl * Bwp / 100.0, 0.42)) + 0.1)};

    Aw = (2.0 * Rwpa + Gwpa + 1.0 / 20.0 * Bwpa - 0.305) * Nbb;
}

void ViewingCondition::setCIECAMWhitePoint(const Illuminant& illuminant, const int& standardObserver)
{
    float x;
    float y;
    
    if (standardObserver == 10)
    {
        if (illuminant == Illuminant::A)
        {
            x = 0.45117;
            y = 0.40594;
        }
        if (illuminant == Illuminant::B)
        {
            x = 0.3498;
            y = 0.3527;
        }
        if (illuminant == Illuminant::C)
        {
            x = 0.31039;
            y = 0.31905;
        }
        if (illuminant == Illuminant::D50)
        {
            x = 0.34773;
            y = 0.35952;
        }
        if (illuminant == Illuminant::D55)
        {
            x = 0.33411;
            y = 0.34877;
        }
        if (illuminant == Illuminant::D65)
        {
            x = 0.31382;
            y = 0.331;
        }
        if (illuminant == Illuminant::D75)
        {
            x = 0.29968;
            y = 0.3174;
        }
        if (illuminant == Illuminant::E)
        {
            x = 1.0 / 3.0;
            y = 1.0 / 3.0;
        }
        if (illuminant == Illuminant::F1)
        {
            x = 0.31811;
            y = 0.33559;
        }
        if (illuminant == Illuminant::F2)
        {
            x = 0.37925;
            y = 0.36733;
        }
        if (illuminant == Illuminant::F3)
        {
            x = 0.41761;
            y = 0.38324;
        }
        if (illuminant == Illuminant::F4)
        {
            x = 0.4492;
            y = 0.39074;
        }
        if (illuminant == Illuminant::F5)
        {
            x = 0.31975;
            y = 0.34246;
        }
        if (illuminant == Illuminant::F6)
        {
            x = 0.3866;
            y = 0.37847;
        }
        if (illuminant == Illuminant::F7)
        {
            x = 0.31569;
            y = 0.3296;
        }
        if (illuminant == Illuminant::F8)
        {
            x = 0.34902;
            y = 0.35939;
        }
        if (illuminant == Illuminant::F9)
        {
            x = 0.37829;
            y = 0.37045;
        }
        if (illuminant == Illuminant::F10)
        {
            x = 0.3509;
            y = 0.35444;
        }
        if (illuminant == Illuminant::F11)
        {
            x = 0.38541;
            y = 0.37123;
        }
        if (illuminant == Illuminant::F12)
        {
            x = 0.44256;
            y = 0.39717;
        }
    }
    else
    {
        if (illuminant == Illuminant::A)
        {
            x = 0.44757;
            y = 0.40745;
        }
        if (illuminant == Illuminant::B)
        {
            x = 0.34842;
            y = 0.35161;
        }
        if (illuminant == Illuminant::C)
        {
            x = 0.31006;
            y = 0.31616;
        }
        if (illuminant == Illuminant::D50)
        {
            x = 0.34567;
            y = 0.3585;
        }
        if (illuminant == Illuminant::D55)
        {
            x = 0.33242;
            y = 0.34743;
        }
        if (illuminant == Illuminant::D65)
        {
            x = 0.31271;
            y = 0.32902;
        }
        if (illuminant == Illuminant::D75)
        {
            x = 0.29902;
            y = 0.31485;
        }
        if (illuminant == Illuminant::E)
        {
            x = 1 / 3;
            y = 1 / 3;
        }
        if (illuminant == Illuminant::F1)
        {
            x = 0.3131;
            y = 0.33727;
        }
        if (illuminant == Illuminant::F2)
        {
            x = 0.37208;
            y = 0.37529;
        }
        if (illuminant == Illuminant::F3)
        {
            x = 0.4091;
            y = 0.3943;
        }
        if (illuminant == Illuminant::F4)
        {
            x = 0.44018;
            y = 0.40329;
        }
        if (illuminant == Illuminant::F5)
        {
            x = 0.31379;
            y = 0.34531;
        }
        if (illuminant == Illuminant::F6)
        {
            x = 0.3779;
            y = 0.38835;
        }
        if (illuminant == Illuminant::F7)
        {
            x = 0.31292;
            y = 0.32933;
        }
        if (illuminant == Illuminant::F8)
        {
            x = 0.34588;
            y = 0.35875;
        }
        if (illuminant == Illuminant::F9)
        {
            x = 0.37417;
            y = 0.37281;
        }
        if (illuminant == Illuminant::F10)
        {
            x = 0.34609;
            y = 0.35986;
        }
        if (illuminant == Illuminant::F11)
        {
            x = 0.38052;
            y = 0.37713;
        }
        if (illuminant == Illuminant::F12)
        {
            x = 0.43695;
            y = 0.40441;
        }
    }
    
    Yw = 100.0;
    Xw = Yw * x / y;
    Zw = Yw * (1 - x - y) / y;
}

void ViewingCondition::transformUsingLinearMatrix(float& colourAOut, float& colourBOut, float& colourCOut, TransformMatrix transformMatrix, 
                                                  const float& colourAIn, const float& colourBIn, const float& colourCIn)
{
    if (transformMatrix == TransformMatrix::MRGB)
    {
        colourAOut = colourAIn *  3.2406 + colourBIn * -1.5372 + colourCIn * -0.4986;
        colourBOut = colourAIn * -0.9689 + colourBIn *  1.8758 + colourCIn *  0.0415;
        colourCOut = colourAIn *  0.0557 + colourBIn * -0.2040 + colourCIn *  1.0570;
    }
    else if (transformMatrix == TransformMatrix::InversedMRGB)
    {
        colourAOut = colourAIn * 0.4124 + colourBIn * 0.3576 + colourCIn * 0.1805;
        colourBOut = colourAIn * 0.2126 + colourBIn * 0.7152 + colourCIn * 0.0722;
        colourCOut = colourAIn * 0.0193 + colourBIn * 0.1192 + colourCIn * 0.9505;
    }
    else if (transformMatrix == TransformMatrix::Mcat02)
    {
        colourAOut = colourAIn *  0.7328 + colourBIn * 0.4296 + colourCIn * -0.1624;
        colourBOut = colourAIn * -0.7036 + colourBIn * 1.6975 + colourCIn *  0.0061;
        colourCOut = colourAIn *  0.0030 + colourBIn * 0.0136 + colourCIn *  0.9834;
    }
    else if (transformMatrix == TransformMatrix::InversedMcat02)
    {
        colourAOut = colourAIn *  1.096124 + colourBIn * -0.278869 + colourCIn * 0.182745;
        colourBOut = colourAIn *  0.454369 + colourBIn *  0.473533 + colourCIn * 0.072098;
        colourCOut = colourAIn * -0.009628 + colourBIn * -0.005698 + colourCIn * 1.015326;
    }

    else if (transformMatrix == TransformMatrix::Mhpe)
    {
        colourAOut = colourAIn *  0.38971 + colourBIn * 0.68898 + colourCIn * -0.07868;
        colourBOut = colourAIn * -0.22981 + colourBIn * 1.18340 + colourCIn *  0.04641;
        colourCOut = colourAIn *  0.00000 + colourBIn * 0.00000 + colourCIn *  1.00000;
    }
    else if (transformMatrix == TransformMatrix::InversedMhpe)
    {
        colourAOut = colourAIn * 1.9102 + colourBIn * -1.1121 + colourCIn * 0.2019;
        colourBOut = colourAIn * 0.3710 + colourBIn *  0.6291 + colourCIn * 0.00;
        colourCOut = colourAIn * 0.00   + colourBIn *  0.00   + colourCIn * 1.00;
    }
    else if (transformMatrix == TransformMatrix::Mljg)
    {
        colourAOut = colourAIn *  0.7990 + colourBIn * 0.4194 + colourCIn * -0.1648;
        colourBOut = colourAIn * -0.4493 + colourBIn * 1.3265 + colourCIn *  0.0927;
        colourCOut = colourAIn * -0.1149 + colourBIn * 0.3394 + colourCIn *  0.7170;
    }
}
