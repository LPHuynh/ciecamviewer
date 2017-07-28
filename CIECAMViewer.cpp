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

#include "ciecamviewer.h"

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include <iostream>
#include <experimental/filesystem>

CIECAMViewer::CIECAMViewer() : mColorWheel(), mBgColor(mColorWheel.vc), mColor(mColorWheel.vc), mTempColor(mColorWheel.vc), mRightBgColor(mColorWheel.vc), 
mColorScheme(mColorWheel.vc), mTempPigment(mColorWheel.vc), mPigment(mColorWheel.vc), mImageColorScheme(mColorWheel.vc, 100 * 100), mImageColor(mColorWheel.vc)
{
    mBgColor.setColor(30, 0, 0, ColorSpace::Jab);
    mRightBgColor.setColor(20, 0, 0, ColorSpace::Jab);
    
    if (!mFont.loadFromFile("Cousine-Regular.ttf"))
        std::cout << "cannot load font";
    
    
    for (int i = 0; i < 5; i++)
    {
        mRandomBox.push_back(sf::RectangleShape());
        mRandomLongBox.push_back(sf::RectangleShape());
        mRandomBoxWithinBox.push_back(sf::RectangleShape());
    }
    
    for (int i = 0; i < 7; i++)
    {
        mRandomLine.push_back(sf::RectangleShape());
    }
    
    mPigment.loadDataFromFile("pigment.json");
    mTempPigment.loadDataFromFile("pigment.json");

    Pigment pigment;                       //Pretend pixels from image = pigment
    pigment.J = 0;
    pigment.a = 0;
    pigment.b = 0;

	if (sf::VideoMode::getDesktopMode().height < 1000)
	{
		mWindow.setSize(sf::Vector2u(1080, 600));
	}
}


void CIECAMViewer::Run()
{
    mWindow.setFramerateLimit(60);
    mWindow.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(mWindow);
    
    sf::Clock deltaClock;
    
    while (mWindow.isOpen())
    {
        sf::Event event;
        while (mWindow.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            
            if (event.type == sf::Event::Closed) 
                mWindow.close();
            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !ImGui::IsMouseHoveringAnyWindow())
            {
                sf::Vector2i localPosition = sf::Mouse::getPosition(mWindow);
                
                if (localPosition.x < 1240)
                {
                    if (mGridType == GridType::Circle)
                    {
                        int gridX = localPosition.x / 50;
                        int gridY = localPosition.y / 50;
                        
                        mColor.setColor(mColorWheel.color.at(gridY * 10 + 20).at(gridX * 10).J, mColorWheel.color.at(gridY * 10 + 20).at(gridX * 10).C, 
                                        mColorWheel.color.at(gridY * 10 + 20).at(gridX * 10).h, ColorSpace::JCh);
                        mPigment.findPigment(mColor);
                    }
                    else if (mGridType == GridType::Hex)
                    {
                        int gridX = localPosition.x / 50;
                        int gridY = localPosition.y / 45;
                        
                        if (gridY % 2 == 0)
                        {
                            if (localPosition.x % 50 > 25)
                            {
                                mColor.setColor(mColorWheel.color.at(gridY * 9 + 21).at(gridX * 10 + 5).J, mColorWheel.color.at((gridY) * 9 + 21).at(gridX * 10 + 5).C, 
                                                mColorWheel.color.at(gridY * 9 + 21).at(gridX * 10 + 5).h, ColorSpace::JCh);
                            }
                            else
                            {
                                mColor.setColor(mColorWheel.color.at(gridY * 9 + 21).at(gridX * 10 - 5).J, mColorWheel.color.at((gridY) * 9 + 21).at(gridX * 10 - 5).C, 
                                                mColorWheel.color.at(gridY * 9 + 21).at(gridX * 10 - 5).h, ColorSpace::JCh);
                            }
                            
                        }
                        else 
                        {
                            mColor.setColor(mColorWheel.color.at(gridY * 9 + 21).at(gridX * 10).J, mColorWheel.color.at((gridY) * 9 + 21).at(gridX * 10).C, 
                                            mColorWheel.color.at(gridY * 9 + 21).at(gridX * 10).h, ColorSpace::JCh);
                        }
                        
                        mPigment.findPigment(mColor);
                    }
                    else if  (mGridType == GridType::Solid)
                    {
                        int gridX = localPosition.x / 5;
                        int gridY = localPosition.y / 5;
                        mColor.setColor(mColorWheel.color.at((gridY + 20)).at(gridX).J, mColorWheel.color.at((gridY + 20)).at(gridX).C, 
                                        mColorWheel.color.at((gridY + 20)).at(gridX).h, ColorSpace::JCh);
                        mPigment.findPigment(mColor);
                    }
                }
                else if (localPosition.x > 1250 && localPosition.y > 30 && localPosition.y < 140)
                {
                    for (int i = 0; i < 5; i++)
                    {
                        if (localPosition.x > 1250 + i * 110 && localPosition.x < 1350 + i * 110)
                            mColor.setColor(mColorScheme.colorSet.at(i).J, mColorScheme.colorSet.at(i).C, mColorScheme.colorSet.at(i).h, ColorSpace::JCh);
                        mPigment.findPigment(mColor);
                    }
                }
                
            }
        }
        ImGui::SFML::Update(mWindow, deltaClock.restart());
        
        drawGUI();
        
        if (mGridType == GridType::Circle)
            drawCircleGrid();
        if (mGridType == GridType::Hex)
            drawHexGrid();
        else if (mGridType == GridType::Solid)
            drawSolidGrid();
        
        drawColorSchemer();
        
        ImGui::Render();
        mWindow.display();
        mWindow.clear(sf::Color(mBgColor.R, mBgColor.G, mBgColor.B));
    }
    
    ImGui::SFML::Shutdown();
}

void CIECAMViewer::drawGUI()
{
    drawColorWheelControlGUI();
    drawColorPickerGUI();
    drawImageAnalyzerGUI();
}

