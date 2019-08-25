#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <queue>
#include <map>

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

Coords middle(hexSettings& hSettings, Coords start, Coords end)
{
	 sf::Vector2i point = sf::Vector2i(coordsToCenter(hSettings, start) +
												  (coordsToCenter(hSettings, end) -
													coordsToCenter(hSettings, start))/2);
	 return pixelToCoords(hSettings, point);
}

void addToMap(std::map<Coords, Hex>& result, std::map<Coords, Hex> toAdd, hexSettings& hSettings)
{
	 std::map<Coords, Hex>::iterator found;
	 for(auto it = toAdd.begin(); it != toAdd.end(); ++it)
	 {
		  found = result.find(it->first);
		  if(found == result.end())
		  {
				result.insert(*it);
		  }
		  else found->second = it->second;
	 }
}

void initArea(hexSettings& hSettings, std::map<Coords, Hex>& result, Coords pos, int radius)
{
	 for(int ring = 0; ring <= radius; ++ring)
	 {
		  for(int turn = 0; turn < 6; ++turn)
		  {
				if(ring == 0) result.insert(std::pair<Coords, Hex>(pos, {hSettings, pos, 0, 0}));
				for(int i = 0; i < ring; ++i)
				{
					 result.insert(std::pair<Coords, Hex>(pos, {hSettings, pos, 0, 0}));
					 /*std::cout << "(" << pos.m_r << ", " << pos.m_q << ") =?= {"
								  << coordsToCenter(hSettings, pos).x << ", "
								  << coordsToCenter(hSettings, pos).y << "} ";
					 centerToCoords(hSettings, coordsToCenter(hSettings, pos)).print();

					 std::cout << (pos == centerToCoords(hSettings, coordsToCenter(hSettings, pos)))
								  << std::endl;
					 */
					 
					 pos += direction(turn);
				}
		  }
		  pos += direction(4);
	 }
}

std::map<Coords, Hex> createRidge(
	 hexSettings& hSettings, genSettings& gSettings, Coords start, Coords end)
{
	 //getchar();
	 
	 std::map<Coords, Hex> result;
	 Coords middle = {0, 0};
	 
	 if(length(distance(start, end)) > 1)
	 {
		  // converting coords into pixels for more natural curves
		  sf::Vector2i startPixel = coordsToCenter(hSettings, start);
		  sf::Vector2i endPixel = coordsToCenter(hSettings, end);
		  sf::Vector2i middlePixel = startPixel + (endPixel - startPixel)/2;

		  //std::cout << "(" <<
		  //		startPixel.x << ", " << startPixel.y << ") | (" <<
		  //  	endPixel.x   << ", " << endPixel.y   << ")" << std::endl;
				
		  sf::Vector2i variation;

		  do{
				// curving ridges
				variation.x = RandomI(std::round(length(distance(start, end))
															* (hSettings.hexWidth -  hSettings.hexQuarter)
															* -gSettings.deviation),
											 std::round(length(distance(start, end))
															* (hSettings.hexWidth -  hSettings.hexQuarter)
															*  gSettings.deviation));
				variation.y = RandomI(std::round(length(distance(start, end))
															* hSettings.hexHeight * -gSettings.deviation),
											 std::round(length(distance(start, end))
															* hSettings.hexHeight *  gSettings.deviation));

				middlePixel += variation;
				middle = centerToCoords(hSettings, middlePixel);

				//checking if in bounds
		  }while(length(middle) > gSettings.mapRadius || middle == start || middle == end);
		  
		  result.insert(std::pair<Coords, Hex>(middle, {hSettings, middle, 0, 3}));

		  // recurrently generate next
		  addToMap(result, createRidge(hSettings, gSettings, start , middle), hSettings);
		  addToMap(result, createRidge(hSettings, gSettings, middle, end   ), hSettings);
	 }

	 return result;
}

