#pragma once


#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

template<typename Y>
Y& ElementOfCoords(std::vector< std::vector<Y> >& array, sf::Vector2i coords)
{
    return array[coords.x][coords.y];
}

void PrintVector(sf::Vector2i a, bool enter = false);
void PrintVector(sf::Vector2f a, bool enter = false);
double round_with_prec(double value, short precision);
std::string Sround_with_prec(double value, short precision);
int RandomI(int min, int max);
float RandomF(float min, float max, float step = 0.01f);
bool RandomB();
float degToRad(float angle);
float radToDeg(float angle);
std::vector<int> Range(int min, int max);
std::vector<int> Choose(std::vector<int> from, int many);
long long Power(long long base, int level);
int Power(int base, int level);
float Power(float base, int level);
long long Modulo(long long a, int b);
float Modulo(float a, float b);
double Modulo(double a, double b);
