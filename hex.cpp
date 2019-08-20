#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "hex.hpp"


bool operator==(const Coords a, const Coords b) {return (a.m_r == b.m_r && a.m_q == b.m_q); }
bool operator!=(const Coords a, const Coords b) {return !(a == b); }
bool operator>(const Coords a, const Coords b)
{
	 if(a.m_q == b.m_q) return (a.m_r > b.m_r);
	 return (a.m_q > b.m_q);
}
bool operator<(const Coords a,const  Coords b)
{
	 return (a != b && !(a > b));
}
void operator+=(Coords& a, Coords b) {a.m_r += b.m_r; a.m_q += b.m_q; }
Coords operator+(Coords a, Coords b) {return Coords(a.m_r + b.m_r, a.m_q + b.m_q); }
Coords operator-(Coords a, Coords b) {return Coords(a.m_r - b.m_r, a.m_q - b.m_q); }
Coords operator*(Coords a, int f) {return Coords(a.x() * f, a.y() * f); }

Coords direction(int i)
{
	 switch(i)
	 {
		  case 0:  return Coords(+1, -1); break;
		  case 1:  return Coords(+1,  0); break;
		  case 2:  return Coords( 0, +1); break;
		  case 3:  return Coords(-1, +1); break;
		  case 4:  return Coords(-1,  0); break;
		  case 5:  return Coords( 0, -1); break;
		  default: return Coords( 0,  0); break;
	 }
}

sf::Vector2i coordsToPixelI(hexSettings& set, Coords a)
{
	 int x = (set.hexWidth - set.hexQuarter)  * (-a.z());
	 int y = set.hexHeight * (a.y() - a.x())/2;

	 return sf::Vector2i(x, y);
}

sf::Vector2f coordsToPixelF(hexSettings& set, Coords a)
{
	 float x = (set.hexWidth - set.hexQuarter)  * (-a.z());
	 float y = set.hexHeight * (a.y() - a.x())/2;

	 return sf::Vector2f(x, y);
}

/*Coords pixelToCoords(hexSettings set, sf::Vector2i a)
{
	 Coords result(a.y/set.hexWidth, )
}*/

Coords distance(const Coords& a, const Coords& b)
{
	 return Coords(b.m_r - a.m_r, b.m_q - a.m_q);
}

int length(const Coords& a, const Coords& b)
{
	 Coords temp = distance(a, b);
	 return std::max(std::max(temp.x(), temp.y()), temp.z());
}

Hex::Hex(hexSettings& settings, Coords coords, int type, std::vector<int> borders):
	 m_settings(settings),
	 m_coords(coords),
	 m_mode(0)
{
	 // Representation intializations
	 setType(type);
	 for(int i=0; i<6; ++i)
	 {
		  m_borders.push_back(-1);
		  //SHexBorders.push_back(sf::Sprite());
		  setBorder(i, borders[i]);
	 }
}

void Hex::setType(int newType)
{
	 int temp = m_mode;
	 m_type = newType;

	 //if(m_type >= 0) Shex.setTexture(m_settings.hexTypes[m_type].texture);
	 
	 m_mode = -1;
	 switchMode(temp);
}

void Hex::setBorder(int border, int type)
{
	 m_borders[border] = type;
	 //if(m_borders[border] >= 0) SHexBorders[border].setTexture(m_settings.wallTypes
	 //																			  [m_borders[border]].textures[border]);
}

bool Hex::switchMode(int newMode)
{
	 if(m_mode == newMode) return false;
	 m_mode = newMode;
	 
	 /*switch(m_mode)
	 {
		  case 0: // normal
				Shex.setColor(m_settings.normalColors[m_type]);
				break;
	 }*/
	 return true;
}

/*void Hex::draw(sf::RenderWindow& window, int mode)
{
	 sf::Vector2f position(coordsToPixel(m_settings, m_coords) -
								  sf::Vector2i(m_settings.hexWidth/2, m_settings.hexHeight/2));
	 
	 Shex.setPosition(position);
	 switchMode(mode);
	 
	 if(m_type >= 0) window.draw(Shex);
	 
	 for(int i=0; i<m_borders.size(); ++i)
	 {
		  SHexBorders[i].setPosition(position);
		  if(m_borders[i] >= 0) window.draw(SHexBorders[i]);
	 }
}*/


