#include "generator.hpp"


Coords randomCoord(int radius, Coords start)
{
    int x = RandomI(-radius, radius);
    return Coords(x, RandomI(std::max(-x, 0) - radius, std::min(-x, 0) + radius)) + start;
}

int randomFromDist(int min, int step, std::vector<float> distribution)
{
    float roll = RandomF(0.001f, 1.f, 0.001f);
    float total = 0.f;

    for(int i = 0; i < distribution.size(); ++i)
    {
	total += distribution[i];
	if(roll <= total)
	{
	    //std::cout << roll << " = " << min + step*i << std::endl;
	    return min + step*i;
	}
    }
}

Coords middle(hexSettings& hSetts, Coords start, Coords end)
{
    sf::Vector2i point = sf::Vector2i(coordsToCenter(hSetts, start) +
				      (coordsToCenter(hSetts, end) -
				       coordsToCenter(hSetts, start))/2);
    return pixelToCoords(hSetts, point);
}

void addToMap(std::map<Coords, Hex>& result, std::map<Coords, Hex> toAdd, hexSettings& hSetts)
{
    std::map<Coords, Hex>::iterator found;
    for(auto it = toAdd.begin(); it != toAdd.end(); ++it)
    {
	found = result.find(it->first);
	if(found == result.end())
	{
	    result.insert(std::pair<Coords, Hex> (it->first, Hex(hSetts, it->first)));
	}
	result.at(it->first).setType(it->second.getType());
	result.at(it->first).setHeight(std::max(result.at(it->first).getHeight(),
						it->second.getHeight()));
	result.at(it->first).setDrainage(std::max(result.at(it->first).getDrainage(),
						  it->second.getDrainage()));
    }
}

void initArea(hexSettings& hSetts, std::map<Coords, Hex>& result, Coords pos, int radius)
{
    for(int ring = 0; ring <= radius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0) result.insert(std::pair<Coords, Hex>(pos, {hSetts, pos, 2, 0, 0, 0, 0}));
	    for(int i = 0; i < ring; ++i)
	    {
		result.insert(std::pair<Coords, Hex>(pos, {hSetts, pos, 2, 0, 0, 0, 0}));
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);
    }
}

std::map<Coords, Hex> createRidge(
       hexSettings& hSetts, genSettings& gSetts, Coords start, Coords end)
{
    //getchar();
	 
    std::map<Coords, Hex> result;
    Coords middle = {0, 0};
	 
    if(length(distance(start, end)) > 1)
    {
	// converting coords into pixels for more natural curves
	sf::Vector2i startPixel = coordsToCenter(hSetts, start);
	sf::Vector2i endPixel = coordsToCenter(hSetts, end);
	sf::Vector2i middlePixel = startPixel + (endPixel - startPixel)/2;

	//std::cout << "(" <<
	//		startPixel.x << ", " << startPixel.y << ") | (" <<
	//  	endPixel.x   << ", " << endPixel.y   << ")" << std::endl;
				
	sf::Vector2i variation;

	do{
	    // curving ridges
	    variation.x = RandomI(std::round(length(distance(start, end))
					     * (hSetts.hexWidth -  hSetts.hexQuarter)
					     * -gSetts.deviation),
				  std::round(length(distance(start, end))
					     * (hSetts.hexWidth -  hSetts.hexQuarter)
					     *  gSetts.deviation));
	    variation.y = RandomI(std::round(length(distance(start, end))
					     * hSetts.hexHeight * -gSetts.deviation),
				  std::round(length(distance(start, end))
					     * hSetts.hexHeight *  gSetts.deviation));

	    middlePixel += variation;
	    middle = centerToCoords(hSetts, middlePixel);

	    //checking if in bounds
	}while(length(middle) > gSetts.mapRadius || middle == start || middle == end);
		  
	result.insert(std::pair<Coords, Hex>(middle, {hSetts, middle, 3, 0}));

	// recurrently generate next
	addToMap(result, createRidge(hSetts, gSetts, start , middle), hSetts);
	addToMap(result, createRidge(hSetts, gSetts, middle, end   ), hSetts);
    }

    return result;
}

