#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <stdarg.h>
#include <utility>


struct Coords // x + y + z = 0 | x = r | y = q
{
	 int m_r; // row
	 int m_q; // collumn

	 Coords(int r, int q): m_r(r), m_q(q) {; }

	 sf::Vector2i rq() {return sf::Vector2i(m_r, m_q); }
	 sf::Vector3i xyz() {return sf::Vector3i(m_r, m_q, -m_r -m_q); }
	 int x() {return m_r; }
	 int y() {return m_q; }
	 int z() {return -m_r -m_q; }
	 void print() {std::cout << "(" << m_r << ", " << m_q << ", " << -m_r -m_q << ") " << std::endl; }
};

bool operator==(const Coords a, const Coords b);
bool operator!=(const Coords a, const Coords b);
bool operator>(const Coords a, const Coords b);
bool operator<(const Coords a, const Coords b);
void operator+=(Coords& a, Coords b);
Coords operator+(Coords a, Coords b);
Coords operator-(Coords a, Coords b);
Coords operator*(Coords a, int f);

Coords direction(int i);

class HexType
{
	 public:
	 std::string name;
	 sf::Texture texture;
	 int difficulty;
	 bool buildable;
	 int height;
};

class WallType
{
	 public:
	 std::string name;
	 std::vector<sf::Texture> textures;
	 bool traversible;
};

struct hexSettings
{
	 std::vector<HexType> hexTypes;
	 std::vector<WallType> wallTypes;
	 int hexWidth;
	 int hexHeight;
	 int hexQuarter;
	 int hexOffset;
};

struct mapSettings
{
	 float scrollSpeed;
	 sf::Vector2i viewSize;
	 sf::FloatRect viewPort;
};

sf::Vector2i coordsToPixel(hexSettings set, Coords a);
Coords pixelToCoords(sf::Vector2i a);

class Hex
{
	 private:
	 hexSettings m_settings;
	 Coords m_coords;
	 int m_type;
	 int m_mode;
	 int m_building;
	 std::vector<int> m_borders;
	 sf::Sprite Shex;
	 std::vector<sf::Sprite> SHexBorders;
	 
	 public:
	 Hex(hexSettings settings, Coords coords, int type, std::vector<int> borders = {-1, -1, -1, -1, -1, -1});

	 void setType(int type);

	 void setBorder(int border, int type);

	 bool switchMode(int newMode);

	 void draw(sf::RenderWindow& window, int mode);
	 
	 const Coords& getCoords()
		  {return m_coords; }
	 
	 const int& getType()
		  {return m_type; }

	 const int& getBuilding()
		  {return m_building; }

	 const int& getBorder(int border)
		  {return m_borders[border]; }
};

class Map;

struct Processor // Struct for passing functions to Map::doInRing
{
    virtual bool operator()(Map& hexes, Coords coords, int ring, va_list args) = 0;
};

struct MakeItType: public Processor // Function for Map::doInRing that changes type
{
	 bool operator()(Map& hexes, Coords coords, int ring, va_list args) override;
};

struct InitIt: public Processor // Function for Map:doInRing that initializes a hex
{
	 bool operator()(Map& hexes, Coords coords, int ring, va_list args) override;
};

class Map
{
	 private:
	 std::map<Coords, Hex> m_hexes;
	 hexSettings m_hexSettings;
	 mapSettings m_mapSettings;
	 Coords m_start;
	 int m_mode;
	 int m_nextRingToGen;
	 sf::Vector2i m_screenPos;
	 sf::Vector2i m_topLeftPixel;
	 sf::Vector2i m_bottomRightPixel;
	 sf::View m_view;

	 public:
	 Map(hexSettings hSettings, mapSettings mSettings, Coords start = {0, 0});
	 
	 bool initHex(Coords coords);
		  
	 bool setType(Coords coords, int type);

	 bool setBorder(Coords coords, int border, int type);

	 int doInRing(Coords coords, int ring, bool allowEmpty, Processor& operation, ...);

	 template <typename... T>
	 int doInHex(Coords coords, int TORings, bool allowEmpty, Processor& operation, T&&... args)
		  {
				for(int ring = 0; ring < TORings; ++ring)
				{
					 doInRing(coords, ring, allowEmpty, operation, std::forward<T>(args)...);
				}
		  }
	 
	 void generateUpTo(int upTo);

	 void switchMode(int newMode) {m_mode = newMode; }

	 bool moveScreen(sf::Vector2f direction);

	 void draw(sf::RenderWindow& window);

	 
	 bool doesHexExists(Coords coords) {return (m_hexes.count(coords) > 0); }
	 
	 Hex& getHex(Coords coords) {return m_hexes.find(coords)->second; } 
};
