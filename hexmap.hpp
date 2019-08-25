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

#include "hex.hpp"
#include "generator.hpp"


struct mapSettings // parameters that govern how the map is 
{
	 float scrollSpeed;
	 float zoomSpeed;
	 sf::Vector2i viewSize;
	 sf::FloatRect viewPort;
	 std::string filename;
	 int pixelsPerLevel;
	 float lightPerHeight;
};


class TileMap: public sf::Drawable, public sf::Transformable
{
	 public:
	 bool load(hexSettings& hSettings, mapSettings& mSettings, std::map<Coords, Hex>& hexes);
	 
	 private:
	 virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	 
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};
				  

class HexMap;

struct Processor // Struct for passing functions to Map::doInRing
{
    virtual bool operator()(HexMap& hexes, Coords coords, int ring, va_list args) = 0;
};

struct MakeItType: public Processor // Function for Map::doInRing that changes type
{
	 bool operator()(HexMap& hexes, Coords coords, int ring, va_list args) override;
};

/*struct InitIt: public Processor // Function for Map:doInRing that initializes a hex
{
	 bool operator()(HexMap& hexes, Coords coords, int ring, va_list args) override;
};*/

class HexMap
{
	 private:
	 std::map<Coords, Hex> m_hexes;
	 hexSettings m_hexSettings;
	 mapSettings m_mapSettings;
	 genSettings m_genSettings;
	 Coords m_start;
	 int m_mode;
	 int m_lastRing;
	 sf::Vector2i m_screenPos;
	 float m_zoom;
	 sf::Vector2i m_topLeftPixel;
	 sf::Vector2i m_bottomRightPixel;
	 sf::View m_view;
	 TileMap m_tilemap;

	 public:
	 HexMap(hexSettings& hSettings, mapSettings& mSettings, genSettings& gSettings, Coords start = {0, 0});
		  
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

	 void createMap();

	 void switchMode(int newMode) {m_mode = newMode; }

	 bool moveScreen(sf::Vector2f direction);
	 
	 bool zoomScreen(bool in);

	 void draw(sf::RenderWindow& window);

	 
	 bool doesHexExists(Coords coords) {return (m_hexes.count(coords) > 0); }
	 
	 Hex& getHex(Coords coords) {return m_hexes.find(coords)->second; } 
};
