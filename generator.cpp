#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <queue>
#include <map>

#include "tom.hpp"
#include "coords.hpp"
#include "generator.hpp"


Coords randomCoord(int radius, Coords start = {0, 0})
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

void addToMap(std::map<Coords, tempHex>& result, std::map<Coords, tempHex> toAdd)
{
	 for(auto it = toAdd.begin(); it != toAdd.end(); ++it)
	 {
		  result[it->first] = it->second;
	 }
}

void initArea(hexSettings& hSettings, std::map<Coords, tempHex>& result, Coords pos, int radius)
{
	 for(int ring = 0; ring <= radius; ++ring)
	 {
		  for(int turn = 0; turn < 6; ++turn)
		  {
				if(ring == 0) result.insert(std::pair<Coords, tempHex>(pos, {1, 0}));
				for(int i = 0; i < ring; ++i)
				{
					 result.insert(std::pair<Coords, tempHex>(pos, {1, 0}));
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

void printMap(std::map<Coords, tempHex>& result)
{
	 int prevY = result.begin()->first.m_r;
	 for(auto it = result.begin(); it != result.end(); ++it)
	 {
		  if(it->first.m_r != prevY) std::cout << std::endl;
		  std::cout << it->second.type;
	 }
}

std::map<Coords, tempHex> createRidge(
	 hexSettings& hSettings, genSettings& gSettings, Coords start, Coords end)
{
	 //getchar();
	 
	 std::map<Coords, tempHex> result;
	 Coords middle = {0, 0};
	 
	 if(length(distance(start, end)) > 1)
	 {
		  
		  sf::Vector2i startPixel = coordsToCenter(hSettings, start);
		  sf::Vector2i endPixel = coordsToCenter(hSettings, end);
		  sf::Vector2i middlePixel = startPixel + (endPixel - startPixel)/2;

		  //std::cout << "(" <<
		  //		startPixel.x << ", " << startPixel.y << ") | (" <<
		  //  	endPixel.x   << ", " << endPixel.y   << ")" << std::endl;
				
		  sf::Vector2i variation;

		  do{
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
				
		  }while(length(middle) > gSettings.radius || middle == start || middle == end);
		  
		  result.insert(std::pair<Coords, tempHex>(middle, {3, 0}));
		  
		  addToMap(result, createRidge(hSettings, gSettings, start , middle));
		  addToMap(result, createRidge(hSettings, gSettings, middle, end   ));
	 }

	 return result;
}

std::map<Coords, tempHex> generateHeight(hexSettings& hSettings, genSettings& gSettings,
													  std::map<Coords, tempHex> mountains)
{
	 std::map<Coords, tempHex> result;
	 std::priority_queue<std::pair<Coords, int>, std::vector<std::pair<Coords, int>>, heightComp>
		  toExamine;
	 std::map<Coords, bool> visited;
	 Coords examined(0, 0);
	 Coords neighbour(0, 0);
	 Coords pos(0, 0);
	 int height;

	 for(auto it = mountains.begin(); it != mountains.end(); ++it)
	 {
		  result.insert(std::pair<Coords, tempHex>(it->first, {4, gSettings.mountainHeight}));
		  toExamine.emplace(std::pair<Coords, int>(it->first, it->second.height));
	 }

	 while(toExamine.size() > 0)
	 {
		  examined = toExamine.top().first;
		  toExamine.pop();

		  if(visited.find(examined) == visited.end())
		  {
				for(int i = 0; i < 6; ++i)
				{
					 neighbour = examined + direction(i);
					 if(length(neighbour) <= gSettings.radius && visited.find(neighbour) == visited.end())
					 {
						  height = randomFromDist(0, 1, gSettings.heightProb[result[examined].height]);
						 
						  if(result.find(neighbour) != result.end())
						  {
								result[neighbour].height = std::max(result[neighbour].height, height);
						  }
						  else result[neighbour] = {1, height};
					 
						  if(result[neighbour].height == gSettings.mountainHeight) result[neighbour].type = 4;
						  if(result[neighbour].height > 0)
						  {
								toExamine.emplace(neighbour, result[neighbour].height);
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
						  if(length(pos) <= gSettings.radius)
						  {
								height = randomFromDist(0, 1, heightProb[gSettings.heightOfRings[ring-1];
								if(temp.find(pos) != temp.end())
								{
									 temp[pos].height = std::max(temp[pos].height, height);
								}
								else temp[pos] = {0, height};
						  }
						  
						  pos += direction(turn);
					 }
				}
				pos += direction(4);
		  }
		  }*/

	 pos = gSettings.start;
	 for(int ring = 0; ring <= gSettings.radius; ++ring)
	 {
		  for(int turn = 0; turn < 6; ++turn)
		  {
				if(ring == 0)
				{
					 if(result.find(pos) == result.end()) result[pos] = {0, 0};
					 else if(result[pos].height == 0) result[pos].type = 0;
				}
				
				for(int i = 0; i < ring; ++i)
				{
					 if(result.find(pos) == result.end()) result[pos] = {0, 0};
					 else if(result[pos].height == 0) result[pos].type = 0;
					 
					 pos += direction(turn);
				}
		  }
		  pos += direction(4);		  
	 }

	 return result;
}

std::map<Coords, tempHex> generateMountains(hexSettings& hSettings, genSettings& gSettings,
														  std::vector<Coords>& mountains)
{
	 std::map<Coords, tempHex> result;
	 Coords start(0, 0);
	 Coords end(0, 0);
	 int count = 0;

	 for(int i = 0; i < gSettings.TORidges; ++i)
	 {
		  do{
				start = randomCoord(gSettings.mountainRadius, gSettings.start);
				end   = randomCoord(gSettings.mountainRadius, gSettings.start);
		  }while(length(distance(start, end)) > gSettings.maxRidgeLength);
				
		  start.print();
		  end.print();
		  addToMap(result, createRidge(hSettings, gSettings, start, end));
	 }

	 for(auto it = result.begin(); it != result.end(); ++it)
	 {
		  mountains.push_back(it->first);
	 }
	 result = generateHeight(hSettings, gSettings, result);

	 return result;
}

bool closerToZero(const int& a, const int& b)
{
	 return (abs(a) < abs(b));
}

void generateRivers(genSettings& gSettings, std::map<Coords, tempHex>& result,
													  std::vector<Coords>& mountains)
{
	 Coords curr(0, 0);
	 int dir = RandomI(0, 5);
	 std::vector< std::vector<int> > heights(6, std::vector<int>(0));
	 std::map<Coords, bool> visited;
	 Coords neighbour(0, 0);

	 for(int i = 0; i < gSettings.TORivers; ++i)
	 {
		  curr = mountains[RandomI(0, mountains.size()-1)];
		  visited.clear();

		  while(result[curr].height > 0)
		  {
				for(int j = 0; j <= 5; ++j)
				{
					 heights[j].clear();
				}

				//--result[curr].height;
				visited[curr] = true;
				
				for(int j = 0; j < 6; ++j)
				{
					 neighbour = curr + direction(j);
					 if(length(neighbour) <= gSettings.radius && visited.find(neighbour) == visited.end())
					 {
						  heights[result[neighbour].height].emplace_back(j - dir);
					 }
				}

				for(int j = 0; j <= 5; ++j)
				{
					 if(heights[j].size() > 0)
					 {
						  heights[j] = Choose(heights[j], heights[j].size());
						  sort(heights[j].begin(), heights[j].end(), closerToZero);


						  dir += heights[j][0];
						  std::cout << dir;
						  result[curr].water.emplace_back((dir) % 6);
						  curr += direction(dir);
						  result[curr].water.emplace_back((dir + 3) % 6);
						  std::cout << dir << std::endl;
						  break;
					 }
				}
		  }
	 }
	 std::cout << "'Ere'" << std::endl;
}

void generate(std::map<Coords, tempHex>& result, hexSettings& hSettings, genSettings& gSettings)
{
	 std::vector<Coords> mountains;
		  
	 initArea(hSettings, result, gSettings.start, gSettings.radius);

	 addToMap(result, generateMountains(hSettings, gSettings, mountains));

	 generateRivers(gSettings, result, mountains);

	 result[direction(0)].type = 0;
	 result[direction(1)].type = 1;
	 result[direction(2)].type = 2;
	 result[direction(3)].type = 3;
	 result[direction(4)].type = 4;
	 result[direction(5)].type = 5;
	 
	 std::cout << "'Ere'" << std::endl;
}
