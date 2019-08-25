#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "hex.hpp"
#include "hexmap.hpp"


bool TileMap::load(hexSettings& hSettings, mapSettings& mSettings, std::map<Coords, Hex>& hexes)
{
	 float heightLight;
	 
	 sf::Vector2f tileSize(hSettings.hexWidth  + 2 * hSettings.hexOffset,
								  hSettings.hexHeight + 2 * hSettings.hexOffset);
	 
	 // load the tileset texture
	 if(!m_tileset.loadFromFile(mSettings.filename)) return false;

	 // resize the vertex array to fit the level size
	 m_vertices.setPrimitiveType(sf::Quads);
	 //m_vertices.resize(hexes.size() * 4);

	 // populate the vertex array, with one quad per tile
	 for(auto it = hexes.begin(); it != hexes.end(); ++it)
	 {
		  //std::cout << it->second.getType() << std::endl;
		  
		  heightLight = std::round((std::max(1.f, 1.f - 5 * mSettings.lightPerHeight)
											 - it->second.getHeight() * mSettings.lightPerHeight) * 255);
		  // hex types
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSettings, it->first) +
										sf::Vector2f(0.f,
														 -it->second.getHeight() * mSettings.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f( it->second.getType()      * (tileSize.x + 1)    ,
														  0.f)));
		  
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSettings, it->first) +
										sf::Vector2f(tileSize.x,
														 -it->second.getHeight() * mSettings.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1,
														 0.f)));
								  
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSettings, it->first) +
										sf::Vector2f(tileSize.x,
														 tileSize.y - it->second.getHeight() * mSettings.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1,
														 tileSize.y)));
		  
		  m_vertices.append(sf::Vertex(
										coordsToPixelF(hSettings, it->first) +
										sf::Vector2f(0.f,
														 tileSize.y - it->second.getHeight() * mSettings.pixelsPerLevel),
										sf::Color(heightLight, heightLight, heightLight),
										sf::Vector2f( it->second.getType()      * (tileSize.x + 1)    ,
														  tileSize.y)));

		  for(int i = 0; i < it->second.getWater().size(); ++i)
		  {
				// rivers
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSettings, it->first) +
											 sf::Vector2f(0.f,
															  -it->second.getHeight() * mSettings.pixelsPerLevel),
											 sf::Vector2f( it->second.getWater()[i]      * (tileSize.x + 1)    ,
																(tileSize.y + 1) * 2)));
		  
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSettings, it->first) +
											 sf::Vector2f(tileSize.x,
															  -it->second.getHeight() * mSettings.pixelsPerLevel),
											 sf::Vector2f((it->second.getWater()[i] + 1) * (tileSize.x + 1) - 1,
															  (tileSize.y + 1) * 2)));
								  
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSettings, it->first) +
											 sf::Vector2f(tileSize.x,
															  tileSize.y - it->second.getHeight()
															  * mSettings.pixelsPerLevel),
											 sf::Vector2f((it->second.getWater()[i] + 1) * (tileSize.x + 1) - 1,
															  (tileSize.y + 1) * 3 - 1)));
		  
				m_vertices.append(sf::Vertex(
											 coordsToPixelF(hSettings, it->first) +
											 sf::Vector2f(0.f,
															  tileSize.y - it->second.getHeight()
															  * mSettings.pixelsPerLevel),
											 sf::Vector2f( it->second.getWater()[i]      * (tileSize.x + 1)    ,
																(tileSize.y + 1) * 3 - 1)));
		  }
	 }

	 return true;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	 // apply the transform
	 states.transform *= getTransform();

	 // apply the tileset texture
	 states.texture = &m_tileset;

	 // draw the vertex array
	 target.draw(m_vertices, states);
}

/*std::vector<Coords> createRidge(Coords start, Coords end, float deviation)
{
	 std::vector<Coords> result;
	 if(length(start, end) <= 1) return result;

	 Coords middle;
}*/


HexMap::HexMap(hexSettings& hSettings, mapSettings& mSettings, genSettings& gSettings, Coords start):
	 m_hexSettings(hSettings),
	 m_mapSettings(mSettings),
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
							  (it->first, Hex(m_hexSettings, it->first, it->second.getHeight(),
													it->second.getType(), it->second.getWater())));
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