std::map<Coords, Hex> generateHeight(hexSettings& hSettings, genSettings& gSettings,
													  std::map<Coords, Hex> mountains)
{
	 std::map<Coords, Hex> result;
	 std::priority_queue<std::pair<Coords, int>, std::vector<std::pair<Coords, int>>, heightComp>
		  toExamine;
	 std::map<Coords, bool> visited;
	 Coords examined(0, 0);
	 Coords neighbour(0, 0);
	 Coords pos(0, 0);
	 int height;

	 for(auto it = mountains.begin(); it != mountains.end(); ++it)
	 {
		  result.insert(std::pair<Coords, Hex>
							 (it->first, {hSettings, it->first, gSettings.mountainHeight, 4}));
		  toExamine.emplace(std::pair<Coords, int>(it->first, gSettings.mountainHeight));
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
					 if(length(neighbour) <= gSettings.mapRadius && visited.find(neighbour) == visited.end())
					 {
						  height = randomFromDist(0, 1, gSettings.heightProb[result.at(examined).getHeight()]);
						 
						  if(result.find(neighbour) != result.end())
						  {
								result.at(neighbour).setHeight(
									 std::max(result.at(neighbour).getHeight(), height));
						  }
						  else result.insert(std::pair<Coords, Hex>(neighbour,
																				  {hSettings, neighbour, height, 1}));
					 
						  if(result.at(neighbour).getHeight() == gSettings.mountainHeight)
								result.at(neighbour).setType(4);
						  if(result.at(neighbour).getHeight() > 0)
						  {
								toExamine.emplace(neighbour, result.at(neighbour).getHeight());
						  }
					 }
				}
				visited[examined] = true;
		  }
	 }
	 
	 /*for(auto it = mountains.begin(); it != mountains.end(); ++it)
	 {
		  pos = it->first;
		  
		  for(int ring = 0; ring <= gSettings.heightRadius; ++ring)
		  {
				for(int turn = 0; turn < 6; ++turn)
				{
					 for(int i = 0; i < ring; ++i)
					 {
						  if(length(pos) <= gSettings.mapRadius)
						  {
								height = randomFromDist(0, 1, heightProb[gSettings.heightOfRings[ring-1];
								if(temp.find(pos) != temp.end())
								{
									 temp[pos].setHeight(std::max(temp[pos].getHeight(), height));
								}
								else temp[pos] = {0, height};
						  }
						  
						  pos += direction(turn);
					 }
				}
				pos += direction(4);
		  }
		  }*/

	 /*pos = gSettings.start;
	 for(int ring = 0; ring <= gSettings.mapRadius; ++ring)
	 {
		  for(int turn = 0; turn < 6; ++turn)
		  {
				if(ring == 0)
				{
					 if(result.find(pos) == result.end()) result[pos] = {0, 0};
					 else if(result[pos].getHeight() == 0) result[pos].setType() = 0;
				}
				
				for(int i = 0; i < ring; ++i)
				{
					 if(result.find(pos) == result.end()) result[pos] = {0, 0};
					 else if(result[pos].getHeight() == 0) result[pos].setType() = 0;
					 
					 pos += direction(turn);
				}
		  }
		  pos += direction(4);		  
	 }*/

	 return result;
}