void CIECAMViewer::drawColorWheelControlGUI()
{
    ImGui::Begin("Colour Wheel Control");
    if(ImGui::Combo("Wheel Type", &mWheelSelection, "CIECAM02 JCh/Jab\0CIECAM02 sJh\0\0"))
    {
        if (mWheelSelection == 0)
        {
            mWheelType = WheelType::JCh;
            mColorWheel.lightness = 50;
            mColorWheel.chroma = 0;
            mColorWheel.tiltDegree = 0;
            mColorWheel.rotateDegree = 0;
            mColorWheel.update(mGridType, mWheelType);
        }
        else
        {
            mWheelType = WheelType::Jsh;
            mColorWheel.lightness = 0;
            mColorWheel.chroma = 50;
            mColorWheel.tiltDegree = 0;
            mColorWheel.rotateDegree = 0;
            mColorWheel.update(mGridType, mWheelType);
        }
    }    
    if(ImGui::Combo("Grid Type", &mGridSelection, "Circle Grid\0Hexagon Grid\0Solid Grid\0\0"))
    {
        if (mGridSelection == 0)
        {
            mGridType = GridType::Circle;
            mColorWheel.update(mGridType, mWheelType);
        }
        else if (mGridSelection == 1)
        {
            mGridType = GridType::Hex;
            mColorWheel.update(mGridType, mWheelType);
        }
        else
        {
            mGridType = GridType::Solid;
            mColorWheel.update(mGridType, mWheelType);
        }
    }
    
    if (mWheelType == WheelType::JCh)
    {
        if(ImGui::SliderInt("Lightness", &mColorWheel.lightness, 0, 100))
            mColorWheel.update(mGridType, mWheelType);
        if(ImGui::SliderInt("Chroma", &mColorWheel.chroma, -100, 100))
            mColorWheel.update(mGridType, mWheelType);
    }
    else if (mWheelType == WheelType::Jsh)
    {
        if(ImGui::SliderInt("Lightness", &mColorWheel.lightness, 0, 100))
            mColorWheel.update(mGridType, mWheelType);
        if(ImGui::SliderInt("Saturation", &mColorWheel.chroma, 0, 100))
            mColorWheel.update(mGridType, mWheelType);
    }
    
    if(ImGui::SliderInt("Tilt Degree", &mColorWheel.tiltDegree, -90, 90))
        mColorWheel.update(mGridType, mWheelType);
    if(ImGui::SliderInt("Rotation", &mColorWheel.rotateDegree, -180, 180))
        mColorWheel.update(mGridType, mWheelType);
    ImGui::Checkbox("Show Pigment", &mIsPigmentShown);
    
    if (ImGui::CollapsingHeader("Viewing Condition"))
    {
        if(ImGui::Combo("Surrounding", &mSurroundSelection, "Average\0Dim\0Dark\0\0"))
        {
            if (mSurroundSelection == 0)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Average);
            if (mSurroundSelection == 1)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Dim);
            if (mSurroundSelection == 2)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Dark);
            
            for (Color& color : mColorScheme.colorSet)
                color.updateColor(ColorSpace::JCh);
            mColorWheel.update(mGridType, mWheelType);
        }
        
        if(ImGui::RadioButton("2", &mStandardObserver, 2))
        {
            mColorWheel.vc.setCIECAMWhitePoint(mIlluminant, mStandardObserver);
            for (Color& color : mColorScheme.colorSet)
                color.updateColor(ColorSpace::JCh);
            mColorWheel.update(mGridType, mWheelType);
        }
        
        ImGui::SameLine();
        if(ImGui::RadioButton("10", &mStandardObserver, 10))
        {
            mColorWheel.vc.setCIECAMWhitePoint(mIlluminant, mStandardObserver);
            for (Color& color : mColorScheme.colorSet)
                color.updateColor(ColorSpace::JCh);
            mColorWheel.update(mGridType, mWheelType);
        }
        
        if(ImGui::Combo("Illuminant", &mIlluminantSelection, "A\0B\0C\0D50\0D55\0D65\0D75\0E\0F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0F11\0F12\0\0"))
        {
            if (mIlluminantSelection == 0)
                mIlluminant = Illuminant::A;
            if (mIlluminantSelection == 1)
                mIlluminant = Illuminant::B;
            if (mIlluminantSelection == 2)
                mIlluminant = Illuminant::C;
            if (mIlluminantSelection == 3)
                mIlluminant = Illuminant::D50;
            if (mIlluminantSelection == 4)
                mIlluminant = Illuminant::D55;
            if (mIlluminantSelection == 5)
                mIlluminant = Illuminant::D65;
            if (mIlluminantSelection == 6)
                mIlluminant = Illuminant::D75;
            if (mIlluminantSelection == 7)
                mIlluminant = Illuminant::E;
            if (mIlluminantSelection == 8)
                mIlluminant = Illuminant::F1;
            if (mIlluminantSelection == 9)
                mIlluminant = Illuminant::F2;
            if (mIlluminantSelection == 10)
                mIlluminant = Illuminant::F3;
            if (mIlluminantSelection == 11)
                mIlluminant = Illuminant::F4;
            if (mIlluminantSelection == 12)
                mIlluminant = Illuminant::F5;
            if (mIlluminantSelection == 13)
                mIlluminant = Illuminant::F6;
            if (mIlluminantSelection == 14)
                mIlluminant = Illuminant::F7;
            if (mIlluminantSelection == 15)
                mIlluminant = Illuminant::F8;
            if (mIlluminantSelection == 16)
                mIlluminant = Illuminant::F9;
            if (mIlluminantSelection == 17)
                mIlluminant = Illuminant::F10;
            if (mIlluminantSelection == 18)
                mIlluminant = Illuminant::F11;
            if (mIlluminantSelection == 19)
                mIlluminant = Illuminant::F12;
            
            mColorWheel.vc.setCIECAMWhitePoint(mIlluminant, mStandardObserver);
            for (Color& color : mColorScheme.colorSet)
                color.updateColor(ColorSpace::JCh);
            mColorWheel.update(mGridType, mWheelType);
        }
        
        if(ImGui::SliderFloat("Surround(La)", &mColorWheel.vc.La, 0, 100))
        {
            if (mSurroundSelection == 0)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Average);
            if (mSurroundSelection == 1)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Dim);
            if (mSurroundSelection == 2)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Dark);
            
            for (Color& color : mColorScheme.colorSet)
                color.updateColor(ColorSpace::JCh);
            mColorWheel.update(mGridType, mWheelType);
        }
        
        if(ImGui::SliderFloat("Background(Yb)", &mColorWheel.vc.Yb, 0, 100))
        {
            if (mSurroundSelection == 0)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Average);
            if (mSurroundSelection == 1)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Dim);
            if (mSurroundSelection == 2)
                mColorWheel.vc.setCIECAMSurrounding(Surrounding::Dark);
            
            if(mBindYbToBg)
                mBgColor.setColor(mColorWheel.vc.Yb, 0, 0, ColorSpace::JCh);
            if(mBindYbToRightBg)
                mRightBgColor.setColor(mColorWheel.vc.Yb, 0, 0, ColorSpace::JCh);
            
            for (Color& color : mColorScheme.colorSet)
                color.updateColor(ColorSpace::JCh);
            mColorWheel.update(mGridType, mWheelType);
        }
        ImGui::Checkbox("Bind Left Background to Yb", &mBindYbToBg);
        ImGui::Checkbox("Bind Right Background to Yb", &mBindYbToRightBg);
    }
    
    ImGui::End();
}

