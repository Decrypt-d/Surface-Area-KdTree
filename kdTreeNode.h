#pragma once
struct kdTreeNode
{
	kdTreeNode * left;
	kdTreeNode * right;
	Box boundingBox;
	bool isLeaf = false;
	std::vector<Triangle *> triangles;
	kdTreeNode(kdTreeNode * left, kdTreeNode * right, const Box & boundingBox, bool isLeaf) : left(left), right(right), isLeaf(isLeaf), boundingBox(boundingBox) {}
	kdTreeNode(kdTreeNode * left, kdTreeNode * right, bool isLeaf, std::vector<Triangle *> triangles) : left(left), right(right), isLeaf(isLeaf), triangles(triangles) {}
};

