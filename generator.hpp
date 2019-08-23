#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <map>

#include "tom.hpp"
#include "coords.hpp"


struct genSettings
{
	 int radius;
	 int TORidges;
	 float deviation;
	 int maxRidgeLength;
	 int mountainRadius;
	 int mountainHeight;
	 int heightRadius;
	 std::vector< std::vector<float> > heightProb; // height probabilities
	 int TORivers;
	 Coords start;
};

struct tempHex
{
	 int type;
	 int height;
	 std::vector<int> water;
};

Coords randomCoord(genSettings& gSettings);

int randomFromDist(int min, int step, std::vector<float> distribution);

Coords middle(hexSettings& hSettings, Coords start, Coords end);

void addToMap(std::map<Coords, tempHex>& result, std::map<Coords, tempHex> toAdd);

void initArea(hexSettings& hSettings, std::map<Coords, tempHex>& result, Coords pos, int radius);

std::map<Coords, tempHex> createRidge(
	 hexSettings& hSettings, genSettings& gSettings, Coords start, Coords end);

std::map<Coords, tempHex> generateMountains(genSettings& gSettings);

struct heightComp
{
	 bool operator()(const std::pair<Coords, int>& a, const std::pair<Coords, int>& b)
		  {
				return (a.second < b.second);
		  }
};
	 
std::map<Coords, tempHex> generateHeight(hexSettings& hSettings, genSettings& gSettings,
													  std::map<Coords, tempHex> mountains);

bool closerToZero(const int& a, const int& b);

void generateRivers(genSettings& gSettings, std::map<Coords, tempHex>& result,
													  std::vector<Coords>& mountains);

void generate(std::map<Coords, tempHex>& result, hexSettings& hSettings, genSettings& gSettings);
