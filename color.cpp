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

#include "color.h"

#include <cmath>
#include <cstdlib>

Color::Color(ViewingCondition& viewingCondition) : vc(viewingCondition)
{
    setColor(0.0, 0.0, 0.0, ColorSpace::sRGB);
}

Color::Color(const Color& color) : vc(color.vc)
{
    setColor(color.J, color.ac, color.bc, ColorSpace::Jab);
}

void Color::setColor(const float& colorAIn, const float& colorBIn, const float& colorCIn, const ColorSpace& colorSpace, const bool& isColorUpdated)
{
    if (colorSpace == ColorSpace::sRGB)
    {
        trueR = colorAIn;
        trueG = colorBIn;
        trueB = colorCIn;
        isInRGBRange = true;
        R = colorAIn;
        G = colorBIn;
        B = colorCIn;
        
    }
    if (colorSpace == ColorSpace::XYZ)
    {
        X = colorAIn;
        Y = colorBIn;
        Z = colorCIn;
    }
    if (colorSpace == ColorSpace::JCh)
    {
        J = colorAIn;
        C = colorBIn;
        h = colorCIn;
    }
    if (colorSpace == ColorSpace::QMh)
    {
        Q = colorAIn;
        M = colorBIn;
        h = colorCIn;
    }
    if (colorSpace == ColorSpace::Jab)
    {
        J = colorAIn;
        ac = colorBIn;
        bc = colorCIn;
    }
    if (colorSpace == ColorSpace::Ljg)
    {
        L = colorAIn;
        j = colorBIn;
        g = colorCIn;
    }
    if (isColorUpdated)
        updateColor(colorSpace);
}

void Color::updateColor(const ColorSpace& colorSpace)
{
    if (colorSpace == ColorSpace::sRGB)
    {
        RGB2XYZ();
        XYZ2JCh();
        JCh2QMh();
        JCh2Jab();
        isInRGBRange = true;
        R = trueR;
        G = trueG;
        B = trueB;

    }
    if (colorSpace == ColorSpace::XYZ)
    {
        XYZ2JCh();
        JCh2QMh();
        JCh2Jab();
        XYZ2RGB();
    }
    if (colorSpace == ColorSpace::JCh)
    {
        JCh2Jab();
        JCh2QMh();
        JCh2XYZ();
        XYZ2RGB();
    }
    if (colorSpace == ColorSpace::QMh)
    {
        QMh2JCh();
        JCh2Jab();
        JCh2XYZ();
        XYZ2RGB();
    }
    if (colorSpace == ColorSpace::Jab)
    {
        Jab2JCh();
        JCh2QMh();
        JCh2XYZ();
        XYZ2RGB();
    }
    if (colorSpace == ColorSpace::Ljg)
    {
        //There are no reverse model for OSA-UCS.
    }
}

void Color::XYZ2RGB()
{
    //linear RGB
    float Rl, Gl, Bl;
    vc.transformUsingLinearMatrix(Rl, Gl, Bl, TransformMatrix::MRGB, X / 100.0, Y / 100.0, Z / 100.0);

    //sRGB
    if (Rl > 0.0031308)
        trueR = 255.0 * (1.055 * pow(Rl, 1.0 / 2.4) - 0.055);
    else
        trueR = 255.0 * 12.92 * Rl;
    if (Gl > 0.0031308)
        trueG = 255.0 * (1.055 * pow(Gl, 1.0 / 2.4) - 0.055);
    else
        trueG = 255.0 * 12.92 * Gl;
    if (Bl > 0.0031308) 
        trueB = 255.0 * (1.055 * pow(Bl, 1.0 / 2.4) - 0.055);
    else
        trueB = 255.0 * 12.92 * Bl;

    //Safe version of sRGB within 0-255 range
    R = trueR;
    G = trueG;
    B = trueB;
    isInRGBRange = true;
    
    if (trueR < 0)
    {
        R = 0;
        isInRGBRange = false;
    }
    if (trueR > 255)
    {
        R = 255;
        isInRGBRange = false;
    }
    if (trueG < 0)
    {
        G = 0;
        isInRGBRange = false;
    }
    if (trueG > 255)
    {
        G = 255;
        isInRGBRange = false;

    }
    if (trueB < 0)
    {
        B = 0;
        isInRGBRange = false;

    }
    if (trueB > 255)
    {
        B = 255;
        isInRGBRange = false;
    }
}

