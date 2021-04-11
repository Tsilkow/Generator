#include "commons.hpp"
#include <math.h>


void PrintVector(sf::Vector2i a, bool enter)
{
    std::cout << "{" << a.x << ", " << a.y << "}";
    if(enter) std::cout << std::endl;
}

void PrintVector(sf::Vector2f a, bool enter)
{
    std::cout << "{" << a.x << ", " << a.y << "}";
    if(enter) std::cout << std::endl;
}

double round_with_prec(double value, short precision)
{
    return std::round(value*std::pow(10, precision))/pow(10, precision);
}

std::string Sround_with_prec(double value, short precision)
{
    std::stringstream ss;
    ss.precision(precision);
    ss << std::fixed << value;
    return ss.str();
}

int RandomI(int min, int max)
{
    if(max == min) return min;
    return rand() % (max - min + 1) + min;
}

float RandomF(float min, float max, float step)
{
    return RandomI(std::round(min/step), std::round(max/step)) * step;
}

bool RandomB()
{
    return (rand()%2 == 0);
}

float degToRad(float angle)
{
    return angle * M_PI/180;
}

float radToDeg(float angle)
{
    return angle * 180/M_PI;
}

std::vector<int> Range(int min, int max)
{
    std::vector<int> result;

    for(int i = min; i <= max; ++i)
    {
	result.emplace_back(i);
    }

    return result;
}

std::vector<int> Choose(std::vector<int> from, int many)
{
    std::vector<int> result;
    int random;
	 
    for(int i = 0; i < many && from.size() > 0; ++i)
    {
	random = RandomI(0, from.size()-1);
	result.emplace_back(from[random]);
	from.erase(from.begin() + random);
    }

    return result;
}

long long Power(long long base, int level)
{
    if(level == 0) return 1;
    if(level == 1) return base;
    if(level % 2 == 1) return Power(base, level - 1) * base;
    return Power(base * base, level/2);
}

int Power(int base, int level)
{
    if(level == 0) return 1;
    if(level == 1) return base;
    if(level % 2 == 1) return Power(base, level - 1) * base;
    return Power(base * base, level/2);
}

float Power(float base, int level)
{
    if(level == 0) return 1;
    if(level == 1) return base;
    if(level % 2 == 1) return Power(base, level - 1) * base;
    return Power(base * base, level/2);
}

long long Modulo(long long a, int b)
{
    while(a < 0)
    {
	a += b;
    }
    while(a >= b)
    {
	a -= b;
    }

    return a;
}

float Modulo(float a, float b)
{
    while(a < 0)
    {
	a += b;
    }
    while(a >= b)
    {
	a -= b;
    }

    return a;
}

double Modulo(double a, double b)
{
    while(a < 0)
    {
	a += b;
    }
    while(a >= b)
    {
	a -= b;
    }

    return a;
}
