#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <queue>
#include <map>

#include "FastNoise.h"
#include "commons.hpp"
#include "hex.hpp"


struct genSettings
{
    int heightSeed;
    int temperatureSeed;
    int moistureSeed;
    float heightFreq;
    int heightOcts;
    float heightAmplitude;
    float heightAtSea;
    float heightAtLand;
    float moistureFreq;
    int moistureOcts;
    float moistureAmplitude;
    int mapRadius;
    int landRadius;
    int coastline;
    int mountainRadius;
    int TORidges;
    float deviation;
    int maxRidgeLength;
    int mountainHeight;
    std::vector< std::vector<float> > heightProb; // height probabilities
    int TOBuildups;
    std::vector<float> buildupProb;
    int TOErosions;
    std::vector<float> erosionProb;
    int TORivers;
    int minRiverSourceDistance;
    float meanderFreq;
    int maxRiverCuts;
    int maxLakeSize;
    int moistAtSea;
    int moistLostPerHeight;
    int moistAtLake;
    int moistAtRiver;
    int moistLostPerLand;
    float drainLostPerLand;
    Coords start;
};

Coords randomCoord(int radius, Coords start = {0, 0});

int randomFromDist(int min, int step, std::vector<float> distribution);

Coords middle(hexSettings& hSettings, Coords start, Coords end);

void addToMap(std::map<Coords, Hex>& result, std::map<Coords, Hex> toAdd, hexSettings& hSettings);

void initArea(hexSettings& hSettings, std::map<Coords, Hex>& result, Coords pos, int radius);

std::map<Coords, Hex> createRidge(
       hexSettings& hSettings, genSettings& gSettings, Coords start, Coords end);

struct heightComp
{
    bool operator()(const std::pair<Coords, int>& a, const std::pair<Coords, int>& b)
	{
	    return (a.second < b.second);
	}
};
	 
std::map<Coords, Hex> generateHeight(hexSettings& hSettings, genSettings& gSettings,
				     std::map<Coords, Hex> mountains);

std::map<Coords, Hex> generateMountains(hexSettings& hSettings, genSettings& gSettings);

std::vector<Coords> getNeighbours(std::map<Coords, Hex>& map, Coords start, int radius);

std::vector<int> countNeighbours(std::map<Coords, Hex>& map, Coords start, int radius);

void generateShape(genSettings& gSettings, std::map<Coords, Hex>& result);

bool closerToZero(const int& a, const int& b);

void generateRivers(genSettings& gSettings, std::map<Coords, Hex>& result);

void generateMarshes(genSettings& gSetts, std::map<Coords, Hex>& result);

struct secondComp
{
    bool operator()(const std::pair<Coords, int>& a, const std::pair<Coords, int>& b)
	{
	    return (a.second < b.second);
	}
};

void generate(std::map<Coords, Hex>& result, hexSettings& hSettings, genSettings& gSettings);
