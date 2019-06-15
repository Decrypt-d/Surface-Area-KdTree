#pragma once
#include "Triangle.h"
#include "Box.h"
#include "splitPlane.h"
#include <limits>
#include <algorithm>
#include "kdTreeNode.h"

class sahKDTree
{
	//KT and KI

	public:
		float KT;
		float KI;
		enum Side {Left, Right, Unknown};
		kdTreeNode * root;
		
	private:
		float lambda(int NL, int NR) const
		{
			return (NL == 0 || NR == 0) ? 0.8f : 1.0f;	
		}

		float splitScore(float probLeft, float probRight, int NL, int NR) const
		{
			return lambda(NL, NR) * (KT + KI * (probLeft * NL + probRight * NR));
		}	

		float PVsubGivenV(const Box & vSub, const Box & V)
		{
			return vSub.surfaceArea() / V.surfaceArea();
		}

		void splitBox(splitPlane & p, Box & V, Box & VL, Box & VR)
		{
			VL = V;
			VR = V;
			VL.setMax(p.pk, p.pe);
			VR.setMin(p.pk, p.pe);
		}

		void SAH(splitPlane & p, Box V, int NL,int NP,  int NR, float & C, Side & pSide)
		{
			Box VL, VR;
			splitBox(p, V, VL, VR);	
			float pL = PVsubGivenV(VL, V);
			float pR = PVsubGivenV(VR, V);
			float CPL = splitScore(pL, pR, NL + NP, NR);
			float CPR = splitScore(pL, pR, NL, NR + NP);
			if (CPL < CPR)
			{
				pSide = Left;
				C = CPL;
			}
			else
			{
				pSide = Right;
				C = CPR;
			}
		}

		struct Event
		{
			enum EventType {endingOnPlane = 0, lyingOnPlane=1, startingOnPlane=2};
			Triangle * t;
			splitPlane p;
			EventType type;
			Event(Triangle * t, int k, float pe, EventType type) : p(splitPlane(k, pe)), type(type), t(t){}
			bool operator < (const Event & second)
			{
				return (p.pe < second.p.pe) || (p.pe == second.p.pe && type < second.type);
			}
		};

		Box clipTriangleToBox(Triangle * t, const Box & V)
		{
	   		Box tB = t->getBoundingBox();
			if (tB.lowerLeft.x < V.lowerLeft.x)
				tB.lowerLeft.x = V.lowerLeft.x;
			
			if (tB.topRight.x > V.topRight.x)
				tB.topRight.x = V.topRight.x;

			if (tB.lowerLeft.y < V.lowerLeft.y)
				tB.lowerLeft.y = V.lowerLeft.y; 	

			if (tB.topRight.y > V.topRight.y) 
				tB.topRight.y = V.topRight.y;

			if (tB.lowerLeft.z < V.lowerLeft.z)
				tB.lowerLeft.z = V.lowerLeft.z; 	

			if (tB.topRight.z > V.topRight.z) 
				tB.topRight.z = V.topRight.z;
			return tB;
		}

		bool terminate(int N, float minCV) const
		{
			return minCV > KI * N;
		}
		
		splitPlane findPlane(std::vector<Triangle *> & T, Box & V, float & cP, splitPlane & pHat, Side & pHatSide)
		{
			cP = std::numeric_limits<float>::max();
			for (int k = 0; k < 3; ++k)
			{
				std::vector<Event> E;
				for (int i = 0; i < T.size(); ++i)
				{
					Box tBox = clipTriangleToBox(T[i], V);
					if (tBox.isPlanar())
						E.push_back(Event(T[i], k, tBox.getMinValue(k), Event::lyingOnPlane));
					else
					{
						E.push_back(Event(T[i], k, tBox.getMinValue(k), Event::startingOnPlane));
						E.push_back(Event(T[i], k, tBox.getMaxValue(k), Event::endingOnPlane));
					}
				}	

				std::sort(E.begin(), E.end());

				int NL = 0, NP = 0, NR = T.size();
				
				for (int i = 0; i < E.size(); ++i)
				{
					splitPlane p = E[i].p;
					int pPos = 0, pNeg = 0, pSame = 0;
					while (i < E.size() && E[i].p.pe == p.pe && E[i].type == Event::lyingOnPlane)
					{
						++i;
						++pSame;
					}
					while (i < E.size() && E[i].p.pe == p.pe && E[i].type == Event::startingOnPlane)
					{
						++i;
						++pPos;
					}
					while (i < E.size() && E[i].p.pe == p.pe && E[i].type == Event::endingOnPlane)
					{
						++i;
						++pNeg;
					}

					NP = pSame;
					NR -= (pSame + pNeg);
					float Cnext;
					Side pSide;
					SAH(p, V, NL, NP, NR, Cnext, pSide);

					if (Cnext < cP)
					{
						cP = Cnext;
						pHat = p;
						pHatSide = pSide;
					}	

					NL += (pPos + pSame);
					NP = 0;
				}			
			}
		}

		void splitTrianglesIntVoxel(std::vector<Triangle *> & T, splitPlane & p, Side & pSide, std::vector<Triangle *> & left, std::vector<Triangle *> & right)
		{
			for (int i = 0; i < T.size(); ++i)
			{
				Box tB = T[i]->getBoundingBox();
				if (tB.getMinValue(p.pk) == p.pe && tB.getMaxValue(p.pk) == p.pe)
				{
					if (pSide == Left)
						left.push_back(T[i]);
					else if (pSide == Right)
						right.push_back(T[i]);
					else
						throw "Wrong Side";
				}
				else 
				{
					if (tB.getMinValue(p.pk) < p.pe)
						left.push_back(T[i]);
					if (tB.getMaxValue(p.pk) > p.pe)
						right.push_back(T[i]);
				}
			}
		}

		void deleteKdTreeHelper(kdTreeNode * & root)
		{
			if (root == nullptr)
				return;
			deleteKdTreeHelper(root->left);
			deleteKdTreeHelper(root->right);
			delete root;
		}

		kdTreeNode * buildTree(std::vector<Triangle *> T, Box &V, const splitPlane & prev_p)
		{
			splitPlane p(0, 0);
			Side pSide;	
			float Cp;
			findPlane(T, V, Cp, p, pSide);
			if (terminate(T.size(), Cp) || p == prev_p)
				return new kdTreeNode(nullptr, nullptr, true, T);
			std::vector<Triangle *> left;
			std::vector<Triangle *> right;
			Box VL, VR;
			splitBox(p, V, VL, VR);
			splitTrianglesIntVoxel(T, p, pSide, left, right);
			return new kdTreeNode(buildTree(left, VL, p), buildTree(right, VR, p), V, false);
		}

	public:
		sahKDTree(float kT, float kI, Box & topBox, std::vector<Triangle *> & T) : KT(kT), KI(kI) 
		{
			root = buildTree(T, topBox, splitPlane(0, std::numeric_limits<float>::max()));
		} 

		void getVertices(kdTreeNode * root, std::vector<Vec> & vertices, std::vector<unsigned int> & order)
		{
			if (root == nullptr || root->isLeaf)
				return;
			(root->boundingBox).getVertices(vertices, order);
			getVertices(root->left, vertices, order);
			getVertices(root->right, vertices, order);
		}

		~sahKDTree()
		{
			deleteKdTreeHelper(root);
		}
};
