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

sf::Vector2i coordsToPixel(hexSettings set, Coords a)
{
	 int x = (set.hexWidth - set.hexQuarter)  * (-a.z());
	 int y = set.hexHeight * (a.y() - a.x())/2;

	 return sf::Vector2i(x, y);
}

/*Coords pixelToCoords(hexSettings set, sf::Vector2i a)
{
	 Coords result(a.y/set.hexWidth, )
}*/

Hex::Hex(hexSettings settings, Coords coords, int type, std::vector<int> borders):
	 m_settings(settings),
	 m_coords(coords),
	 m_mode(0)
{
	 // Representation intializations
	 setType(type);
	 for(int i=0; i<6; ++i)
	 {
		  m_borders.push_back(-1);
		  SHexBorders.push_back(sf::Sprite());
		  setBorder(i, borders[i]);
	 }
}

void Hex::setType(int newType)
{
	 int temp = m_mode;
	 m_type = newType;

	 if(m_type >= 0) Shex.setTexture(m_settings.hexTypes[m_type].texture);
	 
	 m_mode = -1;
	 switchMode(temp);
}

void Hex::setBorder(int border, int type)
{
	 m_borders[border] = type;
	 if(m_borders[border] >= 0) SHexBorders[border].setTexture(m_settings.wallTypes
																				  [m_borders[border]].textures[border]);
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

void Hex::draw(sf::RenderWindow& window, int mode)
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
}


Map::Map(hexSettings hSettings, mapSettings mSettings, Coords start):
	 m_hexSettings(hSettings),
	 m_mapSettings(mSettings),
	 m_start(start),
	 m_screenPos(0, 0),
	 m_mode(0),
	 m_nextRingToGen(0)
{
	 m_view = sf::View(sf::Vector2f(m_screenPos), sf::Vector2f(m_mapSettings.viewSize));
	 generateUpTo(6);
}

bool Map::initHex(Coords coords)
{
	 m_topLeftPixel.x     = std::min(coordsToPixel(m_hexSettings, coords).x, m_topLeftPixel.x    );
	 m_topLeftPixel.y     = std::min(coordsToPixel(m_hexSettings, coords).y, m_topLeftPixel.y    );
	 m_bottomRightPixel.x = std::max(coordsToPixel(m_hexSettings, coords).x, m_bottomRightPixel.x);
	 m_bottomRightPixel.y = std::max(coordsToPixel(m_hexSettings, coords).y, m_bottomRightPixel.y);
	 
	 m_hexes.insert(std::pair<Coords, Hex>(coords, Hex(m_hexSettings, coords, 0)));
	 return true;
}

bool Map::setType(Coords coords, int type)
{
	 if(doesHexExists(coords))
	 {
		  m_hexes.at(coords).setType(type);
		  return true;
	 }
	 return false;
}

bool Map::setBorder(Coords coords, int border, int type)
{
	 if(doesHexExists(coords))
	 {
		  m_hexes.at(coords).setBorder(border, type);
		  return true;
	 }
	 return false;
}

int Map::doInRing(Coords coords, int ring, bool allowEmpty, Processor& operation, ...)
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

void Map::generateUpTo(int upTo)
{
	 InitIt initProc;
	 MakeItType makeProc;
	 for(int i = m_nextRingToGen; i <= upTo; ++i)
	 {
		  doInRing(m_start, i, true , initProc);
		  doInRing(m_start, i, true , makeProc, 0);
	 }
	 m_nextRingToGen = upTo+1;
}

bool Map::moveScreen(sf::Vector2f direction)
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

void Map::draw(sf::RenderWindow& window)
{
	 std::cout << "(" << m_screenPos.x << ", " << m_screenPos.y << ")" << std::endl;
	 m_view.setCenter(sf::Vector2f(m_screenPos));
	 window.setView(m_view);
	 //for(auto h: m_hexes)
	 for(auto it = m_hexes.begin(); it != m_hexes.end(); ++it)
	 {
		  //std::cout << "(" << it->first.m_r << "; " << it->first.m_q << ")" << std::endl;
		  it->second.draw(window, m_mode);
	 }
}

bool InitIt::operator()(Map& hexes, Coords coords, int ring, va_list args)
{
	 return (hexes.initHex(coords));
}

bool MakeItType::operator()(Map& hexes, Coords coords, int ring, va_list args)
{
	 return (hexes.setType(coords, va_arg(args, int)));
}