std::map<Coords, Hex> generateMntRanges(hexSettings& hSetts, genSettings& gSetts,
					std::map<Coords, Hex> mountains)
{
    std::map<Coords, Hex> result;
    std::priority_queue<std::pair<Coords, int>, std::vector<std::pair<Coords, int>>, secondComp>
	toExamine;
    std::map<Coords, bool> visited;
    Coords examined(0, 0);
    Coords neighbour(0, 0);
    Coords pos(0, 0);
    int height;

    for(auto it = mountains.begin(); it != mountains.end(); ++it)
    {
	result.insert(std::pair<Coords, Hex>
		      (it->first, {hSetts, it->first, 3, gSetts.mountainHeight, 0, 0, 99}));
	toExamine.emplace(std::pair<Coords, int>(it->first, gSetts.mountainHeight));
    }

    while(toExamine.size() > 0)
    {
	examined = toExamine.top().first;
	//std::cout << result.at(examined).getHeight();
	toExamine.pop();

	if(visited.find(examined) == visited.end())
	{
	    for(int i = 0; i < 6; ++i)
	    {
		neighbour = examined + direction(i);
		if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end())
		{
		    height = randomFromDist(0, 1, gSetts.heightProb[result.at(examined).getHeight()]);
						 
		    if(result.find(neighbour) != result.end())
		    {
			result.at(neighbour).setHeight(
			       std::max(result.at(neighbour).getHeight(), height));
		    }
		    else result.insert(std::pair<Coords, Hex>(neighbour,
							      {hSetts, neighbour, 2, height}));
					 
		    if(result.at(neighbour).getHeight() == gSetts.mountainHeight)
		    {
			result.at(neighbour).setType(3);
			result.at(neighbour).setDrainage(99);
		    }
		    if(result.at(neighbour).getHeight() > 0)
		    {
			toExamine.emplace(neighbour, result.at(neighbour).getHeight());
		    }
		}
	    }
	    visited[examined] = true;
	}
    }
	 
    return result;
}

void generateMountains(hexSettings& hSetts, genSettings& gSetts, std::map<Coords, Hex>& result)
{
    std::map<Coords, Hex> mountains;
    Coords start(0, 0);
    Coords end(0, 0);
    int count = 0;

    for(int i = 0; i < gSetts.TORidges; ++i)
    {
	// randomizing start and end points until they're close enough
	do{
	    start = randomCoord(gSetts.mountainRadius, gSetts.start);
	    end   = randomCoord(gSetts.mountainRadius, gSetts.start);
	}while(length(distance(start, end)) > gSetts.maxRidgeLength);
				
	//start.print();
	//end.print();
	addToMap(mountains, createRidge(hSetts, gSetts, start, end), hSetts);
    }
	 
    addToMap(result, generateMntRanges(hSetts, gSetts, mountains), hSetts);
}

std::vector<Coords> getNeighbours(std::map<Coords, Hex>& map, Coords start, int radius)
{
    std::vector<Coords> result;
	 
    for(int i = 0; i < 6; ++i)
    {
	if(length(start + direction(i)) <= radius)
	{
	    result.emplace_back(start + direction(i));
	}
    }

    return result;
}

std::vector<int> countNeighbours(std::map<Coords, Hex>& map, Coords start, int radius)
{
    std::vector<int> result(6, 0);
    std::vector<Coords> neighbours = getNeighbours(map, start, radius);
	 
    for(int i = 0; i < neighbours.size(); ++i)
    {
	++result[map.at(neighbours[i]).getType()];
    }

    return result;
}

void generateNoiseHeight(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    FastNoise heightNoise(gSetts.heightSeed);
    heightNoise.SetNoiseType(FastNoise::SimplexFractal);
    heightNoise.SetFrequency(gSetts.heightFreq);
    heightNoise.SetFractalOctaves(gSetts.heightOcts);
    int fromCenter;
    float temp;

    for(int y = -gSetts.mapRadius; y <= gSetts.mapRadius; ++y)
    {
	for(int x = std::max(-y, 0) - gSetts.mapRadius; x <= std::min(-y, 0) + gSetts.mapRadius; ++x)
	{
	    temp = heightNoise.GetNoise(
		   ((float)x + gSetts.mapRadius)/(gSetts.mapRadius * 2 + 1),
		   ((float)y + gSetts.mapRadius)/(gSetts.mapRadius * 2 + 1))
		* gSetts.heightAmplitude;
				
	    fromCenter = length(distance(Coords(x, y), gSetts.start));

	    if(fromCenter <= gSetts.landRadius)
	    {
		temp += gSetts.heightAtLand;
	    }
	    else
	    {
		temp += (1.f - (((float)fromCenter - gSetts.landRadius)
				/ (gSetts.mapRadius - gSetts.landRadius))) *
		    (gSetts.heightAtLand - gSetts.heightAtSea) + gSetts.heightAtSea;
	    }
				
	    result.at(Coords(x, y)).setHeight((int)std::round(temp));
					 
	}
    }
}

