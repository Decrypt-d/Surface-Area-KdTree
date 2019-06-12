#pragma once
#include "Triangle.h"
#include <vector> 
#include "sahKDTree.h"

class model : public Geometry
{
	public:
	std::vector<Triangle> triangleMesh;
	std::vector<Triangle *> trianglePointer;
	sahKDTree * tree = nullptr;	

	void printMesh()
	{
		for (int i = 0; i < triangleMesh.size(); ++i)
		{
			std::cout << "v1: [ " << triangleMesh[i].v1.x <<  " " << triangleMesh[i].v1.y << " " << triangleMesh[i].v1.z << " ]  ";
			std::cout << "v2: [ " << triangleMesh[i].v2.x <<  " " << triangleMesh[i].v2.y << " " << triangleMesh[i].v2.z << " ]  ";
			std::cout << "v3: [ " << triangleMesh[i].v3.x <<  " " << triangleMesh[i].v3.y << " " << triangleMesh[i].v3.z << " ]";
			std::cout << std::endl;
		}
	}
	
	Box getTopBoundingBox()
	{
		Vec lowerLeft, topRight;
		lowerLeft.x = triangleMesh[0].v1.x;
		lowerLeft.y = triangleMesh[0].v1.y;
		lowerLeft.z = triangleMesh[0].v1.z;
		topRight.x = triangleMesh[0].v1.x;
		topRight.y = triangleMesh[0].v1.y;
		topRight.z = triangleMesh[0].v1.z;
		for (int i = 0; i < triangleMesh.size(); ++i)
			for (int j = 0; j < 3; ++j)
			{
				lowerLeft.x = std::min(lowerLeft.x, triangleMesh[i].vertices[j]->x);
				lowerLeft.y = std::min(lowerLeft.y, triangleMesh[i].vertices[j]->y);
				lowerLeft.z = std::min(lowerLeft.z, triangleMesh[i].vertices[j]->z);
				topRight.x = std::max(topRight.x, triangleMesh[i].vertices[j]->x);
				topRight.y = std::max(topRight.y, triangleMesh[i].vertices[j]->y);
				topRight.z = std::max(topRight.z, triangleMesh[i].vertices[j]->z);
			}

		return Box(lowerLeft, topRight);
	}
	
	void generateKdTree()
	{
		Box topBox = getTopBoundingBox();
		tree = new sahKDTree(1.0, 1.5, topBox, trianglePointer);
	}

	void updateTrianglePointers()
	{
		for (int i = 0; i < triangleMesh.size(); ++i)
			trianglePointer.push_back(&triangleMesh[i]);				
	}	

    virtual bool intersect(const Vec& p, const Vec& i, Vec& np, Vec& n) const
	{
		return false;
	}

    virtual Vec samplePoint(float& pdf) const
	{

		return Vec();
	}
	
	int getKdTreeDepth(kdTreeNode * r)
	{
		if (r == nullptr)
			return 0;
		if (r->isLeaf)
			std::cout << "Triangles Size: " << r->triangles.size() << std::endl;
		return 1 + std::max(getKdTreeDepth(r->left), getKdTreeDepth(r->right));
	}

	~model()
	{
		if (tree != nullptr)
			delete tree;
	}

    //virtual bool boundingBoxCheck(const Vec& p, const Vec& i) =0;
};
