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

#include "colorschemer.h"

#include <cmath>
#include <cstdlib>

#include <iostream>

ColorSchemer::ColorSchemer(ViewingCondition& vc, int numberOfColor) : colorSet (numberOfColor, vc)
{
    mNumberOfColor = numberOfColor;
    calculateColorScheme();
}

void ColorSchemer::addColorToColorSet(const Color& color, int position)
{
    colorSet.at(position).setColor(color.J, color.ac, color.bc, ColorSpace::Jab);
}

void ColorSchemer::calculateColorScheme()
{
    calculateKanseiScaleRating();
}


void ColorSchemer::calculateKanseiScaleRating()
{
    //Calculating common characterists + warmth Kansei scale (which only have 1 correlation factor)
    float deltaOrangeTotal {0};
    float deltaCyanTotal {0};
    float chromaTotal {0};
    float lightnessTotal {0};
    float totalJ{0};
    float totala{0};
    float totalb{0};
    float totals{0};
    
    for (Color &color : colorSet)
    {
        deltaOrangeTotal = deltaOrangeTotal + sqrt(pow(color.ac - 92.0, 2.0) + pow(color.bc - 77.0, 2.0));
        deltaCyanTotal = deltaCyanTotal + sqrt(pow(color.ac + 92.0, 2.0) + pow(color.bc + 77.0, 2.0));
        chromaTotal = chromaTotal + color.C;
        lightnessTotal = lightnessTotal + color.J;
        totalJ = totalJ + color.J;
        totala = totala + color.ac;
        totalb = totalb + color.bc;
        totals = totals + color.s;
    }

    mWarmth = (deltaCyanTotal - deltaOrangeTotal) / 2.4 / mNumberOfColor;
    mChromaticnessAverage = chromaTotal / mNumberOfColor;
    mLightnessAverage = lightnessTotal / mNumberOfColor;
    mLightnessChromaticnessAverage = sqrt(mChromaticnessAverage * mLightnessAverage);
    float averageJ = totalJ / mNumberOfColor;
    float averagea = totala / mNumberOfColor;
    float averageb = totalb / mNumberOfColor;
    mSaturationAverage = totals / mNumberOfColor;
    
    mLightnessDistributedDegree = 0;
    mDistanceDistributedDegree = 0;
    for (Color &color : colorSet)
    {
        mLightnessDistributedDegree = mLightnessDistributedDegree +  std::abs(color.J - mLightnessAverage);
        mDistanceDistributedDegree = mDistanceDistributedDegree + sqrt(pow(color.J - averageJ, 2.0) + pow(color.ac - averagea, 2.0) + pow(color.bc - averageb, 2.0));
        mSaturationDistributedDegree = mSaturationAverage + std::abs(color.s - mSaturationAverage);
    }
    mLightnessDistributedDegree = mLightnessDistributedDegree / mNumberOfColor;
    mDistanceDistributedDegree = mDistanceDistributedDegree / mNumberOfColor;
    
    //Calculating the 4 other Kansei scale
    float totalAbsoluteCorrelation;
    totalAbsoluteCorrelation = 0.813 + 0.590 + 0.791;
    mSoftness = (0.813 * (mLightnessAverage - 50) + 0.590 * (mLightnessChromaticnessAverage - 50) - 0.791 * (mLightnessDistributedDegree - 25)) / totalAbsoluteCorrelation;
    
    totalAbsoluteCorrelation = 0.620 + 0.745 + 0.724;
    mNaturalness = (0.620 * (mLightnessAverage - 50) - 0.745 * (mLightnessDistributedDegree - 25) - 0.724 * (mDistanceDistributedDegree - 50)) / totalAbsoluteCorrelation;
    
    totalAbsoluteCorrelation = 0.970 + 0.916 + 0.654;
    mBrightness = (0.970 * (mLightnessAverage - 50) + 0.916 * (mLightnessChromaticnessAverage - 50) - 0.645 * (mLightnessDistributedDegree - 25)) / totalAbsoluteCorrelation;
    
    totalAbsoluteCorrelation = 0.796 + 0.909 + 0.566;
    mActiveness = (0.796 * (mChromaticnessAverage - 50) + 0.909 * (mLightnessChromaticnessAverage - 50) + 0.566 * (mDistanceDistributedDegree - 50)) / totalAbsoluteCorrelation;

    mSoftness = mSoftness / 1.25;
    mNaturalness = mNaturalness / 1.25;
    mBrightness = mBrightness / 1.25;
    mActiveness = mActiveness / 1.5;
}

std::string ColorSchemer::getDescWramth()
{
    if (mWarmth < -50)
        return "extremely cool";
    if (mWarmth < -20)
        return "very cool";
    if (mWarmth < -10)
        return "cool";
    if (mWarmth < -2)
        return "slightly cool";

    if (mWarmth > 50)
        return "extremely wam";
    if (mWarmth > 20)
        return "very warm";
    if (mWarmth > 10)
        return "warm";
    if (mWarmth > 2)
        return "slightly warm";
    
    return "neither warm or cool";
}

std::string ColorSchemer::getDescSoftness()
{


    if (mSoftness < -50)
        return "extremely hard";
    if (mSoftness < -20)
        return "very hard";
    if (mSoftness < -10)
        return "hard";
    if (mSoftness < -2)
        return "slightly hard";
    
    if (mSoftness > 50)
        return "extremely soft";
    if (mSoftness > 20)
        return "very soft";
    if (mSoftness > 10)
        return "soft";
    if (mSoftness > 2)
        return "slightly soft";
    
    return "neither soft or hard";
}

std::string ColorSchemer::getDescNaturalness()
{
    if (mNaturalness < -50)
        return "extremely artificial";
    if (mNaturalness < -20)
        return "very artificial";
    if (mNaturalness < -10)
        return "artificial";
    if (mNaturalness < -2)
        return "slightly artificial";
    
    if (mNaturalness > 50)
        return "extremely natural";
    if (mNaturalness > 20)
        return "very natural";
    if (mNaturalness > 10)
        return "natural";
    if (mNaturalness > 2)
        return "slightly natural";
    
    return "neither natural or artificial";
}


std::string ColorSchemer::getDescBrightness()
{
    if (mBrightness < -50)
        return "extremely dark";
    if (mBrightness < -20)
        return "very dark";
    if (mBrightness < -10)
        return "dark";
    if (mBrightness < -2)
        return "dark";
    
    if (mBrightness > 50)
        return "extremely bright";
    if (mBrightness > 20)
        return "very bright";
    if (mBrightness > 10)
        return "bright";
    if (mBrightness > 2)
        return "slightly bright";
    
    return "neither bright or dark";
}

std::string ColorSchemer::getDescActiveness()
{
    if (mActiveness < -50)
        return "extremely passive";
    if (mActiveness < -20)
        return "very passive";
    if (mActiveness < -10)
        return "passive";
    if (mActiveness < -2)
        return "slightly passive";

    if (mActiveness > 50)
        return "extremely active";
    if (mActiveness > 20)
        return "very active";
    if (mActiveness > 10)
        return "active";
    if (mActiveness > 2)
        return "slightly active";

    return "neither active or passive";
}

float ColorSchemer::getWarmth()
{
    return mWarmth;
}

float ColorSchemer::getSoftness()
{
    return mSoftness;
}

float ColorSchemer::getNaturalness()
{
    return mNaturalness;
}

float ColorSchemer::getBrightness()
{
    return mBrightness;
}

float ColorSchemer::getActiveness()
{
    return mActiveness;
}


