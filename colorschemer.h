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

#ifndef COLORSCHEMER_H
#define COLORSCHEMER_H

#include "color.h"
#include <vector>

//This program is loosely based on "A Method of Changing a Color Scheme with Kansei Scales" by Hideki Yamazaki and Kunio Kondo(1999)
//  The five kansei scales 
//      1. warm vs cool (method used in this program is different from those presented in the original paper)
//      2. soft vs hard
//      3. natural vs artificial
//      4. bright vs dark
//      5. active vs passive (named gay vs quiet in original paper, which is not as common in Artist's lingos)
//      
//  We calculate these using the more accurate CIECAM02 colourspace instead of the CIELab colourspace used in the paper.
//  The method for determining warm vs cool has also changed: the original was based on hue angle, which would imply that Brunt Umber is warmer than Quinacridone Red, which is simply not true.
//  Here, we determine warmth vs coolness of a colour based on its proportional colour differences from an imaginary orange (JCh: -,120,40) and cyan (JCh: -,120,220)

class ColorSchemer
{
public:
    std::vector<Color> colorSet;

    ColorSchemer(ViewingCondition& vc, int numberOfColor = 5);
    void addColorToColorSet(const Color& color, int position);
    void calculateColorScheme();
    void calculateKanseiScaleRating();
    
    std::string getDescWramth();
    std::string getDescSoftness();
    std::string getDescNaturalness();
    std::string getDescBrightness();
    std::string getDescActiveness();
    
    float getWarmth();
    float getSoftness();
    float getNaturalness();
    float getBrightness();
    float getActiveness();
    
    float mChromaticnessAverage, mLightnessAverage;
    float mLightnessChromaticnessAverage, mLightnessDistributedDegree, mDistanceDistributedDegree;
    float mWarmth, mSoftness, mNaturalness, mBrightness, mActiveness;
    float mSaturationAverage, mSaturationDistributedDegree;                                         //Not part of the Kansei scale paper, but availiable for CIECAM02 and fairly useful
    
private:
    int mNumberOfColor;
};

#endif // COLORSCHEMER_H
