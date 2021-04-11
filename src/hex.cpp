#include "hex.hpp"


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



Hex::Hex(hexSettings& settings, Coords coords, int type, int height, int temperature, int moisture,
			int drainage, std::vector<int> water, std::vector<int> borders):
	 m_settings(settings),
	 m_coords(coords),
	 m_water(water)
{
	 // Representation intializations
	 setType(type);
	 setHeight(height);
	 setTemperature(temperature);
	 setMoisture(moisture);
	 setDrainage(drainage);
	 
	 for(int i=0; i<6; ++i)
	 {
		  m_borders.push_back(-1);
		  setBorder(i, borders[i]);
	 }
}

void Hex::setType(int type)
{
	 m_type = type;
}

void Hex::setHeight(int height)
{
	 m_height = height;
}

void Hex::setTemperature(int temperature)
{
	 m_temperature = temperature;
}

void Hex::setMoisture(int moisture)
{
	 m_moisture = moisture;
}

void Hex::setDrainage(int drainage)
{
	 m_drainage = drainage;
}

void Hex::addWater(int water)
{
	 m_water.emplace_back(water);
}

void Hex::setBorder(int border, int type)
{
	 m_borders[border] = type;
	 //if(m_borders[border] >= 0) SHexBorders[border].setTexture(m_settings.wallTypes
	 //																			  [m_borders[border]].textures[border]);
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
