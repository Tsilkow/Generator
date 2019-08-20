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
	 
	 int fpsLimit = 60;
	 sf::Vector2i resolution(1200, 900);

	 bool exit = false;
	 clock_t frameStart = clock();
	 clock_t frameEnd;
	 float fps;
	 int gameState=1;
	 sf::Vector2i screenPos(0, 0);
	 bool hasFocus = true;
	 
	 sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Generator");
	 sf::Event event;

	 sf::Texture plainsTex;
	 plainsTex.loadFromFile("data/hexPlains.png");
	 sf::Texture wastelandTex;
	 wastelandTex.loadFromFile("data/hexWasteland.png");
	 sf::Texture forestTex;
	 forestTex.loadFromFile("data/hexForest.png");
	 sf::Texture mountainTex;
	 mountainTex.loadFromFile("data/hexMountain.png");
	 sf::Texture urbanTex;
	 urbanTex.loadFromFile("data/hexUrban.png");

	 vector<sf::Texture> wallBorderTex(6);
	 for(int i = 0; i < 6; ++i)
	 {
		  wallBorderTex[i].loadFromFile("data/borderWall" + to_string(i) + ".png");
	 }
	 
	 hexSettings hexData = {
		  {{"plains", plainsTex, 3, true, 0},
			{"wasteland", wastelandTex, 4, false, 0},
			{"forest", forestTex, 4, false, 1},
			{"mountain", mountainTex, 0, false, 5},
			{"urban", urbanTex, 2, true, 2}},
		  {{"wall", wallBorderTex, false}},
		  120, // hexWidth
		  104, // hexHeight
		  30,  // hexQuarter
		  30,  // hexOffset
	 };

	 mapSettings mapData = {
		  5.f,
		  resolution,
		  sf::FloatRect(0.f, 0.f, 1.f, 1.f)
	 };

	 int count = 0;
	 
	 Map testMap(hexData, mapData, Coords(0, 0));

	 while(!exit)
	 {	  
		  if(((float)(frameEnd - frameStart))/((float)(CLOCKS_PER_SEC)) > 1.0/fpsLimit)
		  {
				++count;
				fps = 1.0/(((float)(frameEnd - frameStart))/((float)(CLOCKS_PER_SEC)));
				//cout << fps << endl;
				frameStart = clock();
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
						  break;
				}
		  }
		  frameEnd = clock();
		  if(count >= 3) exit = true;
	 }
	 window.close();
}
