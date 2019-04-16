#include "Sphere.h"
#include <math.h>
#include <iostream>

int radius, stack, sector;
const double PI = 3.141592653589793;

Sphere::Sphere(const float radius, const int stacks, const int sectors)
{
	::stack = stacks;
	::sector = sectors;
	::radius = radius;
}

void Sphere::points(float* vertices)
{
	const float sectorStep =  PI / sector;
	const float stackStep = 2 * PI / stack;

	std::cout << sectorStep << " " << stackStep << std::endl;

	float stackAngle, sectorAngle;
	float x, y, z, xy;
	int count = 0;
	for (int i = 0; i <= stack; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);
		for (int j = 0; j <= sector; ++j)
		{
			sectorAngle = j * sectorStep;
			// std::cout << stackAngle << " " << sectorAngle << std::endl;
			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);
			if (abs(x) < 1.0e-5) x = 0;
			if (abs(y) < 1.0e-5) y = 0;
			if (abs(z) < 1.0e-5) z = 0;
			vertices[count] = x;
			vertices[count + 1] = y;
			vertices[count + 2] = z;
			count += 3;;
		}
	}
}



Sphere::~Sphere()
{
	std::cout << "deleted this shit" << std::endl;
}
