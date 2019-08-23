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

struct hexSettings
{
	 std::vector<HexType> hexTypes;
	 std::vector<WallType> wallTypes;
	 int hexWidth;
	 int hexHeight;
	 int hexQuarter;
	 int hexOffset;
	 std::string filename;
};

sf::Vector2i coordsToPixelI(hexSettings& set, Coords a);
sf::Vector2f coordsToPixelF(hexSettings& set, Coords a);
sf::Vector2i coordsToCenter(hexSettings& set, Coords a);
Coords pixelToCoords(hexSettings& set, sf::Vector2i a);
Coords pixelToCoords(hexSettings& set, sf::Vector2f a);
Coords centerToCoords(hexSettings& set, sf::Vector2i a);

Coords distance(const Coords& a, const Coords& b); // coords of b shown with a as (0, 0, 0)
int length(Coords a); // length of a