void CIECAMViewer::drawColorPickerGUI()
{
    ImGui::Begin("Colour Picker");
    ImGui::Combo("Input", &mPickerInputSelection, "sRGB\0CIE 1931 XYZ\0CIECAM02 JCh\0CIECAM02 Jab\0CIECAM02 QMh\0\0");
    
    if (mPickerInputSelection == 0)
    {
        if (ImGui::SliderFloat("R", &mColor.trueR, 0, 255))
        {
            mColor.updateColor(ColorSpace::sRGB);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("G", &mColor.trueG, 0, 255))
        {
            mColor.updateColor(ColorSpace::sRGB);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("B", &mColor.trueB, 0, 255))
        {
            mColor.updateColor(ColorSpace::sRGB);
            mPigment.findPigment(mColor);
        }
    }
    else if (mPickerInputSelection == 1)
    {
        if (ImGui::SliderFloat("X", &mColor.X, 0.0, 120.0))
        {
            mColor.updateColor(ColorSpace::XYZ);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("Y", &mColor.Y, 0.0, 120.0))
        {
            mColor.updateColor(ColorSpace::XYZ);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("Z", &mColor.Z, 0.0, 120.0))
        {
            mColor.updateColor(ColorSpace::XYZ);
            mPigment.findPigment(mColor);
        }
        
    }
    else if (mPickerInputSelection == 2)
    {
        if (ImGui::SliderFloat("J", &mColor.J, 0, 100))
        {
            mColor.updateColor(ColorSpace::JCh);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("C", &mColor.C, 0, 120))
        {
            mColor.updateColor(ColorSpace::JCh);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("h", &mColor.h, 0, 360))
        {
            mColor.updateColor(ColorSpace::JCh);
            mPigment.findPigment(mColor);
        }
        
    }
    else if (mPickerInputSelection == 3)
    {
        if (ImGui::SliderFloat("J", &mColor.J, 0, 100))
        {
            mColor.updateColor(ColorSpace::Jab);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("a", &mColor.ac, -120, 120))
        {
            mColor.updateColor(ColorSpace::Jab);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("b", &mColor.bc, -120, 120))
        {
            mColor.updateColor(ColorSpace::Jab);
            mPigment.findPigment(mColor);
        }
        
    }
    else if (mPickerInputSelection == 4)
    {
        if (ImGui::SliderFloat("Q", &mColor.Q, 0, 145))
        {
            mColor.updateColor(ColorSpace::QMh);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("M", &mColor.M, 0, 120))
        {
            mColor.updateColor(ColorSpace::QMh);
            mPigment.findPigment(mColor);
        }
        if (ImGui::SliderFloat("h", &mColor.h, 0, 360))
        {
            mColor.updateColor(ColorSpace::QMh);
            mPigment.findPigment(mColor);
        }
    }
    
    ImGui::Separator();
    
    ImGui::Columns(4, "Conversion Output Column", false);
    drawText({"CIECAM02:"});
    drawText("J: " + std::to_string(mColor.J));
    drawText("C: " + std::to_string(mColor.C));
    drawText("h: " + std::to_string(mColor.h));
    
    drawText({"CIECAM02:"});
    drawText("J: " + std::to_string(mColor.J));
    drawText("a: " + std::to_string(mColor.ac));
    drawText("b: " + std::to_string(mColor.bc));
    
    drawText({"CIECAM02:"});
    drawText("J: " + std::to_string(mColor.J));
    drawText("s: " + std::to_string(mColor.s));
    drawText("h: " + std::to_string(mColor.h));

    drawText({"CIECAM02:"});
    drawText("Q: " + std::to_string(mColor.Q));
    drawText("M: " + std::to_string(mColor.M));
    drawText("h: " + std::to_string(mColor.h));
    
    drawText({"CIE 1931:"});
    drawText("X: " + std::to_string(mColor.X));
    drawText("Y: " + std::to_string(mColor.Y));
    drawText("Z: " + std::to_string(mColor.Z));
    
    drawText({"sRGB:"});
    drawText("R: " + std::to_string(mColor.trueR));
    drawText("G: " + std::to_string(mColor.trueG));
    drawText("B: " + std::to_string(mColor.trueB));
    ImGui::Columns(1);
    
    ImGui::Separator();
    
    if (mColor.isInRGBRange)
        drawText ("");
    else
        drawText ("Colour is out of sRGB range!!!");
    
    ImGui::PushStyleColor(ImGuiCol_Button, sf::Color(mColor.R, mColor.G, mColor.B));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, sf::Color(mColor.R, mColor.G, mColor.B));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, sf::Color(mColor.R, mColor.G, mColor.B));
    
    ImGui::Button("##ColourBox", sf::Vector2f(500,100));
    ImGui::PopStyleColor(3);
    
    if(ImGui::Button("Add1"))
    {
        mColorScheme.addColorToColorSet(mColor, 0);
    }
    ImGui::SameLine();
    if(ImGui::Button("Add2"))
    {
        mColorScheme.addColorToColorSet(mColor, 1);
        
    }
    ImGui::SameLine();
    if(ImGui::Button("Add3"))
    {
        mColorScheme.addColorToColorSet(mColor, 2);
        
    }
    ImGui::SameLine();
    if(ImGui::Button("Add4"))
    {
        mColorScheme.addColorToColorSet(mColor, 3);
        
    }
    ImGui::SameLine();
    if(ImGui::Button("Add5"))
    {
        mColorScheme.addColorToColorSet(mColor, 4);
        
    }
    ImGui::SameLine();
    if(ImGui::Button("GO!"))
    {
        setRandomBoxes();
        setRandomLongBoxes();
        setRandomLines();
        setRandomBoxesWithinBoxes();
        mColorScheme.calculateColorScheme();
    }
    
    if(ImGui::CollapsingHeader("Colour Scheme Generator (by Rythmetic Principle):"))
    {
        if(ImGui::Combo("Input##schemer", &mColourSchemerInputSelection, "sRGB\0CIE 1931 XYZ\0CIECAM02 JCh\0CIECAM02 Jab\0CIECAM02 QMh\0\0"))
        {
            if (mColourSchemerInputSelection == 0)
            {
                if (mColorSpanA > 127)
                    mColorSpanA = 127;
                if (mColorSpanB > 127)
                    mColorSpanB = 127;
                if (mColorSpanC > 127)
                    mColorSpanC = 127;
            }
            else if (mColourSchemerInputSelection == 1)
            {
                if (mColorSpanA > 50)
                    mColorSpanA = 50;
                if (mColorSpanB > 50)
                    mColorSpanB = 50;
                if (mColorSpanC > 50)
                    mColorSpanC = 50;
            }
            else if (mColourSchemerInputSelection == 2)
            {
                if (mColorSpanA > 50)
                    mColorSpanA = 50;
                if (mColorSpanB > 60)
                    mColorSpanB = 60;
                if (mColorSpanC > 144)
                    mColorSpanC = 144;
            }
            else if (mColourSchemerInputSelection == 3)
            {
                if (mColorSpanA > 50)
                    mColorSpanA = 50;
                if (mColorSpanB > 60)
                    mColorSpanB = 60;
                if (mColorSpanC > 60)
                    mColorSpanC = 60;
            }
            else if (mColourSchemerInputSelection == 4)
            {
                if (mColorSpanA > 72)
                    mColorSpanA = 72;
                if (mColorSpanB > 60)
                    mColorSpanB = 60;
                if (mColorSpanC > 144)
                    mColorSpanC = 144;
            }
        }
        
        if (mColourSchemerInputSelection == 0)
        {            
            ImGui::SliderFloat("R +/-Span", &mColorSpanA, 0, 127);
            ImGui::SliderFloat("G +/-Span", &mColorSpanB, 0, 127);
            ImGui::SliderFloat("B +/-Span", &mColorSpanC, 0, 127);
        }
        else if (mColourSchemerInputSelection == 1)
        {
            ImGui::SliderFloat("X +/-Span", &mColorSpanA, 0, 50);
            ImGui::SliderFloat("Y +/-Span", &mColorSpanB, 0, 50);
            ImGui::SliderFloat("Z +/-Span", &mColorSpanC, 0, 50);
        }
        else if (mColourSchemerInputSelection == 2)
        {
            ImGui::SliderFloat("J +/-Span", &mColorSpanA, 0, 50);
            ImGui::SliderFloat("C +/-Span", &mColorSpanB, 0, 60);
            ImGui::SliderFloat("h +/-Span", &mColorSpanC, 0, 144);
        }
        else if (mColourSchemerInputSelection == 3)
        {
            ImGui::SliderFloat("J +/-Span", &mColorSpanA, 0, 50);
            ImGui::SliderFloat("a +/-Span", &mColorSpanB, 0, 60);
            ImGui::SliderFloat("b +/-Span", &mColorSpanC, 0, 60); 
        }
        else if (mColourSchemerInputSelection == 4)
        {
            ImGui::SliderFloat("Q +/-Span", &mColorSpanA, 0, 72);
            ImGui::SliderFloat("M +/-Span", &mColorSpanB, 0, 60);
            ImGui::SliderFloat("h +/-Span", &mColorSpanC, 0, 144);
        }
        ImGui::Checkbox("Bounded Elements", &mIsRythmicElementBound);
        
        if(mIsRythmicElementBound)
        {
            ImGui::SameLine();
            ImGui::Checkbox("Reversed Second Element", &mReversedColorSpanB);
            ImGui::SameLine();
            ImGui::Checkbox("Reversed Third Element", &mReversedColorSpanC);
        }
        
        
        if(ImGui::Button("Add All"))
        {
            mColorScheme.addColorToColorSet(mColor, 2);
            
            if(mIsRythmicElementBound)
            {
                int b{1}, c{1};
                if(mReversedColorSpanB)
                    b = -1;
                if(mReversedColorSpanC)
                    c = -1;
                
                if (mColourSchemerInputSelection == 0)
                {
                    mTempColor.setColor(mColor.trueR - mColorSpanA, mColor.trueG - mColorSpanB * b * b, mColor.trueB - mColorSpanC * c, ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 0);
                    mTempColor.setColor(mColor.trueR - (mColorSpanA / 2), mColor.trueG - (mColorSpanB * b / 2), mColor.trueB - (mColorSpanC * c / 2), ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 1);
                    mTempColor.setColor(mColor.trueR + (mColorSpanA / 2), mColor.trueG + (mColorSpanB * b / 2), mColor.trueB + (mColorSpanC * c / 2), ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 3);
                    mTempColor.setColor(mColor.trueR + mColorSpanA, mColor.trueG + mColorSpanB * b, mColor.trueB + mColorSpanC * c, ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 4);
                }
                else if (mColourSchemerInputSelection == 1)
                {
                    mTempColor.setColor(mColor.X - mColorSpanA, mColor.Y - mColorSpanB * b, mColor.Z - mColorSpanC * c, ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 0);
                    mTempColor.setColor(mColor.X - (mColorSpanA / 2), mColor.Y - (mColorSpanB * b / 2), mColor.Z - (mColorSpanC * c / 2), ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 1);
                    mTempColor.setColor(mColor.X + (mColorSpanA / 2), mColor.Y + (mColorSpanB * b / 2), mColor.Z + (mColorSpanC * c / 2), ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 3);
                    mTempColor.setColor(mColor.X + mColorSpanA, mColor.Y + mColorSpanB * b, mColor.Z + mColorSpanC * c, ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 4); 
                }
                else if (mColourSchemerInputSelection == 2)
                {
                    mTempColor.setColor(mColor.J - mColorSpanA, mColor.C - mColorSpanB * b, mColor.h - mColorSpanC * c, ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 0);
                    mTempColor.setColor(mColor.J - (mColorSpanA / 2), mColor.C - (mColorSpanB * b / 2), mColor.h - (mColorSpanC * c / 2), ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 1);
                    mTempColor.setColor(mColor.J + (mColorSpanA / 2), mColor.C + (mColorSpanB * b / 2), mColor.h + (mColorSpanC * c / 2), ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 3);
                    mTempColor.setColor(mColor.J + mColorSpanA, mColor.C + mColorSpanB * b, mColor.h + mColorSpanC * c, ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 4); 
                }
                else if (mColourSchemerInputSelection == 3)
                {
                    mTempColor.setColor(mColor.J - mColorSpanA, mColor.ac - mColorSpanB * b, mColor.bc - mColorSpanC * c, ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 0);
                    mTempColor.setColor(mColor.J - (mColorSpanA / 2), mColor.ac - (mColorSpanB * b / 2), mColor.bc - (mColorSpanC * c / 2), ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 1);
                    mTempColor.setColor(mColor.J + (mColorSpanA / 2), mColor.ac + (mColorSpanB * b / 2), mColor.bc + (mColorSpanC * c / 2), ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 3);
                    mTempColor.setColor(mColor.J + mColorSpanA, mColor.ac + mColorSpanB * b, mColor.bc + mColorSpanC * c, ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 4); 
                }
                else if (mColourSchemerInputSelection == 4)
                {
                    mTempColor.setColor(mColor.Q - mColorSpanA, mColor.C - mColorSpanB * b, mColor.h - mColorSpanC * c, ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 0);
                    mTempColor.setColor(mColor.Q - (mColorSpanA / 2), mColor.C - (mColorSpanB * b / 2), mColor.h - (mColorSpanC * c / 2), ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 1);
                    mTempColor.setColor(mColor.Q + (mColorSpanA / 2), mColor.C + (mColorSpanB * b / 2), mColor.h + (mColorSpanC * c / 2), ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 3);
                    mTempColor.setColor(mColor.Q + mColorSpanA, mColor.C + mColorSpanB * b, mColor.h + mColorSpanC * c, ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 4); 
                } 
            }
            else
            {
                int dice1;
                int dice2;
                int dice3;
                std::vector<bool> isColorSpanASet(5, false);
                std::vector<bool> isColorSpanBSet(5, false);
                std::vector<bool> isColorSpanCSet(5, false);
                
                isColorSpanASet.at(2) = true;
                isColorSpanBSet.at(2) = true;
                isColorSpanCSet.at(2) = true;
                
                std::vector<float> spanA {-mColorSpanA, -mColorSpanA / 2, 0, mColorSpanA / 2, mColorSpanA};
                std::vector<float> spanB {-mColorSpanB, -mColorSpanB / 2, 0, mColorSpanB / 2, mColorSpanB};
                std::vector<float> spanC {-mColorSpanC, -mColorSpanC / 2, 0, mColorSpanC / 2, mColorSpanC};
                
                if (mColourSchemerInputSelection == 0)
                {                    
                    dice1 = rand() % 4;
                    dice2 = rand() % 4;
                    dice3 = rand() % 4;
                    
                    for (int i = 0; i < 5; i++)
                    {
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.trueR = mColor.trueR + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.trueG = mColor.trueG + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.trueB = mColor.trueB + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        dice2--;
                        dice3--;
                    }
                    mTempColor.updateColor(ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 0); 
                    
                    dice1 = rand() % 3;
                    dice2 = rand() % 3;
                    dice3 = rand() % 3;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.trueR = mColor.trueR + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.trueG = mColor.trueG + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.trueB = mColor.trueB + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 1); 
                    
                    dice1 = rand() % 2;
                    dice2 = rand() % 2;
                    dice3 = rand() % 2;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.trueR = mColor.trueR + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.trueG = mColor.trueG + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.trueB = mColor.trueB + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 3); 
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if (isColorSpanASet.at(i) == false)
                        {
                            mTempColor.trueR = mColor.trueR + spanA.at(i);
                            isColorSpanASet.at(i) = true;
                            
                        }
                        if (isColorSpanBSet.at(i) == false)
                        {
                            mTempColor.trueG = mColor.trueG + spanB.at(i);
                            isColorSpanBSet.at(i) = true;
                        }
                        if (isColorSpanCSet.at(i) == false)
                        {
                            mTempColor.trueB = mColor.trueB + spanC.at(i);
                            isColorSpanCSet.at(i) = true;
                            
                        }
                    }
                    mTempColor.updateColor(ColorSpace::sRGB);
                    mColorScheme.addColorToColorSet(mTempColor, 4);
                }
                if (mColourSchemerInputSelection == 1)
                {                    
                    dice1 = rand() % 4;
                    dice2 = rand() % 4;
                    dice3 = rand() % 4;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.X = mColor.X + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.Y = mColor.Y + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.Z = mColor.Z + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 0); 
                    
                    dice1 = rand() % 3;
                    dice2 = rand() % 3;
                    dice3 = rand() % 3;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.X = mColor.X + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.Y = mColor.Y + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.Z = mColor.Z + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 1); 
                    
                    dice1 = rand() % 2;
                    dice2 = rand() % 2;
                    dice3 = rand() % 2;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.X = mColor.X + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.Y = mColor.Y + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.Z = mColor.Z + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 3); 
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if (isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.X = mColor.X + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if (isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.Y = mColor.Y + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if (isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.Z = mColor.Z + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                    }
                    mTempColor.updateColor(ColorSpace::XYZ);
                    mColorScheme.addColorToColorSet(mTempColor, 4);
                }
                if (mColourSchemerInputSelection == 2)
                {                    
                    dice1 = rand() % 4;
                    dice2 = rand() % 4;
                    dice3 = rand() % 4;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.J = mColor.J + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.C = mColor.C + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.h = mColor.h + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                    }
                    mTempColor.updateColor(ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 0); 
                    
                    dice1 = rand() % 3;
                    dice2 = rand() % 3;
                    dice3 = rand() % 3;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.J = mColor.J + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.C = mColor.C + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.h = mColor.h + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 1); 
                    
                    dice1 = rand() % 2;
                    dice2 = rand() % 2;
                    dice3 = rand() % 2;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.J = mColor.J + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.C = mColor.C + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.h = mColor.h + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                    }
                    mTempColor.updateColor(ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 3); 
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if (isColorSpanASet.at(i) == false)
                        {
                            mTempColor.J = mColor.J + spanA.at(i);
                            isColorSpanASet.at(i) = true;
                        }
                        if (isColorSpanBSet.at(i) == false)
                        {
                            mTempColor.C = mColor.C + spanB.at(i);
                            isColorSpanBSet.at(i) = true;
                        }
                        if (isColorSpanCSet.at(i) == false)
                        {
                            mTempColor.h = mColor.h + spanC.at(i);
                            isColorSpanCSet.at(i) = true;
                        }
                    }
                    mTempColor.updateColor(ColorSpace::JCh);
                    mColorScheme.addColorToColorSet(mTempColor, 4);
                }
                if (mColourSchemerInputSelection == 3)
                {                    
                    dice1 = rand() % 4;
                    dice2 = rand() % 4;
                    dice3 = rand() % 4;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.J = mColor.J + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.ac = mColor.ac + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.bc = mColor.bc + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 0); 
                    
                    dice1 = rand() % 3;
                    dice2 = rand() % 3;
                    dice3 = rand() % 3;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.J = mColor.J + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.ac = mColor.ac + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.bc = mColor.bc + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 1); 
                    
                    dice1 = rand() % 2;
                    dice2 = rand() % 2;
                    dice3 = rand() % 2;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.J = mColor.J + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.ac = mColor.ac + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.bc = mColor.bc + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 3); 
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if (isColorSpanASet.at(i) == false)
                        {
                            mTempColor.J = mColor.J + spanA.at(i);
                            isColorSpanASet.at(i) = true;
                        }
                        if (isColorSpanBSet.at(i) == false)
                        {
                            mTempColor.ac = mColor.ac + spanB.at(i);
                            isColorSpanBSet.at(i) = true;
                        }
                        if (isColorSpanCSet.at(i) == false)
                        {
                            mTempColor.bc = mColor.bc + spanC.at(i);
                            isColorSpanCSet.at(i) = true;
                        }
                    }
                    mTempColor.updateColor(ColorSpace::Jab);
                    mColorScheme.addColorToColorSet(mTempColor, 4);
                }
                if (mColourSchemerInputSelection == 4)
                {                    
                    dice1 = rand() % 4;
                    dice2 = rand() % 4;
                    dice3 = rand() % 4;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.Q = mColor.Q + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.M = mColor.M + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.h = mColor.h + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 0); 
                    
                    dice1 = rand() % 3;
                    dice2 = rand() % 3;
                    dice3 = rand() % 3;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.Q = mColor.Q + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.M = mColor.M + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.h = mColor.h + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 1); 
                    
                    dice1 = rand() % 2;
                    dice2 = rand() % 2;
                    dice3 = rand() % 2;
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if(isColorSpanASet.at(i) == false)
                        {
                            if (dice1 == 0)
                            {
                                mTempColor.Q = mColor.Q + spanA.at(i);
                                isColorSpanASet.at(i) = true;
                            }
                            dice1--;
                        }
                        if(isColorSpanBSet.at(i) == false)
                        {
                            if (dice2 == 0)
                            {
                                mTempColor.M = mColor.M + spanB.at(i);
                                isColorSpanBSet.at(i) = true;
                            }
                            dice2--;
                        }
                        if(isColorSpanCSet.at(i) == false)
                        {
                            if (dice3 == 0)
                            {
                                mTempColor.h = mColor.h + spanC.at(i);
                                isColorSpanCSet.at(i) = true;
                            }
                            dice3--;
                        }
                        
                    }
                    mTempColor.updateColor(ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 3); 
                    
                    for (int i = 0; i < 5; i++)
                    {                        
                        if (isColorSpanASet.at(i) == false)
                        {
                            mTempColor.Q = mColor.Q + spanA.at(i);
                            isColorSpanASet.at(i) = true;
                            
                        }
                        if (isColorSpanBSet.at(i) == false)
                        {
                            mTempColor.M = mColor.M + spanB.at(i);
                            isColorSpanBSet.at(i) = true;
                            
                        }
                        if (isColorSpanCSet.at(i) == false)
                        {
                            mTempColor.h = mColor.h + spanC.at(i);
                            isColorSpanCSet.at(i) = true;
                        }
                    }
                    mTempColor.updateColor(ColorSpace::QMh);
                    mColorScheme.addColorToColorSet(mTempColor, 4);
                }
            }
            
            setRandomBoxes();
            setRandomLongBoxes();
            setRandomLines();
            setRandomBoxesWithinBoxes();
            mColorScheme.calculateColorScheme();
        }
        ImGui::SameLine();
        if(ImGui::Button("GO!##2"))
        {
            setRandomBoxes();
            setRandomLongBoxes();
            setRandomLines();
            setRandomBoxesWithinBoxes();
            mColorScheme.calculateColorScheme();
        }        
    }
    
    if(ImGui::CollapsingHeader("Nearest Pigments (by watercolour masstone):"))
    {
        for (int i = 0; i < 10; i++)
        {
            mTempColor.setColor(mPigment.pigmentList.at(i).J, mPigment.pigmentList.at(i).a, mPigment.pigmentList.at(i).b, ColorSpace::Jab);
            
            std::string text {"Select##" + std::to_string(i)};
            char buttonText[1024];
            strncpy(buttonText, text.c_str(), sizeof(buttonText));
            buttonText[sizeof(buttonText) - 1] = 0;
            
            ImGui::PushStyleColor(ImGuiCol_Button, sf::Color(mTempColor.R, mTempColor.G, mTempColor.B));
            
            if(ImGui::Button(buttonText, sf::Vector2f(62,20)))
            {
                mColor.setColor(mTempColor.J, mTempColor.C, mTempColor.h, ColorSpace::JCh);
                mPigment.findPigment(mColor);
            }
            ImGui::PopStyleColor(1);
            
            ImGui::SameLine();
            
            drawText(mPigment.pigmentList.at(i).commonName + "(" + mPigment.pigmentList.at(i).pigmentCode + ") [JCh: "
            + std::to_string((int)mTempColor.J) + "," + std::to_string((int)mTempColor.C) + "," + std::to_string((int)mTempColor.h) + "]");
            drawText("alias:    " + mPigment.pigmentList.at(i).alias);
            drawText("pigment:  " + mPigment.pigmentList.at(i).pigmentCode);
            if (mPigment.pigmentList.at(i).otherPigment != "")
            {
                ImGui::SameLine();
                drawText(", " + mPigment.pigmentList.at(i).otherPigment);
            }
            drawText("chemical: " + mPigment.pigmentList.at(i).chemical);
            drawText("delta-E:  " + std::to_string((int)mPigment.pigmentList.at(i).distanceFromTarget));
            ImGui::SameLine();
            drawText(" (J:" + std::to_string((int)(mTempColor.J - mColor.J)));            
            ImGui::SameLine();
            drawText(" C:" + std::to_string((int)(mTempColor.C - mColor.C)));
            ImGui::SameLine();
            
            if (mTempColor.h - mColor.h > 180)
                drawText(" h:" + std::to_string((int)(mTempColor.h - mColor.h - 360)) + ")");
            else if (mTempColor.h - mColor.h < -180)
                drawText(" h:" + std::to_string((int)(mTempColor.h - mColor.h + 360)) + ")");
            else
                drawText(" h:" + std::to_string((int)(mTempColor.h - mColor.h)) + ")");
            
            drawText("note:     " + mPigment.pigmentList.at(i).note);
            ImGui::Separator();
        }
    }
    
    ImGui::End();
}