void generateNoiseMoisture(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    FastNoise moistureNoise(gSetts.moistureSeed);
    moistureNoise.SetNoiseType(FastNoise::SimplexFractal);
    moistureNoise.SetFrequency(gSetts.moistureFreq);
    moistureNoise.SetFractalOctaves(gSetts.moistureOcts);
    float temp;

    for(int y = -gSetts.mapRadius; y <= gSetts.mapRadius; ++y)
    {
	for(int x = std::max(-y, 0) - gSetts.mapRadius; x <= std::min(-y, 0) + gSetts.mapRadius; ++x)
	{
	    temp = moistureNoise.GetNoise(
		   ((float)x + gSetts.mapRadius)/(gSetts.mapRadius * 2 + 1),
		   ((float)y + gSetts.mapRadius)/(gSetts.mapRadius * 2 + 1))
		* gSetts.moistureAmplitude;
				
	    result.at(Coords(x, y)).setMoisture(std::clamp(result.at(Coords(x, y)).getMoisture()
							   + (int)std::round(temp), 0, 99));
					 
	}
    }
}

void generateShape(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    Coords pos = gSetts.start;

    for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0)
	    {
		if(result.at(pos).getHeight() < 0)
		{
		    result.at(pos).setType(1);
		    result.at(pos).setHeight(0);
		}
		else result.at(pos).setType(2);
	    }
				
	    for(int i = 0; i < ring; ++i)
	    {
		if(result.at(pos).getHeight() < 0)
		{
		    result.at(pos).setType(1);
		    result.at(pos).setHeight(0);
		}
		else result.at(pos).setType(2);
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);		  
    }
}

bool closerToZero(const int& a, const int& b)
{
    return (abs(a) < abs(b));
}

std::vector<Coords> considerLake(genSettings& gSetts, std::map<Coords, Hex>& result, Coords from)
{
    std::queue<Coords> toExamine;
    std::map<Coords, bool> visited;
    std::vector<Coords> lake;
    Coords examined;
    Coords neighbour;
    bool fall = false;

    toExamine.push(from);
    visited[from] = true;

    while(lake.size() <= gSetts.maxLakeSize && toExamine.size() > 0 && !fall)
    {
	examined = toExamine.front();
	lake.push_back(examined);
	toExamine.pop();

	fall = false;

	for(int i = 0; i < 6; ++i)
	{
	    neighbour = examined + direction(i);
	    if(result.at(neighbour).getHeight() < result.at(examined).getHeight()) fall = true;
	    else if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end() &&
		    result.at(neighbour).getHeight() == result.at(examined).getHeight())
	    {
		toExamine.push(neighbour);
		visited[neighbour] = true;
	    }
	    if(fall) break;
	}
    }

    if(fall || lake.size() > gSetts.maxLakeSize) lake.clear();
    return lake;
}

