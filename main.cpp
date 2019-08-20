#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <stdarg.h>
#include <queue>

#include "tom.hpp"
#include "hex.hpp"

using namespace std;


int main()
{
	 srand(time(NULL));
	 
	 sf::Vector2i resolution(1200, 900);

	 bool exit = false;
	 clock_t frameStart = clock();
	 clock_t frameEnd;
	 float fps;
	 int gameState = 1;
	 sf::Vector2i screenPos(0, 0);
	 bool hasFocus = true;
	 
	 sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Generator");
	 window.setFramerateLimit(60);
	 sf::Event event;
	 
	 hexSettings hexData = {
		  {{"plains", 3, true, 0},
			{"wasteland", 4, false, 0},
			{"forest", 4, false, 1},
			{"mountain", 0, false, 5},
			{"urban", 2, true, 2}},
		  {{"wall", false}},
		  120, // hexWidth
		  104, // hexHeight
		  30,  // hexQuarter
		  30,  // hexOffset
		  "data/tileset.png"
	 };

	 mapSettings mapData = {
		  5.f,
		  resolution,
		  sf::FloatRect(0.f, 0.f, 1.f, 1.f)
	 };

	 int count = 0;
	 
	 HexMap testMap(hexData, mapData, Coords(0, 0));
	 sf::Clock clock;
	 float lastTime = 0;

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
													 /*case sf::Keyboard::Tab:
														++mapMode;
														mapMode %= TOModes;
														break;*/
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