void CIECAMViewer::drawImageAnalyzerGUI()
{
    ImGui::Begin("Image Analyzer");
    
    ImGui::InputText("File Name", mCharFileName, 100);
    mFileName = mCharFileName;
    
    ImGui::Checkbox("Map image gamut", &mIsImageGamutShownOnColorWheel);
    
    if (mIsImageGamutShownOnColorWheel)
    {
        ImGui::SameLine();
        ImGui::Checkbox("Higher sampling rate(slow)", &mIsHigherAccuracyImageGamut); 
    }
    
    if (ImGui::Button("Analyze image"))
    {
        sf::Image image;
        
        if (!std::experimental::filesystem::exists(mFileName))                                      //determine file extension without printing error message to console
        {
            if (std::experimental::filesystem::exists(mFileName + ".jpg"))
                mFileName = mFileName + ".jpg";
            else if (std::experimental::filesystem::exists(mFileName + ".jpeg"))
                mFileName = mFileName + ".jpeg";
            else if (std::experimental::filesystem::exists(mFileName +".png"))
                mFileName = mFileName + ".png";
            else if (std::experimental::filesystem::exists(mFileName + ".bmp"))
                mFileName = mFileName + ".bmp";
            else if (std::experimental::filesystem::exists(mFileName + ".tga"))
                mFileName = mFileName + ".tga";
            else if (std::experimental::filesystem::exists(mFileName +".gif"))
                mFileName = mFileName + ".gif";
        }

        if (image.loadFromFile(mFileName))
        {
            sf::Vector2u imageSize = image.getSize();
            int randX;
            int randY;
            
            mImageColor.pigmentList.clear();
            
            for (int x = 0; x < 10000; x++)
            {
                randX = std::rand() % imageSize.x;
                randY = std::rand() % imageSize.y;

                sf::Color color = image.getPixel(randX, randY);
                mTempColor.setColor(color.r, color.g, color.b, ColorSpace::sRGB);
                
                mImageColorScheme.addColorToColorSet(mTempColor, x);
                
                if (x % 5 == 0 || mIsHigherAccuracyImageGamut)                                      //We only need 2000 samples of pixel for gamut mapping to save CPU proccessing
                {
                    Pigment pigment;                                                                //Pretend pixels from image = pigment
                    pigment.J = mImageColorScheme.colorSet.at(x).J;
                    pigment.a = mImageColorScheme.colorSet.at(x).ac;
                    pigment.b = mImageColorScheme.colorSet.at(x).bc;
                    pigment.distanceFromTarget = 0;
                    
                    mImageColor.pigmentList.push_back(pigment);
                }
            }

            mImageColorScheme.calculateColorScheme();
        }
    }
    
    ImGui::Separator();
    drawText("Kansei Scale Rating:");
    drawText(mImageColorScheme.getDescWramth() + " \(" + std::to_string(mImageColorScheme.getWarmth()) + ")");
    drawText(mImageColorScheme.getDescSoftness() + " \(" + std::to_string(mImageColorScheme.getSoftness()) + ")");
    drawText(mImageColorScheme.getDescNaturalness() + " \(" + std::to_string(mImageColorScheme.getNaturalness()) + ")");
    drawText(mImageColorScheme.getDescActiveness() + " \(" + std::to_string(mImageColorScheme.getActiveness()) + ")");
    drawText(mImageColorScheme.getDescBrightness() + " \(" + std::to_string(mImageColorScheme.getBrightness()) + ")");
    ImGui::Separator();
    drawText("Chomaticness Average: (" + std::to_string(mImageColorScheme.mChromaticnessAverage) + ")");
    drawText("Lightness Average: (" + std::to_string(mImageColorScheme.mLightnessAverage) + ")");
    drawText("Lightness Chromaticness Average: (" + std::to_string(mImageColorScheme.mLightnessChromaticnessAverage) + ")");
    drawText("Lightness Distributed Degree: (" + std::to_string(mImageColorScheme.mLightnessDistributedDegree) + ")");
    drawText("Distance Distributed Degree: (" + std::to_string(mImageColorScheme.mDistanceDistributedDegree) + ")");
    ImGui::Separator();
    
    drawText("Saturation Average: (" + std::to_string(mImageColorScheme.mSaturationAverage) + ")");
    drawText("Saturation Distributed Degree: (" + std::to_string(mImageColorScheme.mSaturationDistributedDegree) + ")");
    ImGui::End();
}



