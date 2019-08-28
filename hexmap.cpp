#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "hex.hpp"
#include "hexmap.hpp"


void createGradient(std::vector<sf::Color>& result, sf::Color start, sf::Color end, int total)
{
	 sf::Color temp = start;
	 
	 result.emplace_back(start);
	 for(int i = 1; i < total-1; ++i)
	 {
		  temp = sf::Color(
				std::max(0, std::min(255, (int)std::round((((float)end.r) - start.r) / (total-1) + temp.r))),
				std::max(0, std::min(255, (int)std::round((((float)end.g) - start.g) / (total-1) + temp.g))),
				std::max(0, std::min(255, (int)std::round((((float)end.b) - start.b) / (total-1) + temp.b))));

		  result.emplace_back(temp);
	 }
	 result.emplace_back(end);
}

bool TileMap::load(hexSettings& hSetts, mapSettings& mSetts, std::map<Coords, Hex>& hexes)
{
	 setMode(DisplayMode::Textured);
	 
	 sf::Vector2f tileSize(hSetts.hexWidth  + 2 * hSetts.hexOffset,
								  hSetts.hexHeight + 2 * hSetts.hexOffset);
	 float heightLight;
	 int height;
	 std::vector<sf::Vector2f> position(4);
	 std::vector<sf::Vector2f> texPos(4);
	 std::vector<sf::Vector2f> riverPos(4);
	 std::vector<sf::Vector2f> blankPos(4);
	 std::vector<sf::Color> heightGradient;
	 std::vector<sf::Color> elevationGradient;
	 std::vector<sf::Color> temperatureGradient;
	 std::vector<sf::Color> moistureGradient;

	 createGradient(heightGradient, sf::Color(255, 255, 255), sf::Color(191, 191, 191), 6);
	 createGradient(elevationGradient, sf::Color(255, 255, 255), sf::Color(64, 255, 64), 10);
	 createGradient(temperatureGradient, sf::Color(255, 255, 255), sf::Color(255, 128, 0), 10);
	 createGradient(moistureGradient, sf::Color(255, 255, 255), sf::Color(0, 128, 255), 10);
	 
	 if(!m_tileset.loadFromFile(mSetts.filename)) return false;
	 m_defaultMap.setPrimitiveType(sf::Quads);
	 m_elevationMap.setPrimitiveType(sf::Quads);
	 m_temperatureMap.setPrimitiveType(sf::Quads);
	 m_moistureMap.setPrimitiveType(sf::Quads);

	 for(auto it = hexes.begin(); it != hexes.end(); ++it)
	 {
		  position[0] = coordsToPixelF(hSetts, it->first) +
				sf::Vector2f(0.f       ,            - it->second.getHeight() * mSetts.pixelsPerLevel);
		  
		  position[1] = coordsToPixelF(hSetts, it->first) +
				sf::Vector2f(tileSize.x,            - it->second.getHeight() * mSetts.pixelsPerLevel);
		  
		  position[2] = coordsToPixelF(hSetts, it->first) +
				sf::Vector2f(tileSize.x, tileSize.y - it->second.getHeight() * mSetts.pixelsPerLevel);
		  
		  position[3] = coordsToPixelF(hSetts, it->first) +
				sf::Vector2f(0.f       , tileSize.y - it->second.getHeight() * mSetts.pixelsPerLevel);

		  texPos[0] = sf::Vector2f((it->second.getType()    ) * (tileSize.x + 1)    ,
											(tileSize.y + 1)         );
		  texPos[1] = sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1,
											(tileSize.y + 1)        );
		  texPos[2] = sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1,
											(tileSize.y + 1) * 2 - 1);
		  texPos[3] = sf::Vector2f((it->second.getType()    ) * (tileSize.x + 1)    ,
											(tileSize.y + 1) * 2 - 1);
		  
		  blankPos[0] = sf::Vector2f(0.f       , 0.f       );
		  blankPos[1] = sf::Vector2f(tileSize.x, 0.f       );
		  blankPos[2] = sf::Vector2f(tileSize.x, tileSize.y);
		  blankPos[3] = sf::Vector2f(0.f       , tileSize.y);

		  for(int i = 0; i < 4; ++i)
		  {
				// textured map
				m_defaultMap.append(
					 sf::Vertex(position[i], heightGradient[std::max(0, it->second.getHeight())],
									texPos[i]));
		  }

		  for(int i = 0; i < 4; ++i)
		  {
				// map colored based on hex height
				m_elevationMap.append(
					 sf::Vertex(position[i], elevationGradient[std::max(0, it->second.getHeight())],
									blankPos[i]));
		  }
		  
		  for(int i = 0; i < 4; ++i)
		  {
				// map colored based on hex temperature
				m_temperatureMap.append(
					 sf::Vertex(position[i], temperatureGradient[std::max(0, it->second.getTemperature())],
									blankPos[i]));
		  }
		  
		  for(int i = 0; i < 4; ++i)
		  {
				// map colored based on hex moisture
				m_moistureMap.append(
					 sf::Vertex(position[i], moistureGradient[std::max(0, it->second.getMoisture())],
									blankPos[i]));
		  }
		  
		  for(int i = 0; i < it->second.getWater().size(); ++i)
		  {
				riverPos[0] = sf::Vector2f((it->second.getWater()[i]    ) * (tileSize.x + 1)    ,
													(tileSize.y + 1) * 3    );
				riverPos[1] = sf::Vector2f((it->second.getWater()[i] + 1) * (tileSize.x + 1) - 1,
													(tileSize.y + 1) * 3    );
				riverPos[2] = sf::Vector2f((it->second.getWater()[i] + 1) * (tileSize.x + 1) - 1,
													(tileSize.y + 1) * 4 - 1);
				riverPos[3] = sf::Vector2f((it->second.getWater()[i]    ) * (tileSize.x + 1)    ,
													(tileSize.y + 1) * 4 - 1);
				
				for(int j = 0; j < 4; ++j)
				{
					 // rivers on textured map
					 m_defaultMap.append(
						  sf::Vertex(position[j], riverPos[j]));

					 // rivers on map colored based on hex moisture
					 m_moistureMap.append(
						  sf::Vertex(position[j], riverPos[j]));
				}
		  }

		  /*
		  // hex types
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSetts, it->first) +
										sf::Vector2f(0.f,
														 -it->second.getHeight() * mSetts.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f( it->second.getType()      * (tileSize.x + 1)    ,
														  0.f)));
		  
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSetts, it->first) +
										sf::Vector2f(tileSize.x,
														 -it->second.getHeight() * mSetts.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1,
														 0.f)));
								  
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSetts, it->first) +
										sf::Vector2f(tileSize.x,
														 tileSize.y - it->second.getHeight() * mSetts.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1,
														 tileSize.y)));
		  
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSetts, it->first) +
										sf::Vector2f(0.f,
														 tileSize.y - it->second.getHeight() * mSetts.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f( it->second.getType()      * (tileSize.x + 1)    ,
														  tileSize.y)));

		  for(int i = 0; i < it->second.getWater().size(); ++i)
		  {
				// rivers
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSetts, it->first) +
											 sf::Vector2f(0.f,
															  -it->second.getHeight() * mSetts.pixelsPerLevel),
											 sf::Vector2f( it->second.getWater()[i]      * (tileSize.x + 1)    ,
																(tileSize.y + 1) * 2)));
		  
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSetts, it->first) +
											 sf::Vector2f(tileSize.x,
															  -it->second.getHeight() * mSetts.pixelsPerLevel),
											 sf::Vector2f((it->second.getWater()[i] + 1) * (tileSize.x + 1) - 1,
															  (tileSize.y + 1) * 2)));
								  
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSetts, it->first) +
											 sf::Vector2f(tileSize.x,
															  tileSize.y - it->second.getHeight()
															  * mSetts.pixelsPerLevel),
											 sf::Vector2f((it->second.getWater()[i] + 1) * (tileSize.x + 1) - 1,
															  (tileSize.y + 1) * 3 - 1)));
		  
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSetts, it->first) +
											 sf::Vector2f(0.f,
															  tileSize.y - it->second.getHeight()
															  * mSetts.pixelsPerLevel),
											 sf::Vector2f( it->second.getWater()[i]      * (tileSize.x + 1)    ,
																(tileSize.y + 1) * 3 - 1)));
		  }
		  */
	 }

	 return true;
}

