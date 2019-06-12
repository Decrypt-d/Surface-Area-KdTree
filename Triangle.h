#pragma once
#include "Vec.h"
#include "Geometry.h"
#include "Box.h"

class Triangle : public Geometry
{
	public:
	Vec v1, v2, v3;
	const Vec * vertices[3];
	Triangle(const Vec & v1, const Vec & v2, const Vec & v3): v1(v1), v2(v2), v3(v3) 
	{
		vertices[0] = &v1;
		vertices[1] = &v2;
		vertices[2] = &v3;
	}

	Box getBoundingBox()
	{
		float leftX = std::min(std::min(v1.x, v2.x), v3.x);				
		float rightX = std::max(std::max(v1.x, v2.x), v3.x);
		float lowerY = std::min(std::min(v1.y, v2.y), v3.y);
		float upperY = std::max(std::max(v1.y, v2.y), v3.y);
		float nearZ = std::min(std::min(v1.z, v2.z), v3.z);
		float farZ = std::max(std::max(v1.z, v2.z), v3.z);
		return Box(Vec(leftX, lowerY, nearZ), Vec(rightX, upperY, farZ));
	}

    virtual bool intersect(const Vec& p, const Vec& i, Vec& np, Vec& n) const
	{
		return false;
	}

    virtual Vec samplePoint(float& pdf) const
	{

		return Vec();
	}
};