void CIECAMViewer::drawText(std::string text)
{
    char outputText[1024];
    strncpy(outputText, text.c_str(), sizeof(outputText));
    outputText[sizeof(outputText) - 1] = 0;
    ImGui::TextWrapped(outputText);
    //ImGui::Text(outputText);
    ImGui::NextColumn();
}

void CIECAMViewer::drawCircleGrid()
{
    sf::CircleShape circle(20);
    circle.setOrigin({20,20});
    
    for (int x = 0; x < 241; x = x + 10)
    {
        for (int y = 20; y < 220; y = y + 10)
        {
            if (mColorWheel.color.at(y).at(x).isInRGBRange)
                circle.setFillColor(sf::Color(mColorWheel.color.at(y).at(x).R, mColorWheel.color.at(y).at(x).G, mColorWheel.color.at(y).at(x).B));
            else
                circle.setFillColor(sf::Color(mBgColor.R, mBgColor.G, mBgColor.B));
            
            if(mIsImageGamutShownOnColorWheel)
            {
                bool isInImageGamut{false};
                for (const Pigment& imagePixel : mImageColor.pigmentList)
                {
                    if(std::abs(mColorWheel.color.at(y).at(x).J - imagePixel.J) < 5 &&
                                std::abs(mColorWheel.color.at(y).at(x).ac - imagePixel.a) < 5 &&
                                std::abs(mColorWheel.color.at(y).at(x).bc - imagePixel.b) < 5 && 
                                mColorWheel.color.at(y).at(x).isInRGBRange)
                    {
                        isInImageGamut = true;
                        break;          // saves cpu power for large sample sizes
                    }             
                }
                
                if (!isInImageGamut && mColorWheel.color.at(y).at(x).isInRGBRange)
                {
                    mTempColor.setColor(mColorWheel.color.at(y).at(x).R, mColorWheel.color.at(y).at(x).G, mColorWheel.color.at(y).at(x).B, ColorSpace::sRGB);
                    mTempColor.setColor(mTempColor.J / 4, mTempColor.C / 4, mTempColor.h, ColorSpace::JCh);                    //Slash Lightness and Chroma by 4                    
                    circle.setFillColor(sf::Color(mTempColor.R, mTempColor.G, mTempColor.B));
                }
            }            
            
            circle.setPosition(x * 5 + 20, (y - 20) * 5 + 20);
            mWindow.draw(circle);            
        }
    }
    
    circle.setRadius(5);
    circle.setPointCount(4);
    circle.setOrigin({5,5});
    circle.setFillColor(sf::Color::White);
    circle.setPosition(620, 520);
    
    mWindow.draw(circle);
    
    if(mIsPigmentShown)
    {
        for (int x = 0; x < 241; x = x + 10)
        {
            for (int y = 20; y < 220; y = y + 10)
            {           
                mTempPigment.findPigment(mColorWheel.color.at(y).at(x));
                
                if(std::abs(mColorWheel.color.at(y).at(x).J - mTempPigment.pigmentList.at(0).J) < 5 &&  //we need to detect pigment within a cube shape area around this point
                    std::abs(mColorWheel.color.at(y).at(x).ac - mTempPigment.pigmentList.at(0).a) < 5 &&
                    std::abs(mColorWheel.color.at(y).at(x).bc - mTempPigment.pigmentList.at(0).b) < 5)
                {                
                    sf::Text text;
                    text.setFont(mFont);
                    text.setString(mTempPigment.pigmentList.at(0).pigmentCode);
                    text.setCharacterSize(10);
                    text.setOrigin({15,5});                   
                    text.setPosition(x * 5 + 20, (y - 20) * 5 + 20);
                    
                    if (mColorWheel.color.at(y).at(x).J < 50)
                        text.setFillColor(sf::Color::White);
                    else
                        text.setFillColor(sf::Color::Black);
                    
                    mWindow.draw(text);
                }
            }
        }
    }    
}