void generateRivers(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    Coords start(0, 0);
    Coords pos(0, 0);
    Coords temp(0, 0);
    int dir = RandomI(0, 5);
    int backstep = 0;
    int cutsSoFar = 0;
    std::vector<int> river;
    std::vector<Coords> lake;
    std::vector<Coords> toBeLakes;
    std::vector< std::vector<int> > heights(7, std::vector<int>(0));
    std::map<Coords, bool> visited;
    std::map<Coords, bool> inRiver;
    std::vector<Coords> riverSources; 
    Coords neighbour(0, 0);
    bool illegalRiver = false; // river that has to rise or pass through mountains
    bool metOther = false; // met other river
    bool lakeBetween = false; // found lake during de-meandering
    bool newStart = false;
    //std::vector<Coords> neighbours;

    for(int i = 0; i < gSetts.TORivers; ++i)
    {
	do{
	    river.clear();
	    toBeLakes.clear();
	    do{
		newStart = false;
		start = randomCoord(gSetts.coastline);
		if(result.at(start).getType() != 3 ||
		   result.at(start).getWater().size() > 0)
		    newStart = true;

		if(!newStart)
		{
		    for(int i = 0; i < riverSources.size(); ++i)
		    {
			if(length(distance(riverSources[i], start)) <= gSetts.minRiverSourceDistance)
			{
			    newStart = true;
			    break;
			}
		    }
		}
	    }while(newStart);

	    pos = start;
	    cutsSoFar = 0;

	    visited.clear();
	    inRiver.clear();
	    toBeLakes.clear();

	    do{
		illegalRiver = false;
		metOther = false;
		visited[pos] = true;
		result.at(pos).setTemperature(9);
		inRiver[pos] = true;

		if(RandomF(0.001f, 1.0f, 0.001f) <= gSetts.meanderFreq)
		{
		    if(RandomB()) dir = (dir + 1) % 6;
		    else dir = (dir + 5) % 6;
		}
				
		for(int j = 0; j <= 6; ++j)
		{
		    heights[j].clear();
		}
				
		for(int j = 0; j < 6; ++j)
		{
		    neighbour = pos + direction(j);

		    if(length(neighbour) <= gSetts.mapRadius) // in range
		    {
			if(inRiver.find(neighbour) != inRiver.end() && inRiver[neighbour] == true)
			{
			    if((j + 3) % 6 != river[river.size()-1])
			    {
				temp = pos; 
				lakeBetween = false;
				backstep = 0;
				if(toBeLakes.size() > 0)
				{
				    while(temp != neighbour)
				    {
					++backstep;
					if(temp == toBeLakes.back())
					{
					    lakeBetween = true;
					    break;
					}
					temp += direction((river[river.size() - backstep] + 3) % 6);
				    }
				}

				if(!lakeBetween)
				{
				    ++cutsSoFar;
				    if(cutsSoFar > gSetts.maxRiverCuts)
				    {
					illegalRiver = true;
					break;
				    }
												
				    temp = pos;
				    while(temp != neighbour)
				    {
					inRiver[temp] = false;
					visited[temp] = false;
					temp += direction((river[river.size()-1] + 3) % 6);
					river.pop_back();
				    }
				    inRiver[pos] = true;
				    river.push_back((j + 3) % 6);
				}
			    }
			}
								
			if((visited.find(neighbour) == visited.end() || visited[neighbour] == false) &&
			   result.at(neighbour).getType() != 3 && // not mountains
			   (result.at(neighbour).getHeight() <= result.at(pos).getHeight() || // not higher,
			    considerLake(gSetts, result, pos).size() != 0)) // unless a lake is possible
			{
			    // prioritazing oceans, lakes and rivers
			    if(result.at(neighbour).getType() <= 1 ||
			       result.at(neighbour).getWater().size() > 1)
				heights[0].emplace_back(j - dir);
			    else heights[result.at(neighbour).getHeight() + 1].emplace_back(j - dir);
			}
		    }
		}

		if(!illegalRiver)
		{
		    illegalRiver = true;
		    for(int j = 0; j <= 6; ++j)
		    {
			if(heights[j].size() > 0)
			{
			    illegalRiver = false;
			    heights[j] = Choose(heights[j], heights[j].size()); // randomize the order
			    sort(heights[j].begin(), heights[j].end(), closerToZero);
			    // order based on deviation from the direction
			    dir += heights[j][0];

			    // if choice needs a lake, declare one
			    if(j-1 > result.at(pos).getHeight()) toBeLakes.push_back(pos);
								
			    river.emplace_back(dir);
			    pos += direction(dir);
			    break;
			}
		    }
		    if(result.at(pos).getWater().size() > 1) metOther = true; // met other river
		    if(illegalRiver == true && river.size() > 0) // illegal river, but maybe fixable
		    {
			illegalRiver = false;
			inRiver[pos] = false;
			if(toBeLakes.size() > 0 && pos == toBeLakes.back()) toBeLakes.pop_back();
			pos += direction((river[river.size() - 1] + 3) % 6);
			river.pop_back();
		    }
		}
					 
	    }while(result.at(pos).getType() != 0 && illegalRiver == false && metOther == false);
	}while(illegalRiver == true);

	for(int i = 0; i < toBeLakes.size(); ++i)
	{
	    // creating lakes
	    lake = considerLake(gSetts, result, toBeLakes[i]);

	    for(int j = 0; j < lake.size(); ++j)
	    {
		result.at(lake[j]).setType(1);
		result.at(lake[j]).setHeight(result.at(lake[j]).getHeight() + 1);
	    }
	}

	riverSources.push_back(start);
	pos = start;
	for(int i = 0; i < river.size(); ++i)
	{
	    if(result.at(pos).getType() > 1 && i != 0) result.at(pos).addWater(river[i]);
	    pos += direction(river[i]);
	    if(result.at(pos).getType() > 1) result.at(pos).addWater((river[i] + 3) % 6);
	}
    }
}