void Color::RGB2XYZ()
{
    //linear RGB
    float Rl, Gl, Bl;    
    
    if (trueR > 0.04045)
        Rl = pow((trueR / 255.0 + 0.055) / 1.055, 2.4) * 100.0;
    else
        Rl = trueR / 12.92 * 100.0 / 255.0;
    if (trueG > 0.04045)
        Gl = pow((trueG / 255.0 + 0.055) / 1.055, 2.4) * 100.0;
    else
        Gl = trueG / 12.92 * 100.0 / 255.0;
    if (trueB > 0.04045)
        Bl = pow((trueB / 255.0 + 0.055) / 1.055, 2.4) * 100.0;
    else
        Bl = trueB / 12.92 * 100.0 / 255.0;
    
    //XYZ
    vc.transformUsingLinearMatrix(X, Y, Z, TransformMatrix::InversedMRGB, Rl, Gl, Bl);
}

void Color::XYZ2JCh()
{
    //CAT02 Adapted RGB tristimulus response (These are not sRGB values!)
    float R, G, B;
    vc.transformUsingLinearMatrix(R, G, B, TransformMatrix::Mcat02, X, Y, Z);
    float Rw, Gw, Bw;
    vc.transformUsingLinearMatrix(Rw, Gw, Bw, TransformMatrix::Mcat02, vc.Xw, vc.Yw, vc.Zw);

    float Rc {static_cast<float>(((vc.Yw * vc.D / Rw) + (1.0 - vc.D)) * R)};
    float Gc {static_cast<float>(((vc.Yw * vc.D / Gw) + (1.0 - vc.D)) * G)};
    float Bc {static_cast<float>(((vc.Yw * vc.D / Bw) + (1.0 - vc.D)) * B)};
    
    //Hunt–Pointer–Estevez (HPE) Post-Adaptation Nonlinear Compression
    float Rpc, Gpc, Bpc;
    vc.transformUsingLinearMatrix(Rpc, Gpc, Bpc, TransformMatrix::InversedMcat02, Rc, Gc, Bc);

    float Rp, Gp, Bp;
    vc.transformUsingLinearMatrix(Rp, Gp, Bp, TransformMatrix::Mhpe, Rpc, Gpc, Bpc);
    
    float Rpa, Gpa, Bpa;
    if (Rp < 0.0)     //needed to prevent nan due to negative roots
        Rpa = 400.0 * -pow(vc.Fl * -Rp / 100.0, 0.42) / (27.13 - pow(vc.Fl * -Rp / 100.0, 0.42)) + 0.1;
    else
        Rpa = 400.0 * pow(vc.Fl * Rp / 100.0, 0.42) / (27.13 + pow(vc.Fl * Rp / 100.0, 0.42)) + 0.1;
    if (Gp < 0.0)
        Gpa = 400.0 * -pow(vc.Fl * -Gp / 100.0, 0.42) / (27.13 - pow(vc.Fl * -Gp / 100.0, 0.42)) + 0.1;
    else
        Gpa = 400.0 * pow(vc.Fl * Gp / 100.0, 0.42) / (27.13 + pow(vc.Fl * Gp / 100.0, 0.42)) + 0.1;
    if (Bp < 0.0)
        Bpa = 400.0 * -pow(vc.Fl * -Bp / 100.0, 0.42) / (27.13 - pow(vc.Fl * -Bp / 100.0, 0.42)) + 0.1;
    else
        Bpa = 400.0 * pow(vc.Fl * Bp / 100.0, 0.42) / (27.13 + pow(vc.Fl * Bp / 100.0, 0.42)) + 0.1;

    //Opponent Color Dimisions (These are the initial a, b value, not the a_c, b_c used in Jab)
    float a {static_cast<float>(Rpa - 12.0 * Gpa / 11.0 + Bpa / 11.0)};
    float b {static_cast<float>(1.0 / 9.0 * (Rpa + Gpa - 2 * Bpa))};
    
    //Hue
    h = atan2(b, a) / 3.14159 * 180.0;
    
    if (h < 0.0)
    {
        h += 360.0;
    }
    
    //Lightness
    float A {static_cast<float>((2.0 * Rpa + Gpa + 1.0 / 20.0 * Bpa - 0.305) * vc.Nbb)};
    
    J = 100 * pow(A / vc.Aw, vc.c * vc.z);
   
    //Chroma
    float et {static_cast<float>(1.0 / 4.0 * (cos(h * 3.14159 / 180.0 + 2.0) + 3.8))};                                          //eccentricity factor
    float t {static_cast<float>(50000.0 / 13.0 * vc.Nc * vc.Ncb * et * sqrt(pow(a, 2.0) + pow(b, 2.0)) / (Rpa + Gpa + 21.0 / 20.0 * Bpa))};     //temporary quanity
   
    C = {static_cast<float>(pow(t, 0.9) * sqrt(J / 100.0) * pow(1.64 - pow(0.29, vc.n), 0.73))};
}