void CIECAMViewer::drawHexGrid()
{
    sf::CircleShape circle(20);
    circle.setOrigin({20,20});
    bool isLastLineNormal = true;
    
    for (int y = 21; y < 220; y = y + 9)
    {
        for (int x = 0; x < 241; x = x + 10)
        {
            if (isLastLineNormal)
            {
                if (x < 236)
                {
                    if (mColorWheel.color.at(y).at(x  + 5).isInRGBRange)
                        circle.setFillColor(sf::Color(mColorWheel.color.at(y).at(x  + 5).R, mColorWheel.color.at(y).at(x  + 5).G, mColorWheel.color.at(y).at(x  + 5).B));
                    else
                        circle.setFillColor(sf::Color(mBgColor.R, mBgColor.G, mBgColor.B));
                    
                    if(mIsImageGamutShownOnColorWheel)
                    {
                        bool isInImageGamut{false};
                        for (const Pigment& imagePixel: mImageColor.pigmentList)
                        {
                            if(std::abs(mColorWheel.color.at(y).at(x  + 5).J - imagePixel.J) < 5 &&
                                std::abs(mColorWheel.color.at(y).at(x  + 5).ac - imagePixel.a) < 5 &&
                                std::abs(mColorWheel.color.at(y).at(x  + 5).bc - imagePixel.b) < 5 && 
                                mColorWheel.color.at(y).at(x  + 5).isInRGBRange)
                            {
                                isInImageGamut = true;
                                break;          // saves cpu power for large sample sizes
                            }          
                        }
                        
                        if (!isInImageGamut && mColorWheel.color.at(y).at(x + 5).isInRGBRange)
                        {
                            mTempColor.setColor(mColorWheel.color.at(y).at(x + 5).R, mColorWheel.color.at(y).at(x + 5).G, mColorWheel.color.at(y).at(x + 5).B, ColorSpace::sRGB);
                            mTempColor.setColor(mTempColor.J / 4, mTempColor.C / 4, mTempColor.h, ColorSpace::JCh);                    //Slash Lightness and Chroma by 4                    
                            circle.setFillColor(sf::Color(mTempColor.R, mTempColor.G, mTempColor.B));
                        }
                    }            
                    circle.setPosition(x * 5 + 45, (y - 20) * 5 + 20);
                }                
            }
            else
            {
                if (mColorWheel.color.at(y).at(x).isInRGBRange)
                    circle.setFillColor(sf::Color(mColorWheel.color.at(y).at(x).R, mColorWheel.color.at(y).at(x).G, mColorWheel.color.at(y).at(x).B));
                else
                    circle.setFillColor(sf::Color(mBgColor.R, mBgColor.G, mBgColor.B));
                
                if(mIsImageGamutShownOnColorWheel)
                {
                    bool isInImageGamut{false};
                    for (const Pigment& imagePixel: mImageColor.pigmentList)
                    {
                        if(std::abs(mColorWheel.color.at(y).at(x).J - imagePixel.J) < 5 &&
                            std::abs(mColorWheel.color.at(y).at(x).ac - imagePixel.a) < 5 &&
                            std::abs(mColorWheel.color.at(y).at(x).bc - imagePixel.b) < 5 && 
                            mColorWheel.color.at(y).at(x).isInRGBRange)
                        {
                            isInImageGamut = true;
                            break;          // saves cpu power for large sample sizes
                        }                
                    }
                    
                    if (!isInImageGamut && mColorWheel.color.at(y).at(x).isInRGBRange)
                    {
                        mTempColor.setColor(mColorWheel.color.at(y).at(x).R, mColorWheel.color.at(y).at(x).G, mColorWheel.color.at(y).at(x).B, ColorSpace::sRGB);
                        mTempColor.setColor(mTempColor.J / 4, mTempColor.C / 4, mTempColor.h, ColorSpace::JCh);                    //Slash Lightness and Chroma by 4                    
                        circle.setFillColor(sf::Color(mTempColor.R, mTempColor.G, mTempColor.B));
                    }
                }            
                
                circle.setPosition(x * 5 + 20, (y - 20) * 5 + 20);
            }
            mWindow.draw(circle);
        }
        isLastLineNormal = !isLastLineNormal;
    }
    
    circle.setRadius(5);
    circle.setPointCount(4);
    circle.setOrigin({5,5});
    circle.setFillColor(sf::Color::White);
    circle.setPosition(620, 520);
    
    mWindow.draw(circle);
    
    
    if(mIsPigmentShown)
    {
        isLastLineNormal = true;
        
        for (int y = 21; y < 220; y = y + 9)
        {
            for (int x = 0; x < 241; x = x + 10)
            {
                if (isLastLineNormal)
                {
                    
                    if (x < 236)
                    {
                        mTempPigment.findPigment(mColorWheel.color.at(y).at(x + 5));
                        if(std::abs(mColorWheel.color.at(y).at(x + 5).J - mTempPigment.pigmentList.at(0).J) < 5 &&  //we need to detect pigment within a cube shape area around this point
                            std::abs(mColorWheel.color.at(y).at(x + 5).ac - mTempPigment.pigmentList.at(0).a) < 5 &&
                            std::abs(mColorWheel.color.at(y).at(x + 5).bc - mTempPigment.pigmentList.at(0).b) < 5)
                        {                
                            sf::Text text;
                            text.setFont(mFont);
                            text.setString(mTempPigment.pigmentList.at(0).pigmentCode);
                            text.setCharacterSize(10);
                            text.setOrigin({15,5});   
                            text.setPosition(x * 5 + 45, (y - 20) * 5 + 20);
                            
                            if (mColorWheel.color.at(y).at(x + 5).J < 50)
                                text.setFillColor(sf::Color::White);
                            else
                                text.setFillColor(sf::Color::Black);
                            
                            mWindow.draw(text);
                        }
                        
                    }
                }
                else
                {
                    mTempPigment.findPigment(mColorWheel.color.at(y).at(x));
                    if(std::abs(mColorWheel.color.at(y).at(x).J - mTempPigment.pigmentList.at(0).J) < 5 &&  //we need to detect pigment within a cube shape area around this point
                        std::abs(mColorWheel.color.at(y).at(x).ac - mTempPigment.pigmentList.at(0).a) < 5 &&
                        std::abs(mColorWheel.color.at(y).at(x).bc - mTempPigment.pigmentList.at(0).b) < 5)
                    {                
                        
                        sf::Text text;
                        text.setFont(mFont);
                        text.setString(mTempPigment.pigmentList.at(0).pigmentCode);
                        text.setCharacterSize(10);
                        text.setOrigin({15,5});
                        text.setPosition(x * 5 + 20, (y - 20) * 5 + 20);
                        
                        if (mColorWheel.color.at(y).at(x).J < 50)
                            text.setFillColor(sf::Color::White);
                        else
                            text.setFillColor(sf::Color::Black);
                        
                        mWindow.draw(text);
                        
                    }                    
                }
            }
            isLastLineNormal = !isLastLineNormal;
        }
    }    
}


