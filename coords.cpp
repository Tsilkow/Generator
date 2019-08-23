#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>

#include "coords.hpp"


bool operator==(const Coords a, const Coords b)
{
	 return (a.m_r == b.m_r && a.m_q == b.m_q);
}

bool operator!=(const Coords a, const Coords b)
{
	 return !(a == b);
}

bool operator>(const Coords a, const Coords b)
{
	 if(a.priority() == b.priority()) return (a.m_r > b.m_r);
	 return (a.priority() < b.priority());
}

bool operator<(const Coords a,const  Coords b)
{
	 return (a != b && !(a > b));
}

void operator+=(Coords& a, Coords b)
{
	 a.m_r += b.m_r; a.m_q += b.m_q;
}

Coords operator+(Coords a, Coords b)
{
	 return Coords(a.m_r + b.m_r, a.m_q + b.m_q);
}

Coords operator-(Coords a, Coords b)
{
	 return Coords(a.m_r - b.m_r, a.m_q - b.m_q);
}

Coords operator*(Coords a, int f)
{
	 return Coords(a.x() * f, a.y() * f);
}

Coords direction(int i)
{
	 switch(i)
	 {
		  case 0:  return Coords( 0, +1); break;
		  case 1:  return Coords(+1,  0); break;
		  case 2:  return Coords(+1, -1); break;
		  case 3:  return Coords( 0, -1); break;
		  case 4:  return Coords(-1,  0); break;
		  case 5:  return Coords(-1, +1); break;
		  default: return Coords( 0,  0); break;
	 }
}

sf::Vector2i coordsToPixelI(hexSettings& set, Coords a)
{
	 int x = std::round((set.hexWidth - set.hexQuarter)  * a.x() - set.hexWidth/2.f);
	 int y = std::round(set.hexHeight * (a.z() - a.y())/2.f - set.hexHeight/2.f);

	 return sf::Vector2i(x, y);
}

sf::Vector2f coordsToPixelF(hexSettings& set, Coords a)
{
	 float x = std::round((set.hexWidth - set.hexQuarter)  * a.x() - set.hexWidth/2.f);
	 float y = std::round(set.hexHeight * (a.z() - a.y())/2.f - set.hexHeight/2.f);

	 return sf::Vector2f(x, y);
}

sf::Vector2i coordsToCenter(hexSettings& set, Coords a)
{
	 int x = (set.hexWidth - set.hexQuarter)  * a.x();
	 int y = std::round(set.hexHeight * (a.z() - a.y())/2.f);

	 return sf::Vector2i(x, y);
}

Coords pixelToCoords(hexSettings& set, sf::Vector2i a)
{
	 int r = std::round(((float)a.x + set.hexWidth/2.f) / (set.hexWidth - set.hexQuarter));
	 int q = std::round((((float)a.y + set.hexHeight/2.f) / set.hexHeight + r/2.f)) * -1;

	 return Coords(r, q);
}

Coords pixelToCoords(hexSettings& set, sf::Vector2f a)
{
	 int r = std::round(((float)a.x + set.hexWidth/2) / (set.hexWidth - set.hexQuarter));
	 int q = std::round((((float)a.y + set.hexHeight/2.f) / set.hexHeight + r/2.f)) * -1;

	 return Coords(r, q);
}

Coords centerToCoords(hexSettings& set, sf::Vector2i a)
{
	 int r = std::round(((float)a.x) / (set.hexWidth - set.hexQuarter));
	 int q = std::round((((float)a.y) / set.hexHeight + r/2.f)) * -1;

	 return Coords(r, q);
}

Coords distance(const Coords& a, const Coords& b)
{
	 return Coords(b.m_r - a.m_r, b.m_q - a.m_q);
}

int length(Coords a)
{
	 return std::max(std::max(std::abs(a.x()), std::abs(a.y())), std::abs(a.z()));
}