std::map<Coords, Hex> generateMountains(hexSettings& hSettings, genSettings& gSettings)
{
	 std::map<Coords, Hex> result;
	 Coords start(0, 0);
	 Coords end(0, 0);
	 int count = 0;

	 for(int i = 0; i < gSettings.TORidges; ++i)
	 {
		  // randomizing start and end points until they're close enough
		  do{
				start = randomCoord(gSettings.mountainRadius, gSettings.start);
				end   = randomCoord(gSettings.mountainRadius, gSettings.start);
		  }while(length(distance(start, end)) > gSettings.maxRidgeLength);
				
		  start.print();
		  end.print();
		  addToMap(result, createRidge(hSettings, gSettings, start, end), hSettings);
	 }
	 
	 result = generateHeight(hSettings, gSettings, result);

	 return result;
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

void generateShape(genSettings& gSettings, std::map<Coords, Hex>& result)
{
	 Coords pos = gSettings.start;

	 // creating the shape
	 //std::cout << "OY THERE GOVENOR" << std::endl;
	 for(int ring = 0; ring <= gSettings.coastline; ++ring)
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
	 for(int i = 0; i < gSettings.TOBuildups; ++i)
	 {
		  pos = gSettings.start;
		  for(int ring = 0; ring <= gSettings.landRadius; ++ring)
		  {
				for(int turn = 0; turn < 6; ++turn)
				{
					 if(ring == 0)
					 {
						  if(result.at(pos).getType() == 0 &&
							  RandomF(0.001f, 1.f, 0.001f) <=
							  gSettings.buildupProb[countNeighbours(result, pos, gSettings.mapRadius)[1]])
								result.at(pos).setType(1);
					 }
				
					 for(int i = 0; i < ring; ++i)
					 {
						  if(result.at(pos).getType() == 0 &&
							  RandomF(0.001f, 1.f, 0.001f) <=
							  gSettings.buildupProb[countNeighbours(result, pos, gSettings.mapRadius)[1]])
								result.at(pos).setType(1);
					 
						  pos += direction(turn);
					 }
				}
				pos += direction(4);		  
		  }
	 }

	 // eroding the shape (without touching higher hexes)
	 for(int i = 0; i < gSettings.TOErosions; ++i)
	 {
		  pos = gSettings.start;
		  for(int ring = 0; ring <= gSettings.landRadius; ++ring)
		  {
				for(int turn = 0; turn < 6; ++turn)
				{
					 if(ring == 0)
					 {
						  if(result.at(pos).getType() == 1 && result.at(pos).getHeight() == 0 &&
							  RandomF(0.001f, 1.f, 0.001f) <=
							  gSettings.erosionProb[countNeighbours(result, pos, gSettings.mapRadius)[0]])
								result.at(pos).setType(0);
					 }
				
					 for(int i = 0; i < ring; ++i)
					 {
						  if(result.at(pos).getType() == 1 && result.at(pos).getHeight() == 0 &&
							  RandomF(0.001f, 1.f, 0.001f) <= 
							  gSettings.erosionProb[countNeighbours(result, pos, gSettings.mapRadius)[0]])
								result.at(pos).setType(0);
					 
						  pos += direction(turn);
					 }
				}
				pos += direction(4);		  
		  }
	 }
}

bool closerToZero(const int& a, const int& b)
{
	 return (abs(a) < abs(b));
}

void generateRivers(genSettings& gSettings, std::map<Coords, Hex>& result)
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

	 for(int i = 0; i < gSettings.TORivers; ++i)
	 {
		  do{
				temp.clear();
				do{
					 start = randomCoord(gSettings.coastline);
				}while(result.at(start).getType() == 0 ||
						 result.at(start).getWater().size() > 0 ||
						 (result.at(start).getType() != 4 &&
						  countNeighbours(result, start, gSettings.mapRadius)[4] == 0));

				pos = start;
				step = 0;
				
				visited.clear();

				do{
					 illegalRiver = true;
					 metOther = false;
					 visited[pos] = true;

					 if(RandomF(0.001f, 1.0f, 0.001f) <= gSettings.meanderFreq[step])
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
						  if(length(neighbour) <= gSettings.mapRadius &&
							  visited.find(neighbour) == visited.end() &&
							  result.at(neighbour).getType() != 4 &&
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
				result.at(pos).addWater(temp[i]);
				pos += direction(temp[i]);
				result.at(pos).addWater((temp[i] + 3) % 6);
		  }
	 }
}

void generate(std::map<Coords, Hex>& result, hexSettings& hSettings, genSettings& gSettings)
{		  
	 initArea(hSettings, result, gSettings.start, gSettings.mapRadius);

	 //markOceans(gSettings, result);

	 addToMap(result, generateMountains(hSettings, gSettings), hSettings);

	 generateShape(gSettings, result);

	 generateRivers(gSettings, result);
}
