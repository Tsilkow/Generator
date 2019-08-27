#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>


struct Coords 
{
	 // This struct is for coordinates storage. It stores axial coordinates, but easily converts them to
	 // cube ones.
	 // if 0 degrees starts on the right, then:
	 // +x = 0 deg, +y = 120 deg, +z = 240 deg
	 int m_r; // row
	 int m_q; // collumn

	 Coords() {; }
	 Coords(int r, int q): m_r(r), m_q(q) {; }

	 sf::Vector2i rq() {return sf::Vector2i(m_r, m_q); } // axial coordinates
	 sf::Vector3i xyz() {return sf::Vector3i(m_r, m_q, -m_r -m_q); } // cube coordinates
	 const int x() const {return m_r; } // x from cube coords
	 const int y() const {return m_q; } // y from cube coords
	 const int z() const {return -m_r -m_q; } // z from cub coords
	 const int priority() const {return 2 * m_q + m_r; }
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
	 int difficulty; // difficulty of traversing
	 bool buildable;
	 int height;
};

class WallType
{
	 public:
	 std::string name;
	 bool traversible;
};

struct hexSettings // parameters to convert to and from Coords system
{
	 std::vector<HexType> hexTypes;
	 std::vector<WallType> wallTypes;
	 int hexWidth;
	 int hexHeight;
	 int hexQuarter;
	 int hexOffset;
};

sf::Vector2i coordsToPixelI(hexSettings& set, Coords a);
sf::Vector2f coordsToPixelF(hexSettings& set, Coords a);
sf::Vector2i coordsToCenter(hexSettings& set, Coords a);
Coords pixelToCoords(hexSettings& set, sf::Vector2i a);
Coords pixelToCoords(hexSettings& set, sf::Vector2f a);
Coords centerToCoords(hexSettings& set, sf::Vector2i a);

Coords distance(const Coords& a, const Coords& b); // coords of b shown with a as (0, 0, 0)
int length(Coords a); // length of a



class Hex
{
	 private:
	 hexSettings m_settings;
	 Coords m_coords;
	 int m_type;
	 int m_height;
	 int m_temperature;
	 int m_moisture;
	 std::vector<int> m_water;
	 int m_building;
	 std::vector<int> m_borders;
	 
	 public:
	 //Hex() {; }
		  
	 Hex(hexSettings& settings, Coords coords, int type = 0, int height = 0, int temperature = 0,
		  int moisture = 0, std::vector<int> water = {}, std::vector<int> borders = {-1, -1, -1, -1, -1, -1});

	 void setType(int type);
	 
	 void setHeight(int height);
	 
	 void setTemperature(int temperature);
		  
	 void setMoisture(int moisture);
	 
	 void addWater(int newWater);

	 void setBorder(int border, int type);

	 //void draw(sf::RenderWindow& window, int mode);
	 
	 const Coords& getCoords()
		  {return m_coords; }
	 
	 const int& getType()
		  {return m_type; }

	 const int& getHeight()
		  {return m_height; }

	 const int& getTemperature()
		  {return m_temperature; }

	 const int& getMoisture()
		  {return m_moisture; }

	 const std::vector<int>& getWater()
		  {return m_water; }

	 const int& getBuilding()
		  {return m_building; }

	 const int& getBorder(int border)
		  {return m_borders[border]; }
};
