#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <stdarg.h>
#include <queue>

#include "FastNoise/FastNoise.h"
#include "tom.hpp"
#include "hex.hpp"
#include "generator.hpp"
#include "hexmap.hpp"

using namespace std;


int main()
{
	 srand(time(NULL));
	 
	 sf::Vector2i resolution(1200, 900);
	 
	 hexSettings hexData = {
		  {{"water", 0, false, 0},     // hexTypes[0]
			{"plains", 3, true, 0},     // hexTypes[1]
			{"wasteland", 4, false, 0}, // hexTypes[2]
			{"forest", 4, false, 1},    // hexTypes[3]
			{"mountain", 0, false, 5},  // hexTypes[4]
			{"urban", 2, true, 2}},     // hexTypes[5]
		  {{"wall", false}},           // wallTypes[0]
		  120,                         // hexWidth
		  104,                         // hexHeight
		  30,                          // hexQuarter
		  30                           // hexOffset
	 };

	 std::cout << sf::Texture::getMaximumSize() << std::endl;

	 /*for(int i = 0; i < 6; ++i)
	 {
		  direction(i).print();
		  cout << " = px(" << coordsToPixelI(hexData, direction(i)).x << ", "
				 << coordsToPixelI(hexData, direction(i)).y << ") = ";
		  pixelToCoords(hexData, coordsToPixelI(hexData, direction(i))).print();
		  cout << endl;
	 }*/

	 mapSettings mapData = {
		  5.f,                               // scrollSpeed
		  0.01f,                             // zoomSpeed
		  resolution,                        // viewSize
		  sf::FloatRect(0.f, 0.f, 1.f, 1.f), // viewPort
		  "data/tileset2.png",                // filename
		  10,                                // pixelsPerLevel
		  0.05f                              // lightPerHeight
	 };

	 int heightSeed = rand();
	 int temperatureSeed = rand();
	 int moistureSeed = rand();
	 
	 genSettings genData = {
		  heightSeed,                                       // heightSeed
		  temperatureSeed,                                  // temperatureSeed
		  moistureSeed,                                     // moistureSeed
		  4.0f,                                             // heightFreq
		  32,                                               // heightOcts
		  2,                                                // heightAmplitude
		  -2.f,                                             // heightAtSea
		  1.f,                                              // heightAtLand
		  /*0,
		  1,
		  5,
		  4,
		  3,*/
		  48,                                               // mapRadius
		  24,                                               // landRadius
	 	  30,                                               // coastline
		  30,                                               // mountainsRadius
	 	  6,                                                // TORidges
		  0.1f,                                             // deviation
		  24,                                               // maxRidgeLength
		  5,                                                // mountainHeight
	  	  {{1.0f, 0.f , 0.f , 0.f  , 0.f  , 0.f },          // heightProb[0]
	  	   {0.8f, 0.2f, 0.f , 0.f  , 0.f  , 0.f },          //           [1]
	  	   {0.f , 0.9f, 0.1f, 0.f  , 0.f  , 0.f },          //           [2]
	  	   {0.f , 0.f , 0.9f, 0.1f , 0.f  , 0.f },          //           [3]
	  	   {0.f , 0.f , 0.7f, 0.2f , 0.1f , 0.f },          //           [4]
	  	   {0.f , 0.f , 0.f , 0.45f, 0.45f, 0.1f}},         //           [5]
		  12,                                               // TOBuildups
	     {0.f   , 0.1f, 0.25f, 0.5f, 0.8f, 0.99f, 0.999f}, // buildupProb
		  6,                                                // TOErosions
	     {0.001f, 0.4f, 0.5f, 0.75f, 0.9f, 0.99f, 0.999f}, // erosionProb
		  24,                                               // TORivers
		  1,                                                // minRiverSourceDistance
		  0.6f,                                             // meanderFreq
		  128,                                              // maxRiverCuts
		  24,                                               // maxLakeSize
		  Coords(0, 0)                                      // start
	 };

	 int count = 0;
	 
	 HexMap testMap(hexData, mapData, genData, Coords(0, 0));
	 sf::Clock clock;
	 float lastTime = 0;

	 bool exit = false;
	 int gameState = 1;
	 bool hasFocus = true;
	 
	 sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Generator");
	 window.setFramerateLimit(60);
	 sf::Event event;

	 while(!exit)
	 {
		  
		  ++count;
		  switch(gameState)
		  {
				case 0:
					 break;
				case 1:

					 while(window.pollEvent(event))
					 {
						  switch (event.type)
						  {
								case sf::Event::Closed:
									 exit = true;
									 break;
								case sf::Event::LostFocus:
									 hasFocus = false;
									 break;
								case sf::Event::GainedFocus:
									 hasFocus = true;
									 break;
								case sf::Event::KeyPressed:
									 if(hasFocus)
									 {
										  switch(event.key.code)
										  {
												case sf::Keyboard::Escape:
													 window.close();
													 exit = true;
													 break;
												case sf::Keyboard::Num1:
													 testMap.setMode(DisplayMode::Textured);
													 break;
												case sf::Keyboard::Num2:
													 testMap.setMode(DisplayMode::Elevation);
													 break;
												case sf::Keyboard::Num3:
													 testMap.setMode(DisplayMode::Temperature);
													 break;
												case sf::Keyboard::Num4:
													 testMap.setMode(DisplayMode::Moisture);
													 break;
										  }
									 }
									 break;
						  }
					 }

					 if(hasFocus)
					 {
						  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))
								testMap.moveScreen(sf::Vector2f( 0.f, -1.f));
						  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
								testMap.moveScreen(sf::Vector2f(+1.f,  0.f));
						  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))
								testMap.moveScreen(sf::Vector2f( 0.f,  1.f));
						  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))
								testMap.moveScreen(sf::Vector2f(-1.f,  0.f));
						  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
								testMap.zoomScreen(true);
						  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
								testMap.zoomScreen(false);
					 }

					 window.clear();

					 testMap.draw(window);

					 window.display();
					 /*float currentTime = clock.restart().asSeconds();
					 float fps = 1.f/(currentTime);
					 cout << fps << std::endl;*/
					 break;
		  }
	 }
	 window.close();
}