void TileMap::setMode(DisplayMode mode)
{
	 m_mode = mode;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	 // apply the transform
	 states.transform *= getTransform();
	 states.texture = &m_tileset;

	 switch(m_mode)
	 {
		  case DisplayMode::Textured: target.draw(m_defaultMap, states); break;
		  case DisplayMode::Elevation: target.draw(m_elevationMap, states); break;
		  case DisplayMode::Temperature: target.draw(m_temperatureMap, states); break;
		  case DisplayMode::Moisture: target.draw(m_moistureMap, states); break;
	 }
}

/*std::vector<Coords> createRidge(Coords start, Coords end, float deviation)
{
	 std::vector<Coords> result;
	 if(length(start, end) <= 1) return result;

	 Coords middle;
}*/


HexMap::HexMap(hexSettings& hSetts, mapSettings& mSetts, genSettings& gSettings, Coords start):
	 m_hexSettings(hSetts),
	 m_mapSettings(mSetts),
	 m_genSettings(gSettings),
	 m_start(start),
	 m_screenPos(0, 0),
	 m_zoom(1.f),
	 m_mode(0),
	 m_lastRing(-1)
{
	 m_view = sf::View(sf::Vector2f(m_screenPos), sf::Vector2f(m_mapSettings.viewSize));
	 createMap();
	 m_tilemap.load(m_hexSettings, m_mapSettings, m_hexes);
}