void CIECAMViewer::drawSolidGrid()
{
    sf::RectangleShape rectangle({10,10});
    rectangle.setOrigin({5,5});
    
    for (int x = 0; x < 241; x++)
    {
        for (int y = 20; y < 221; y++)
        {
            if (mColorWheel.color.at(y).at(x).isInRGBRange)
                rectangle.setFillColor(sf::Color(mColorWheel.color.at(y).at(x).R, mColorWheel.color.at(y).at(x).G, mColorWheel.color.at(y).at(x).B));
            else
                rectangle.setFillColor(sf::Color(mBgColor.R, mBgColor.G, mBgColor.B));
            
            rectangle.setPosition(x * 5 + 25, (y - 20) * 5 + 25);
            mWindow.draw(rectangle);
        }
    }
    
    sf::CircleShape circle(5,4);
    circle.setOrigin({5,5});
    circle.setFillColor(sf::Color::White);
    circle.setPosition(625, 525);
    
    mWindow.draw(circle);
    
    if(mIsPigmentShown)
    {
        for (int x = 0; x < 241; x = x + 10)
        {
            for (int y = 20; y < 220; y = y + 10)
            {           
                mTempPigment.findPigment(mColorWheel.color.at(y).at(x));
                
                if(std::abs(mColorWheel.color.at(y).at(x).J - mTempPigment.pigmentList.at(0).J) < 5 &&          //we need the cube shape area around this point
                    std::abs(mColorWheel.color.at(y).at(x).ac - mTempPigment.pigmentList.at(0).a) < 5 &&
                    std::abs(mColorWheel.color.at(y).at(x).bc - mTempPigment.pigmentList.at(0).b) < 5)
                {                
                    sf::Text text;
                    text.setFont(mFont);
                    text.setString(mTempPigment.pigmentList.at(0).pigmentCode);
                    text.setCharacterSize(10);
                    text.setOrigin({15,5});                   
                    text.setPosition(x * 5 + 20, (y - 20) * 5 + 20);
                    
                    if (mColorWheel.color.at(y).at(x).J < 50)
                        text.setFillColor(sf::Color::White);
                    else
                        text.setFillColor(sf::Color::Black);
                    
                    mWindow.draw(text);
                }
            }
        }
    }    
}

void CIECAMViewer::drawColorSchemer()
{
    float startingXPos {1240};
    float startingYPos {0};
    
    sf::RectangleShape background (sf::Vector2f(mWindow.getSize().x - startingXPos, mWindow.getSize().y - startingYPos));
    background.setPosition(startingXPos, startingYPos);
    background.setFillColor(sf::Color(mRightBgColor.R, mRightBgColor.G,mRightBgColor.B));
    mWindow.draw(background);
    
    sf::Text text;
    text.setFont(mFont);
    text.setString("Colour Scheme Generator:");
    text.setCharacterSize(18);
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 10));
    text.setFillColor(sf::Color::Black);
    mWindow.draw(text);
    text.setCharacterSize(10);
    
    sf::RectangleShape colorPalette;
    
    for (int i = 0; i < 5; i++)
    {
        //draw colour palette
        sf::RectangleShape colorPalette (sf::Vector2f(100, 100));
        colorPalette.setPosition(startingXPos + 10 + 110 * i, startingYPos + 30);
        colorPalette.setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
        mWindow.draw(colorPalette);
        
        text.setPosition(startingXPos + 10 + 110 * i, startingYPos + 30);
        text.setString("(" + std::to_string((int)mColorScheme.colorSet.at(i).J) + "," + std::to_string((int)mColorScheme.colorSet.at(i).C) + "," + 
        std::to_string((int)mColorScheme.colorSet.at(i).h) + ")" );
        if(mColorScheme.colorSet.at(i).J < 50)
            text.setFillColor(sf::Color::White);
        mWindow.draw(text);
        text.setFillColor(sf::Color::Black);
        if(!mColorScheme.colorSet.at(i).isInRGBRange)
        {
            text.setPosition(startingXPos + 10 + 190 * i, startingYPos + 30);
            text.setString("!!!");
            mWindow.draw(text);
        }
        
        //draw sample images below the palette
        mWindow.draw(mRandomBox.at(i));
        mWindow.draw(mRandomLongBox.at(i));
        mWindow.draw(mRandomBoxWithinBox.at(i));
    }
    for (int i = 0; i < 7; i++)
    {
        //draw sample images below the palette
        mWindow.draw(mRandomLine.at(i));
    }
    
    text.setCharacterSize(15);
    text.setString("Kansei Scale Rating:");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 700));
    mWindow.draw(text);
    text.setString(mColorScheme.getDescWramth() + " \(" + std::to_string(mColorScheme.getWarmth()) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 720));
    mWindow.draw(text);
    
    text.setString(mColorScheme.getDescSoftness() + " \(" + std::to_string(mColorScheme.getSoftness()) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 740));
    mWindow.draw(text);
    
    text.setString(mColorScheme.getDescNaturalness() + " \(" + std::to_string(mColorScheme.getNaturalness()) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 760));
    mWindow.draw(text);
    
    text.setString(mColorScheme.getDescActiveness() + " \(" + std::to_string(mColorScheme.getActiveness()) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 780));
    mWindow.draw(text);
    
    text.setString(mColorScheme.getDescBrightness() + " \(" + std::to_string(mColorScheme.getBrightness()) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 800));
    mWindow.draw(text);
    
    text.setCharacterSize(10);
    
    text.setString("Chomaticness Average: (" + std::to_string(mColorScheme.mChromaticnessAverage) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 820));
    mWindow.draw(text);
    
    text.setString("Lightness Average: (" + std::to_string(mColorScheme.mLightnessAverage) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 835));
    mWindow.draw(text);    
    
    text.setString("Lightness Chromaticness Average: (" + std::to_string(mColorScheme.mLightnessChromaticnessAverage) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 850));
    mWindow.draw(text);    
    
    text.setString("Lightness Distributed Degree: (" + std::to_string(mColorScheme.mLightnessDistributedDegree) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 865));
    mWindow.draw(text);        
    
    text.setString("Distance Distributed Degree: (" + std::to_string(mColorScheme.mDistanceDistributedDegree) + ")");
    text.setPosition(sf::Vector2f(startingXPos + 10, startingYPos + 880));
    mWindow.draw(text);        
    
}

