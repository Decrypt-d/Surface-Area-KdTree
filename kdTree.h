#include <random>
#include <vector>
#include "splitPlane.h"
#include "kdTreeNode.h"

class kdTree
{
	private:
		void splitVoxel(splitPlane & p, Box & V, Box & VL, Box & VR)
		{
			VL = V;
			VR = V;
			VL.setMin(p.pk, p.pe);			
			VR.setMax(p.pk, p.pe);
		}	

		float getSplitValue(int dimension, Box & V)
		{
			if (dimension == 0)
				return (V.lowerLeft.x + V.topRight.x) / 2.0;
			else if (dimension == 1)
				return (V.lowerLeft.y + V.topRight.y) / 2.0;
			else 
				return (V.lowerLeft.z + V.topRight.z) / 2.0;
		}

		splitPlane findPlane(int depth, Box & V)
		{
			int dimension = depth % 3;
			int pe = getSplitValue(dimension, V);
			return splitPlane(dimension, pe);
		}


		void deleteHelper(kdTreeNode * node)
		{
			if (node == nullptr)
				return;
			if (node->left != nullptr)
				deleteHelper(node->left);
			if (node->right != nullptr)
				deleteHelper(node->right);
			delete node;
		}	

		void clipTriangle(Box & V, Triangle * t, const splitPlane & p, std::vector<Triangle *> & left, std::vector<Triangle *> & right)
		{
			Box b = t->getBoundingBox();
			if (p.pk == 0)
			{
				if (b.lowerLeft.x <= p.pe)
					left.push_back(t);
				if (b.topRight.x >= p.pe)
					right.push_back(t);
			}
			else if (p.pk == 1)
			{
				if (b.lowerLeft.y <= p.pe)
					left.push_back(t);
				if (b.topRight.y >= p.pe)
					right.push_back(t);
			}
			else if (p.pk == 2)
			{
				if (b.lowerLeft.z <= p.pe)
					left.push_back(t);
				if (b.topRight.z >= p.pe)
					right.push_back(t);
			}
		}

		kdTreeNode * buildKdTree(std::vector<Triangle *> triangles, Box & V, int depth)
		{
			if (triangles.size() == 0)
				return nullptr;
			if (depth > maxDepth)
				return new kdTreeNode(nullptr, nullptr, true, triangles);
			splitPlane p = findPlane(depth, V);
			Box VL, VR;
			splitVoxel(p, V, VL, VR);
			std::vector<Triangle *> left;
			std::vector<Triangle *> right;
			for (int i = 0; i < triangles.size(); ++i)
				clipTriangle(V, triangles[i], p, left, right);
			return new kdTreeNode(buildKdTree(left, VL, depth + 1), buildKdTree(right, VR, depth + 1), false);
		}

	public:
		kdTreeNode * root;
		int maxDepth;
		
		kdTree(int maxDepth, std::vector<Triangle *> triangles, Box & topBox) : maxDepth(maxDepth)
		{
			root = buildKdTree(triangles, topBox, 0);
		}	

		~kdTree()
		{
			deleteHelper(root);
		}
};