bool HexMap::setType(Coords coords, int type)
{
	 if(doesHexExists(coords))
	 {
		  m_hexes.at(coords).setType(type);
		  return true;
	 }
	 return false;
}

bool HexMap::setBorder(Coords coords, int border, int type)
{
	 if(doesHexExists(coords))
	 {
		  m_hexes.at(coords).setBorder(border, type);
		  return true;
	 }
	 return false;
}

int HexMap::doInRing(Coords coords, int ring, bool allowEmpty, Processor& operation, ...)
{
	 int count = 0;
	 va_list args;

	 if(ring == 0)
	 {
		  if(allowEmpty || doesHexExists(coords))
		  {
				va_start(args, operation);
				if(operation(*this, coords, 0, args)) ++count;
				va_end(args);
		  }
	 }
	 else
	 {
		  coords += direction(4) * ring;
		  for(int turn = 0; turn < 6; ++turn)
		  {
				for(int i = 0; i < ring; ++i)
				{
					 if(allowEmpty || doesHexExists(coords))
					 {
						  va_start(args, operation);
						  if(operation(*this, coords, ring, args)) ++count;
						  va_end(args);
					 }
					 coords += direction(turn);
				}
		  }
	 }
	 
	 return count;
}

void HexMap::createMap()
{
	 std::map<Coords, Hex> result;
	 
	 generate(result, m_hexSettings, m_genSettings);

	 for(auto it = result.begin(); it != result.end(); ++it)
	 {
		  m_topLeftPixel.x     = std::min(coordsToPixelI(m_hexSettings, it->first).x, m_topLeftPixel.x    );
		  m_topLeftPixel.y     = std::min(coordsToPixelI(m_hexSettings, it->first).y, m_topLeftPixel.y    );
		  m_bottomRightPixel.x = std::max(coordsToPixelI(m_hexSettings, it->first).x, m_bottomRightPixel.x);
		  m_bottomRightPixel.y = std::max(coordsToPixelI(m_hexSettings, it->first).y, m_bottomRightPixel.y);

		  //std::cout << it->second.getType() << std::endl;
		  m_hexes.insert(std::pair<Coords, Hex>
							  (it->first,
								Hex(m_hexSettings, it->first, it->second.getType(), it->second.getHeight(),
									 it->second.getTemperature(), it->second.getMoisture(),
									 it->second.getWater())));
	 }
}

bool HexMap::moveScreen(sf::Vector2f direction)
{
	 m_screenPos += sf::Vector2i(std::round(direction.x * m_mapSettings.scrollSpeed * m_zoom),
										  std::round(direction.y * m_mapSettings.scrollSpeed * m_zoom));

	 m_screenPos.x = std::max(std::min(m_screenPos.x, m_bottomRightPixel.x +
												  (int)std::round(m_mapSettings.viewPort.width/2.0)),
									  m_topLeftPixel.x - (int)std::round(m_mapSettings.viewPort.width/2.0));
	 m_screenPos.y = std::max(std::min(m_screenPos.y, m_bottomRightPixel.y +
												  (int)std::round(m_mapSettings.viewPort.height/2.0)),
									  m_topLeftPixel.y - (int)std::round(m_mapSettings.viewPort.height/2.0));

	 return true;
}

bool HexMap::zoomScreen(bool in)
{
	 m_zoom *= 1.f + (1-2*in) * m_mapSettings.zoomSpeed;
	 m_zoom = std::max(0.001f, m_zoom);
	 m_view.zoom(1.f + (1-2*in) * m_mapSettings.zoomSpeed);

	 return true;
}

void HexMap::draw(sf::RenderWindow& window)
{
	 m_view.setCenter(sf::Vector2f(m_screenPos));
	 window.setView(m_view);

	 window.draw(m_tilemap);
}


/*bool InitIt::operator()(HexMap& hexes, Coords coords, int ring, va_list args)
{
	 return (hexes.initHex(coords));
}*/

bool MakeItType::operator()(HexMap& hexes, Coords coords, int ring, va_list args)
{
	 return (hexes.setType(coords, va_arg(args, int)));
}
