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

#ifndef CIECAMVIEWER_H
#define CIECAMVIEWER_H

#include <SFML/Graphics.hpp>
#include "colorwheel.h"
#include "colorschemer.h"
#include "pigmentlocator.h"

class CIECAMViewer
{
public:
    CIECAMViewer();
    
    void Run();
private:
    sf::RenderWindow mWindow {sf::VideoMode(1800, 1000), "CIECAM02 Viewer"};

    //SFML graphics
    void drawCircleGrid();
    void drawHexGrid();
    void drawSolidGrid();
    void drawColorSchemer();
    sf::Font mFont;

    //ImGui graphics
    void drawGUI();
    void drawColorWheelControlGUI();
    void drawColorPickerGUI();
    void drawImageAnalyzerGUI();
    void drawText(std::string text);
    
    //Colour wheel stuffs
    GridType mGridType{GridType::Circle};
    WheelType mWheelType{WheelType::JCh};
    ColorWheel mColorWheel;
    Color mBgColor;
    int mGridSelection{0}, mWheelSelection{0};
    bool mIsPigmentShown{true};
    int mSurroundSelection{0};
    int mIlluminantSelection {5};
    int mStandardObserver{2};
    Illuminant mIlluminant{Illuminant::D65};
    PigmentLocator mTempPigment;
    bool  mBindYbToBg {true}, mBindYbToRightBg{false};
    
    //Colour Picker
    Color mColor;
    Color mTempColor;
    int mPickerInputSelection {2};
    int mColourSchemerInputSelection{2};
    float mColorSpanA{0}, mColorSpanB{0}, mColorSpanC{0};
    bool mIsRythmicElementBound{false};
    bool mReversedColorSpanB{false}, mReversedColorSpanC{false};
    
    //Colour schemer
    Color mRightBgColor;
    ColorSchemer mColorScheme;
    void setRandomBoxes();
    void setRandomLongBoxes();
    void setRandomLines();
    void setRandomBoxesWithinBoxes();
    std::vector<sf::RectangleShape> mRandomBox;
    std::vector<sf::RectangleShape> mRandomLongBox;
    std::vector<sf::RectangleShape> mRandomLine;
    std::vector<sf::RectangleShape> mRandomBoxWithinBox;
    
    //Pigment Locator
    PigmentLocator mPigment;
    
    //Image Analyzer
    char mCharFileName[100] {""};
    std::string mFileName {""};
    ColorSchemer mImageColorScheme;
    bool mIsImageGamutShownOnColorWheel {false};
    bool mIsHigherAccuracyImageGamut {false};
    PigmentLocator mImageColor;
};

#endif // CIECAMVIEWER_H
