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

#ifndef PIGMENTLOCATOR_H
#define PIGMENTLOCATOR_H

#include <string>
#include <vector>
#include "color.h"
#include "json.hpp"

struct Pigment
{
public:
    std::string pigmentCode{""};
    std::string otherPigment{""};
    std::string commonName{""};
    std::string alias{""};
    std::string chemical{""};
    std::string note{""};
    float J{0}, a{0}, b{0};
    float distanceFromTarget{0};
    static bool sortByDistance(const Pigment& lhs, const Pigment& rhs ) { return lhs.distanceFromTarget < rhs.distanceFromTarget; }
};


class PigmentLocator
{
public:
    PigmentLocator(ViewingCondition& vc);
    void findPigment(Color& color);
    
    std::vector<Pigment> pigmentList;

    void loadDataFromFile(const std::string& fileName);
};

#endif // PIGMENTLOCATOR_H