void markOceans(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    std::queue<Coords> toExamine;
    std::map<Coords, bool> visited;
    Coords pos = direction(4) * gSetts.mapRadius;
    Coords neighbour;
	 
    for(int turn = 0; turn < 6; ++turn)
    {
	for(int i = 0; i < gSetts.mapRadius; ++i)
	{
	    result.at(pos).setType(0);
	    result.at(pos).setHeight(0);
	    toExamine.push(pos);
				
	    pos += direction(turn);
	}
    }

    while(toExamine.size() > 0)
    {
	pos = toExamine.front();
	toExamine.pop();
	visited[pos] = true;

	for(int i = 0; i < 6; ++i)
	{
	    neighbour = pos + direction(i);

	    if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end() &&
	       result.at(neighbour).getHeight() < 0)
	    {
		toExamine.push(neighbour);
		result.at(neighbour).setType(0);
		result.at(neighbour).setHeight(0);
	    }
	}
    }
}

void generateMoisture(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    std::priority_queue<std::pair<Coords, int>, std::vector<std::pair<Coords, int>>, secondComp>
	currentEffect;
    std::priority_queue<std::pair<Coords, int>, std::vector<std::pair<Coords, int>>, secondComp>
	riverEffect;
    std::map<Coords, bool> visited;
    Coords neighbour;
    Coords pos = gSetts.start;
    std::pair<Coords, int> examined;
    int temp;
    int currentDir = RandomI(0, 5);
	 
    for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0)
	    {
		if(result.at(pos).getType() == 0)
		{
		    currentEffect.push(std::pair<Coords, int>(pos, gSetts.moistAtSea));
		    visited[pos] = true;
		    result.at(pos).setMoisture(gSetts.moistAtSea);
		}
		else if(result.at(pos).getType() == 1 || result.at(pos).getWater().size() > 0)
		{
		    riverEffect.push(std::pair<Coords, int>(pos, gSetts.moistAtRiver));
		}
	    }
	    for(int i = 0; i < ring; ++i)
	    {
		if(result.at(pos).getType() == 0)
		{
		    currentEffect.push(std::pair<Coords, int>(pos, gSetts.moistAtSea));
		    visited[pos] = true;
		    result.at(pos).setMoisture(gSetts.moistAtSea);
		}
		else if(result.at(pos).getType() == 1 || result.at(pos).getWater().size() > 0)
		{
		    riverEffect.push(std::pair<Coords, int>(pos, gSetts.moistAtRiver));
		}
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);
    }

    while(currentEffect.size() > 0)
    {
	examined = currentEffect.top();
	pos = examined.first;
	currentEffect.pop();
	visited[pos] = true;

	neighbour = pos + direction(currentDir);

	if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end() &&
	   result.at(neighbour).getType() != 3)
	{
	    temp = std::max(result.at(pos).getMoisture() - gSetts.moistLostPerHeight *
			    (result.at(neighbour).getHeight() + 1),
			    result.at(neighbour).getMoisture());
	    result.at(neighbour).setMoisture(temp);
	    currentEffect.push(std::pair<Coords, int>(neighbour, temp));
	}
    }

    visited.clear();
    while(riverEffect.size() > 0)
    {
	examined = riverEffect.top();
	pos = examined.first;
	riverEffect.pop();
		  
	if(result.at(pos).getType() == 1)
	{
	    result.at(pos).setMoisture(std::max(gSetts.moistAtLake, result.at(pos).getMoisture()));
	}
	else if(result.at(pos).getWater().size() > 0)
	{
	    result.at(pos).setMoisture(std::max(gSetts.moistAtRiver, result.at(pos).getMoisture()));
	}
		  
	if(visited.find(pos) == visited.end())
	{
	    visited[pos] = true;

	    for(int i = 0; i < 6; ++i)
	    {
		neighbour = pos + direction(i);
				
		if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end() &&
		   result.at(neighbour).getType() != 3)
		{
		    temp = std::max(result.at(pos).getMoisture() - gSetts.moistLostPerLand,
				    result.at(neighbour).getMoisture());
		    /*temp = std::round((result.at(pos).getMoisture() - gSetts.moistLostPerLand +
		      result.at(neighbour).getMoisture())/
		      (1.f + ((result.at(pos).getMoisture() - gSetts.moistLostPerLand) *
		      result.at(neighbour).getMoisture())/10000.f));*/
						  
		    result.at(neighbour).setMoisture(temp);
		    riverEffect.push(std::pair<Coords, int>(neighbour, temp));
		}
	    }
	}
    }
}