void Color::JCh2QMh()
{
    //Brightness
    Q = 4.0 / vc.c * sqrt(J / 100.0) * (vc.Aw + 4.0) * pow(vc.Fl, 0.25);
    
    //Colorfullness
    M = C * pow(vc.Fl , 0.25);
    
    //Saturation
    s = 100 * sqrt(M / Q);
}

void Color::JCh2Jab()
{
    //Cartesian coordinates
    ac = C * cos(h * 3.14159 / 180.0);
    bc = C * sin(h * 3.14159 / 180.0);
}

void Color::Jab2JCh()
{
    //Hue
    h = atan2(bc, ac) / 3.14159 * 180.0;
    
    if (h < 0.0)
    {
        h += 360.0;
    }
        
    //Chroma
    C = sqrt(pow(ac, 2.0) + pow(bc, 2.0));    
}

void Color::QMh2JCh()
{
    //Saturation
    s = 100 * sqrt(M / Q);
    
    //Lightness
    J  = pow(Q / (4.0 / vc.c *  (vc.Aw + 4.0) * pow(vc.Fl, 0.25)), 2.0) * 100.0;
    
    //Chroma
    C = M / pow(vc.Fl, 0.25);
}


void Color::JCh2XYZ()
{
    float t {static_cast<float>(pow(C / (sqrt(J / 100.0) * pow(1.64 - pow(0.29, vc.n), 0.73)), 1.0 / 0.9))};
    float et {static_cast<float>(1.0 / 4.0 * (cos(h * 3.14159 / 180.0 + 2.0) + 3.8))};
    float A {static_cast<float>(vc.Aw * pow (J / 100.0,  1.0 / (vc.c * vc.z)))};

    float Rw, Gw, Bw;
    vc.transformUsingLinearMatrix(Rw, Gw, Bw, TransformMatrix::Mcat02, vc.Xw, vc.Yw, vc.Zw);
    
    float a,b;
    
    if (std::abs(sin(h * 3.14159 / 180.0)) >= std::abs(cos(h * 3.14159 / 180.0)))       //prevents div/0 error when angle is at 90 or 270
    {
        b = ((A / vc.Nbb + 0.305) * sin(h * 3.14159 / 180.0)) / (((50000.0 * vc.Nc * vc.Ncb * et) / (13.0 * t)) + (11.0 / 23.0) * cos(h * 3.14159 / 180.0) + (108.0 / 23.0) * sin(h * 3.14159 / 180.0));
        a = b * cos(h * 3.14159 / 180.0) / sin (h * 3.14159 / 180.0);
    }
    else 
    {
        a = ((A / vc.Nbb + 0.305) * cos(h * 3.14159 / 180.0)) / (((50000.0 * vc.Nc * vc.Ncb * et) / (13.0 * t)) + (11.0 / 23.0 * cos(h * 3.14159 / 180.0)) + (108.0 / 23.0 * sin(h * 3.14159 / 180.0)));
        b = a * tan(h * 3.14159 / 180.0);
    }
  
    float Rpa {static_cast<float>((20.0 / 61.0) * (A / vc.Nbb + 0.305) + (451.0 / 1403.0) * a + (288.0 / 1403.0) * b)};
    float Gpa {static_cast<float>((20.0 / 61.0) * (A / vc.Nbb + 0.305) - (891.0 / 1403.0) * a - (261.0 / 1403.0) * b)};
    float Bpa {static_cast<float>((20.0 / 61.0) * (A / vc.Nbb + 0.305) - (220.0 / 1403.0) * a - (6300.0 / 1403.0) * b)};
    
    float Rp, Gp, Bp;
    
    if (((Rpa - 0.1) * 27.13) / (400.0 - (Rpa - 0.1)) < 0)   //needed to prevent nan due to negative roots
        Rp = (100.0 / vc.Fl) * -pow (-((Rpa - 0.1) * 27.13) / (400.0 - (Rpa - 0.1)), 1.0 / 0.42);
    else
        Rp = (100.0 / vc.Fl) * pow (((Rpa - 0.1) * 27.13) / (400.0 - (Rpa - 0.1)), 1.0 / 0.42);
    if (((Gpa - 0.1) * 27.13) / (400.0 - (Gpa - 0.1)) < 0)
        Gp = (100.0 / vc.Fl) * -pow (-((Gpa - 0.1) * 27.13) / (400.0 - (Gpa - 0.1)), 1.0 / 0.42);
    else
        Gp = (100.0 / vc.Fl) * pow (((Gpa - 0.1) * 27.13) / (400.0 - (Gpa - 0.1)), 1.0 / 0.42);
    if (((Bpa - 0.1) * 27.13) / (400.0 - (Bpa - 0.1)) < 0)
        Bp = (100.0 / vc.Fl) * -pow (-((Bpa - 0.1) * 27.13) / (400.0 - (Bpa - 0.1)), 1.0 / 0.42);
    else
        Bp = (100.0 / vc.Fl) * pow (((Bpa - 0.1) * 27.13) / (400.0 - (Bpa - 0.1)), 1.0 / 0.42);
    
    float Rpc, Gpc, Bpc;
    vc.transformUsingLinearMatrix(Rpc, Gpc, Bpc, TransformMatrix::InversedMhpe, Rp, Gp, Bp);
    
    float Rc, Gc, Bc;
    vc.transformUsingLinearMatrix(Rc, Gc, Bc, TransformMatrix::Mcat02, Rpc, Gpc, Bpc);

    float R {Rc / ((vc.Yw * vc.D / Rw) + (1 - vc.D))};
    float G {Gc / ((vc.Yw * vc.D / Gw) + (1 - vc.D))};
    float B {Bc / ((vc.Yw * vc.D / Bw) + (1 - vc.D))};
    
    vc.transformUsingLinearMatrix(X, Y, Z, TransformMatrix::InversedMcat02, R, G, B);
}

float Color::findDistanceFromColor(const float& inputJ, const float& inputa, const float& inputb)
{
    return sqrt(pow(J - inputJ, 2) + pow(ac - inputa, 2.0) + pow(bc - inputb, 2.0));
}
