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

#include "pigmentlocator.h"

#include <fstream>
#include <algorithm>

PigmentLocator::PigmentLocator(ViewingCondition& vc) 
{
    Color color(vc);
    findPigment(color);
}

void PigmentLocator::loadDataFromFile(const std::string& fileName)
{
    std::ifstream inFile(fileName);
    nlohmann::json jsonDatabase;
    inFile >> jsonDatabase;
      
    for (const auto& element : jsonDatabase)
    {
        std::string pigmentCode{""};
        std::string otherPigment{""};
        std::string commonName{""};
        std::string alias{""};
        std::string chemical{""};
        std::string note{""};
        
        if (!element["pigment"].is_null())
            pigmentCode = element["pigment"];
        if (!element["other_pigment"].is_null())
            otherPigment = element["other_pigment"];
        if (!element["common_name"].is_null())
            commonName = element["common_name"];
        if (!element["alias"].is_null())
            alias = element["alias"];
        if (!element["chemical"].is_null())
            chemical = element["chemical"];
        if (!element["note"].is_null())
            note = element["note"];
        
        if (!element["ciecam"]["J"].is_null() && !element["ciecam"]["a"].is_null() && !element["ciecam"]["b"].is_null())
        {
            Pigment pigment;
            pigment.pigmentCode = pigmentCode;
            pigment.otherPigment = otherPigment;
            pigment.commonName = commonName;
            pigment.alias = alias;
            pigment.chemical = chemical;
            pigment.note = note;
            pigment.J = element["ciecam"]["J"];
            pigment.a = element["ciecam"]["a"];
            pigment.b = element["ciecam"]["b"];
            pigment.distanceFromTarget = 0;
            
            pigmentList.push_back(pigment);
        }
   }
}

void PigmentLocator::findPigment(Color& color)
{
    for (Pigment& pigment : pigmentList)
    {
       pigment.distanceFromTarget = color.findDistanceFromColor(pigment.J, pigment.a, pigment.b );
    }
    
   sort(pigmentList.begin(), pigmentList.end(), Pigment::sortByDistance);
}