void generateDrainage(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    std::vector< std::queue<Coords> > toExamine(gSetts.mountainHeight);
    std::map<Coords, bool> visited;
    Coords neighbour;
    Coords pos = gSetts.start;
    Coords examined;
    int temp;

    for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0 && result.at(pos).getType() == 0 || result.at(pos).getType() == 1 ||
	       result.at(pos).getWater().size() > 0)
	    {
		toExamine[result.at(pos).getHeight()].push(pos);
		//visited[pos] = true;
		result.at(pos).setDrainage(99);
	    }
	    for(int i = 0; i < ring; ++i)
	    {
		if(result.at(pos).getType() == 0 || result.at(pos).getType() == 1 ||
		   result.at(pos).getWater().size() > 0)
		{
		    toExamine[result.at(pos).getHeight()].push(pos);
		    //visited[pos] = true;
		    result.at(pos).setDrainage(99);
		}
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);
    }

    for(int i = 0; i < gSetts.mountainHeight; ++i)
    {
	while(toExamine[i].size() > 0)
	{
	    //std::cout << toExamine.size() << std::endl;
	    examined = toExamine[i].front();
	    toExamine[i].pop();
		  
	    if(visited.find(examined) == visited.end())
	    {
		visited[examined] = true;

		for(int i = 0; i < 6; ++i)
		{
		    neighbour = examined + direction(i);
				
		    if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end() &&
		       result.at(neighbour).getType() != 3 &&
		       result.at(neighbour).getHeight() >= result.at(examined).getHeight())
		    {
			if(result.at(neighbour).getHeight() > result.at(examined).getHeight()) temp = 99;
			else
			{
			    temp = std::max((int)std::round(result.at(examined).getDrainage()
							    * gSetts.drainLostPerLand),
					    result.at(neighbour).getDrainage());
			}
			result.at(neighbour).setDrainage(temp);
			toExamine[result.at(neighbour).getHeight()].push(neighbour);
		    }
		}
	    }
	}
    }
}

void generateDeserts(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    Coords pos = gSetts.start;
	 
    for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0 && result.at(pos).getType() == 2 && result.at(pos).getMoisture() <= 1)
	    {
		result.at(pos).setType(6);
	    }
	    for(int i = 0; i < ring; ++i)
	    {
		if(result.at(pos).getType() == 2 && result.at(pos).getMoisture() <= 10)
		{
		    result.at(pos).setType(6);
		}
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);
    }
}

void generateMarshes(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    Coords pos = gSetts.start;
	 
    for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0 && result.at(pos).getType() == 2 && result.at(pos).getMoisture() >= 50 &&
	       result.at(pos).getDrainage() <= 35)
	    {
		result.at(pos).setType(5);
	    }
	    for(int i = 0; i < ring; ++i)
	    {
		if(result.at(pos).getType() == 2 && result.at(pos).getMoisture() >= 50 &&
		   result.at(pos).getDrainage() <= 35)
		{
		    result.at(pos).setType(5);
		}
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);
    }
}