void CIECAMViewer::setRandomBoxes()
{
    float startingXPos {1250};
    float startingYPos {140};
    
    int dice1 = rand() % 5;
    
    std::vector<bool> isColorSet(5, false);
    
    mRandomBox.at(0).setPosition(sf::Vector2f(startingXPos, startingYPos));
    mRandomBox.at(0).setSize(sf::Vector2f(265, 265));
    mRandomBox.at(0).setFillColor(sf::Color(mColorScheme.colorSet.at(dice1).R, mColorScheme.colorSet.at(dice1).G, mColorScheme.colorSet.at(dice1).B));
    isColorSet.at(dice1) = true;
    
    int dice2 = rand() % 4;
    int dice3 = rand() % 3;
    int dice4 = rand() % 2;
    
    int randPosX;
    int randPosY;
    int randSizeX;
    int randSizeY;
    
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice2 == 0)
            {
                randPosX = rand() % 132;
                randPosY = rand() % 132;
                randSizeX = rand() % 30 + 102;
                randSizeY = rand() % 30 + 102;
                mRandomBox.at(1).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
                mRandomBox.at(1).setSize(sf::Vector2f(randSizeX, randSizeY));
                mRandomBox.at(1).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice2--;
            
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice3 == 0)
            {
                randPosX = rand() % 132;
                randPosY = rand() % 132;
                randSizeX = rand() % 50 + 82;
                randSizeY = rand() % 50 + 82;
                mRandomBox.at(2).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
                mRandomBox.at(2).setSize(sf::Vector2f(randSizeX, randSizeY));
                mRandomBox.at(2).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice3--;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice4 == 0)
            {
                randPosX = rand() % 132;
                randPosY = rand() % 132;
                randSizeX = rand() % 70 + 62;
                randSizeY = rand() % 70 + 62;
                mRandomBox.at(3).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
                mRandomBox.at(3).setSize(sf::Vector2f(randSizeX, randSizeY));
                mRandomBox.at(3).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice4--;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            randPosX = rand() % 132;
            randPosY = rand() % 132;
            randSizeX = rand() % 90 + 42;
            randSizeY = rand() % 90 + 42;
            mRandomBox.at(4).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
            mRandomBox.at(4).setSize(sf::Vector2f(randSizeX, randSizeY));
            mRandomBox.at(4).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
            isColorSet.at(i) = true;
        }
    }     
}
void CIECAMViewer::setRandomLongBoxes()
{
    float startingXPos {1525};
    float startingYPos {140};
    
    int dice1 = rand() % 5;
    
    std::vector<bool> isColorSet(5, false);
    
    mRandomLongBox.at(0).setPosition(sf::Vector2f(startingXPos, startingYPos));
    mRandomLongBox.at(0).setSize(sf::Vector2f(265, 265));
    mRandomLongBox.at(0).setFillColor(sf::Color(mColorScheme.colorSet.at(dice1).R, mColorScheme.colorSet.at(dice1).G, mColorScheme.colorSet.at(dice1).B));
    isColorSet.at(dice1) = true;
    
    int dice2 = rand() % 4;
    int dice3 = rand() % 3;
    int dice4 = rand() % 2;
    
    int randPosX;
    int randPosY;
    int randSizeX;
    int randSizeY;
    
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice2 == 0)
            {
                randPosX = rand() % 204;
                randPosY = rand() % 82;
                randSizeX = rand() % 30 + 30;
                randSizeY = rand() % 30 + 152;
                mRandomLongBox.at(1).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
                mRandomLongBox.at(1).setSize(sf::Vector2f(randSizeX, randSizeY));
                mRandomLongBox.at(1).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice2--;
            
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice3 == 0)
            {
                randPosX = rand() % 82;
                randPosY = rand() % 204;
                randSizeX = rand() % 30 + 152;
                randSizeY = rand() % 30 + 30;
                mRandomLongBox.at(2).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
                mRandomLongBox.at(2).setSize(sf::Vector2f(randSizeX, randSizeY));
                mRandomLongBox.at(2).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice3--;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice4 == 0)
            {
                randPosX = rand() % 204;
                randPosY = rand() % 82;
                randSizeX = rand() % 30 + 30;
                randSizeY = rand() % 30 + 152;
                mRandomLongBox.at(3).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
                mRandomLongBox.at(3).setSize(sf::Vector2f(randSizeX, randSizeY));
                mRandomLongBox.at(3).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice4--;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            randPosX = rand() % 82;
            randPosY = rand() % 204;
            randSizeX = rand() % 30 + 152;
            randSizeY = rand() % 30 + 30;
            mRandomLongBox.at(4).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos + randPosY));
            mRandomLongBox.at(4).setSize(sf::Vector2f(randSizeX, randSizeY));
            mRandomLongBox.at(4).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
            isColorSet.at(i) = true;
        }
    }    
}

void CIECAMViewer::setRandomLines()
{
    float startingXPos {1250};
    float startingYPos {415};
    
    int randColor = rand() % 5;
    
    mRandomLine.at(0).setPosition(sf::Vector2f(startingXPos, startingYPos));
    mRandomLine.at(0).setSize(sf::Vector2f(265, 265));
    mRandomLine.at(0).setFillColor(sf::Color(mColorScheme.colorSet.at(randColor).R, mColorScheme.colorSet.at(randColor).G, mColorScheme.colorSet.at(randColor).B));
    int bgColor = randColor;
    
    int randPosX;
    int randPosY;
    int randSizeX;
    int randSizeY;
    
    for (int i = 1; i < 7; i = i + 2)
    {
        randPosX = rand() % 230;
        randSizeX = rand() % 30 + 5;
        randColor = rand() % 4;
        if (randColor >= bgColor)
            randColor++;
        
        mRandomLine.at(i).setPosition(sf::Vector2f(startingXPos + randPosX, startingYPos));
        mRandomLine.at(i).setSize(sf::Vector2f(randSizeX, 265));
        mRandomLine.at(i).setFillColor(sf::Color(mColorScheme.colorSet.at(randColor).R, mColorScheme.colorSet.at(randColor).G, mColorScheme.colorSet.at(randColor).B));

        randPosY = rand() % 230;
        randSizeY = rand() % 30 + 5;
        randColor = rand() % 4;
        
        if (randColor >= bgColor)
            randColor++;
        
        mRandomLine.at(i + 1).setPosition(sf::Vector2f(startingXPos, startingYPos + randPosY));
        mRandomLine.at(i + 1).setSize(sf::Vector2f(265, randSizeY));
        mRandomLine.at(i + 1).setFillColor(sf::Color(mColorScheme.colorSet.at(randColor).R, mColorScheme.colorSet.at(randColor).G, mColorScheme.colorSet.at(randColor).B));
    }
}

void CIECAMViewer::setRandomBoxesWithinBoxes()
{
    float startingXPos {1525};
    float startingYPos {415};
    
    int dice1 = rand() % 5;
    
    std::vector<bool> isColorSet(5, false);
    
    mRandomBoxWithinBox.at(0).setPosition(sf::Vector2f(startingXPos, startingYPos));
    mRandomBoxWithinBox.at(0).setSize(sf::Vector2f(265, 265));
    mRandomBoxWithinBox.at(0).setFillColor(sf::Color(mColorScheme.colorSet.at(dice1).R, mColorScheme.colorSet.at(dice1).G, mColorScheme.colorSet.at(dice1).B));
    isColorSet.at(dice1) = true;
    
    int dice2 = rand() % 4;
    int dice3 = rand() % 3;
    int dice4 = rand() % 2;
    
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice2 == 0)
            {
                mRandomBoxWithinBox.at(1).setPosition(sf::Vector2f(startingXPos + 30, startingYPos + 30));
                mRandomBoxWithinBox.at(1).setSize(sf::Vector2f(265 - 60, 265 - 60));
                mRandomBoxWithinBox.at(1).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice2--;
            
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice3 == 0)
            {
                mRandomBoxWithinBox.at(2).setPosition(sf::Vector2f(startingXPos + 40, startingYPos + 40));
                mRandomBoxWithinBox.at(2).setSize(sf::Vector2f(265 - 80, 265 - 80));
                mRandomBoxWithinBox.at(2).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice3--;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            if (dice4 == 0)
            {
                mRandomBoxWithinBox.at(3).setPosition(sf::Vector2f(startingXPos + 80, startingYPos + 80));
                mRandomBoxWithinBox.at(3).setSize(sf::Vector2f(265 - 160, 265 - 160));
                mRandomBoxWithinBox.at(3).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
                isColorSet.at(i) = true;
            }
            dice4--;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        if (!isColorSet.at(i))
        {
            mRandomBoxWithinBox.at(4).setPosition(sf::Vector2f(startingXPos + 90, startingYPos + 90));
            mRandomBoxWithinBox.at(4).setSize(sf::Vector2f(265 - 180, 265 - 180));
            mRandomBoxWithinBox.at(4).setFillColor(sf::Color(mColorScheme.colorSet.at(i).R, mColorScheme.colorSet.at(i).G, mColorScheme.colorSet.at(i).B));
            isColorSet.at(i) = true;
        }
    }
}
