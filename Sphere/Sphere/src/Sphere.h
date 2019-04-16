#pragma once
class Sphere
{

public:

	struct point
	{
		float x, y, z;
	};

	Sphere(float radius, const int stacks, const int sectors);
	~Sphere();
	void points(float* vertices);
};