void generateForests(genSettings& gSetts, std::map<Coords, Hex>& result)
{
    Coords pos = gSetts.start;
	 
    for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
    {
	for(int turn = 0; turn < 6; ++turn)
	{
	    if(ring == 0 && result.at(pos).getType() == 2 && result.at(pos).getWater().size() == 0 &&
	       result.at(pos).getMoisture() >= 65 && result.at(pos).getDrainage() > 35)
	    {
		result.at(pos).setType(4);
	    }
	    for(int i = 0; i < ring; ++i)
	    {
		if(result.at(pos).getType() == 2 && result.at(pos).getWater().size() == 0 &&
		   result.at(pos).getMoisture() >= 65 && result.at(pos).getDrainage() > 35)
		{
		    result.at(pos).setType(4);
		}
					 
		pos += direction(turn);
	    }
	}
	pos += direction(4);
    }
}

void generate(std::map<Coords, Hex>& result, hexSettings& hSetts, genSettings& gSetts)
{		  
    initArea(hSetts, result, gSetts.start, gSetts.mapRadius);

    generateNoiseHeight(gSetts, result);

    //generateShape(gSetts, result);

    generateMountains(hSetts, gSetts, result);

    markOceans(gSetts, result);

    generateRivers(gSetts, result);

    generateMoisture(gSetts, result);

    generateNoiseMoisture(gSetts, result);

    generateDrainage(gSetts, result);

    generateDeserts(gSetts, result);

    generateMarshes(gSetts, result);

    generateForests(gSetts, result);
}

/*
  void generateShapeCellular(genSettings& gSetts, std::map<Coords, Hex>& result)
  {
  Coords pos = gSetts.start;

  // creating the shape
  //std::cout << "OY THERE GOVENOR" << std::endl;
  for(int ring = 0; ring <= gSetts.coastline; ++ring)
  {
  for(int turn = 0; turn < 6; ++turn)
  {
  if(ring == 0)
  {
  if(result.at(pos).getType() == 0) result.at(pos).setType(1);
  }
				
  for(int i = 0; i < ring; ++i)
  {
  if(result.at(pos).getType() == 0) result.at(pos).setType(1);
					 
  pos += direction(turn);
  }
  }
  pos += direction(4);		  
  }
	
  // building up the shape (without exceeding certain radius)
  for(int i = 0; i < gSetts.TOBuildups; ++i)
  {
  pos = gSetts.start;
  for(int ring = 0; ring <= gSetts.landRadius; ++ring)
  {
  for(int turn = 0; turn < 6; ++turn)
  {
  if(ring == 0)
  {
  if(result.at(pos).getType() == 0 &&
  RandomF(0.001f, 1.f, 0.001f) <=
  gSetts.buildupProb[countNeighbours(result, pos, gSetts.mapRadius)[1]])
  result.at(pos).setType(1);
  }
				
  for(int i = 0; i < ring; ++i)
  {
  if(result.at(pos).getType() == 0 &&
  RandomF(0.001f, 1.f, 0.001f) <=
  gSetts.buildupProb[countNeighbours(result, pos, gSetts.mapRadius)[1]])
  result.at(pos).setType(1);
					 
  pos += direction(turn);
  }
  }
  pos += direction(4);		  
  }
  }

  // eroding the shape (without touching higher hexes)
  for(int i = 0; i < gSetts.TOErosions; ++i)
  {
  pos = gSetts.start;
  for(int ring = 0; ring <= gSetts.landRadius; ++ring)
  {
  for(int turn = 0; turn < 6; ++turn)
  {
  if(ring == 0)
  {
  if(result.at(pos).getType() == 1 && result.at(pos).getHeight() == 0 &&
  RandomF(0.001f, 1.f, 0.001f) <=
  gSetts.erosionProb[countNeighbours(result, pos, gSetts.mapRadius)[0]])
  result.at(pos).setType(0);
  }
				
  for(int i = 0; i < ring; ++i)
  {
  if(result.at(pos).getType() == 1 && result.at(pos).getHeight() == 0 &&
  RandomF(0.001f, 1.f, 0.001f) <= 
  gSetts.erosionProb[countNeighbours(result, pos, gSetts.mapRadius)[0]])
  result.at(pos).setType(0);
					 
  pos += direction(turn);
  }
  }
  pos += direction(4);		  
  }
  }
  }
*/
