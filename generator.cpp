#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <queue>
#include <map>

#include "FastNoise/FastNoise.h"
#include "tom.hpp"
#include "hex.hpp"
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
	 }
}

void initArea(hexSettings& hSetts, std::map<Coords, Hex>& result, Coords pos, int radius)
{
	 for(int ring = 0; ring <= radius; ++ring)
	 {
		  for(int turn = 0; turn < 6; ++turn)
		  {
				if(ring == 0) result.insert(std::pair<Coords, Hex>(pos, {hSetts, pos, 1, 0}));
				for(int i = 0; i < ring; ++i)
				{
					 result.insert(std::pair<Coords, Hex>(pos, {hSetts, pos, 1, 0}));
					 
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
							 (it->first, {hSetts, it->first, 3, gSetts.mountainHeight}));
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
								result.at(neighbour).setType(3);
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

/*std::vector<Coords> considerLake(genSettings& gSetts, std::map<Coords, Hex>& result, Coords from)
{
	 std::queue<Coords> toExamine;
	 std::map<Coords, bool> visited;
	 std::vector<CoordS> lake;
	 Coords examined;
	 Coords neighbour;

	 toExamine.push(from);
	 visited[from] = true;

	 while(lake.size() <= gSetts.maxLakeSize && toExamine.size() > 0)
	 {
		  examined = toExamine;
		  lake.push_back(examined);
		  toExamine.pop();

		  for(int i = 0; i < 6; ++i)
		  {
				neighbour = examined + direction(i);
				if(lenght(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end() &&
					result.at(neighbour).getHeight() == result.at(examined).getHeight())
				{
					 toExamine.push(neighbour);
					 visited[neighbour] = true;
				}
		  }
	 }

	 if(lake.size() > gSetts.maxLakeSize) lake.clear();
	 return lake;
}*/

void generateRivers(genSettings& gSetts, std::map<Coords, Hex>& result)
{
	 Coords start(0, 0);
	 Coords pos(0, 0);
	 int dir = RandomI(0, 5);
	 int step = 0;
	 std::vector<int> temp;
	 std::vector< std::vector<int> > heights(7, std::vector<int>(0));
	 std::map<Coords, bool> visited;
	 Coords neighbour(0, 0);
	 bool illegalRiver = false; // river that has to rise or pass through mountains
	 bool metOther = false; // met other river
	 //std::vector<Coords> neighbours;

	 for(int i = 0; i < gSetts.TORivers; ++i)
	 {
		  do{
				temp.clear();
				do{
					 start = randomCoord(gSetts.coastline);
				}while(result.at(start).getType() == 0 ||
						 result.at(start).getWater().size() > 0 ||
						 (result.at(start).getType() != 3/* &&
																		countNeighbours(result, start, gSetts.mapRadius)[4] == 0*/));

				pos = start;
				step = 0;
				
				visited.clear();

				do{
					 illegalRiver = true;
					 metOther = false;
					 visited[pos] = true;

					 if(RandomF(0.001f, 1.0f, 0.001f) <= gSetts.meanderFreq[step])
					 {
						  step = 0;
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
						  if(length(neighbour) <= gSetts.mapRadius &&
							  visited.find(neighbour) == visited.end() &&
							  result.at(neighbour).getType() != 3 &&
							  result.at(neighbour).getHeight() <= result.at(pos).getHeight())
						  {
								if(result.at(neighbour).getType() == 0) heights[0].emplace_back(j - dir);
								else heights[result.at(neighbour).getHeight() + 1].emplace_back(j - dir);
						  }
					 }

					 for(int j = 0; j <= 6; ++j)
					 {
						  if(heights[j].size() > 0)
						  {
								illegalRiver = false;
								heights[j] = Choose(heights[j], heights[j].size());
								sort(heights[j].begin(), heights[j].end(), closerToZero);
								dir += heights[j][0];
								
								temp.emplace_back(dir);
								pos += direction(dir);
								break;
						  }
					 }
					 ++step;
					 if(result.at(pos).getWater().size() > 1) metOther = true;
					 
				}while(result.at(pos).getType() != 0 && illegalRiver == false && metOther == false);
		  }while(illegalRiver == true);

		  pos = start;
		  for(int i = 0; i < temp.size(); ++i)
		  {
				if(i != 0) result.at(pos).addWater(temp[i]);
				pos += direction(temp[i]);
				if(result.at(pos).getType() > 1) result.at(pos).addWater((temp[i] + 3) % 6);
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
				   result.at(neighbour).getType() == 1)
				{
					 toExamine.push(neighbour);
					 result.at(neighbour).setType(0);
				}
		  }
	 }
}

void generateMoisture(genSettings& gSetts, std::map<Coords, Hex>& result)
{
	 std::priority_queue<std::pair<Coords, int>, std::vector<std::pair<Coords, int>>, secondComp>
		  toExamine;
	 std::map<Coords, bool> visited;
	 Coords neighbour;
	 Coords pos = gSetts.start;
	 std::pair<Coords, int> examined;
	 int temp;
	 
	 for(int ring = 0; ring <= gSetts.mapRadius; ++ring)
	 {
		  for(int turn = 0; turn < 6; ++turn)
		  {
				if(ring == 0)
				{
					 if(result.at(pos).getType() == 0 || result.at(pos).getType() == 1 ||
						 result.at(pos).getWater().size() > 0)
					 {
						  toExamine.push(std::pair<Coords, int>(pos, 9));
						  visited[pos] = true;
						  result.at(pos).setMoisture(9);
					 }
				}
				for(int i = 0; i < ring; ++i)
				{
					 if(result.at(pos).getType() == 0 || result.at(pos).getType() == 1 ||
						 result.at(pos).getWater().size() > 0)
					 {
						  toExamine.push(std::pair<Coords, int>(pos, 9));
						  visited[pos] = true;
						  result.at(pos).setMoisture(9);
					 }
					 
					 pos += direction(turn);
				}
		  }
		  pos += direction(4);
	 }

	 while(toExamine.size() > 0)
	 {
		  examined = toExamine.top();
		  pos = examined.first;
		  toExamine.pop();
		  visited[pos] = true;

		  for(int i = 0; i < 6; ++i)
		  {
				neighbour = pos + direction(i);

				if(length(neighbour) <= gSetts.mapRadius && visited.find(neighbour) == visited.end())
				{
					 temp = std::max(result.at(pos).getMoisture() - 1, result.at(neighbour).getMoisture());
					 result.at(neighbour).setMoisture(temp);
					 toExamine.push(std::pair<Coords, int>(neighbour, temp));
				}
		  }
	 }
}

void generate(std::map<Coords, Hex>& result, hexSettings& hSetts, genSettings& gSetts)
{		  
	 initArea(hSetts, result, gSetts.start, gSetts.mapRadius);

	 generateNoiseHeight(gSetts, result);

	 generateShape(gSetts, result);

	 markOceans(gSetts, result);

	 generateMountains(hSetts, gSetts, result);

	 generateRivers(gSetts, result);

	 generateMoisture(gSetts, result);
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