bool TileMap::load(hexSettings& hSettings, std::map<Coords, Hex>& hexes)
{
	 int count = 0;
	 
	 sf::Vector2f tileSize(hSettings.hexWidth  + 2 * hSettings.hexOffset,
								  hSettings.hexHeight + 2 * hSettings.hexOffset);
	 
	 // load the tileset texture
	 if(!m_tileset.loadFromFile(hSettings.filename)) return false;

	 // resize the vertex array to fit the level size
	 m_vertices.setPrimitiveType(sf::Quads);
	 m_vertices.resize(hexes.size() * 4);

	 // populate the vertex array, with one quad per tile
	 for(auto it = hexes.begin(); it != hexes.end(); ++it)
	 {
		  // define its 4 corners
		  m_vertices[count    ].position =
				coordsToPixelF(hSettings, it->first);
		  m_vertices[count + 1].position =
				coordsToPixelF(hSettings, it->first) + sf::Vector2f(tileSize.x, 0.f);
		  m_vertices[count + 2].position =
				coordsToPixelF(hSettings, it->first) + tileSize;
		  m_vertices[count + 3].position =
				coordsToPixelF(hSettings, it->first) + sf::Vector2f(0.f, tileSize.y);

		  if(it->second.getType() >= 0)
		  {
				// define its 4 texture coordinates
			    m_vertices[count    ].texCoords =
					  sf::Vector2f( it->second.getType()      * (tileSize.x + 1)    , 0.f);
			    m_vertices[count + 1].texCoords =
					  sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1, 0.f);
				 m_vertices[count + 2].texCoords =
					  sf::Vector2f((it->second.getType() + 1) * (tileSize.x + 1) - 1, tileSize.y);
				 m_vertices[count + 3].texCoords =
					  sf::Vector2f( it->second.getType()      * (tileSize.x + 1)    , tileSize.y);
		  }
		  count += 4;
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


HexMap::HexMap(hexSettings& hSettings, mapSettings& mSettings, Coords start):
	 m_hexSettings(hSettings),
	 m_mapSettings(mSettings),
	 m_start(start),
	 m_screenPos(0, 0),
	 m_mode(0),
	 m_lastRing(-1)
{
	 m_view = sf::View(sf::Vector2f(m_screenPos), sf::Vector2f(m_mapSettings.viewSize));
	 generateUpTo(6);
	 m_tilemap.load(m_hexSettings, m_hexes);
}

bool HexMap::initHex(Coords coords)
{
	 m_topLeftPixel.x     = std::min(coordsToPixelI(m_hexSettings, coords).x, m_topLeftPixel.x    );
	 m_topLeftPixel.y     = std::min(coordsToPixelI(m_hexSettings, coords).y, m_topLeftPixel.y    );
	 m_bottomRightPixel.x = std::max(coordsToPixelI(m_hexSettings, coords).x, m_bottomRightPixel.x);
	 m_bottomRightPixel.y = std::max(coordsToPixelI(m_hexSettings, coords).y, m_bottomRightPixel.y);
	 
	 m_hexes.insert(std::pair<Coords, Hex>(coords, Hex(m_hexSettings, coords, 0)));
	 return true;
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

void HexMap::generateUpTo(int upTo)
{
	 InitIt initProc;
	 MakeItType makeProc;
	 for(int i = m_lastRing+1; i <= upTo; ++i)
	 {
		  doInRing(m_start, i, true , initProc);
		  doInRing(m_start, i, true , makeProc, 0);
	 }
	 m_lastRing = upTo;

	 
}

bool HexMap::moveScreen(sf::Vector2f direction)
{
	 m_screenPos += sf::Vector2i(std::round(direction.x * m_mapSettings.scrollSpeed),
										  std::round(direction.y * m_mapSettings.scrollSpeed));

	 m_screenPos.x = std::max(std::min(m_screenPos.x, m_bottomRightPixel.x +
												  (int)std::round(m_mapSettings.viewPort.width/2.0)),
									  m_topLeftPixel.x - (int)std::round(m_mapSettings.viewPort.width/2.0));
	 m_screenPos.y = std::max(std::min(m_screenPos.y, m_bottomRightPixel.y +
												  (int)std::round(m_mapSettings.viewPort.height/2.0)),
									  m_topLeftPixel.y - (int)std::round(m_mapSettings.viewPort.height/2.0));

	 return true;
}

void HexMap::draw(sf::RenderWindow& window)
{
	 m_view.setCenter(sf::Vector2f(m_screenPos));
	 window.setView(m_view);

	 window.draw(m_tilemap);
}


bool InitIt::operator()(HexMap& hexes, Coords coords, int ring, va_list args)
{
	 return (hexes.initHex(coords));
}

bool MakeItType::operator()(HexMap& hexes, Coords coords, int ring, va_list args)
{
	 return (hexes.setType(coords, va_arg(args, int)));
}
