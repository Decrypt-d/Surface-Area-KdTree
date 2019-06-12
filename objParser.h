#pragma once
#include <iostream>
#include <fstream> 
#include <sstream>
#include <vector>
#include "model.h"
#include "Vec.h"
#include "Triangle.h" 

class objParser
{
	public:
	static void parseObjFile(std::string file, model & newModel)
	{
		std::vector<Vec> vertices;
		std::ifstream infile(file.c_str());
		if (!infile.good())
			throw "Failed To Read File";
		std::string line;
		while(std::getline(infile, line))
		{
			if (line[0] == '#')
				continue;
			else if (line[0] == 'v')
			{
				std::string type, x, y, z;
				std::istringstream iss(line);
				iss >> type >> x >> y >> z;
				vertices.push_back(Vec(atof(x.c_str()), atof(y.c_str()), atof(z.c_str())));
			}		
			else if (line[0] == 'f')
			{
				std::string type, v1, v2, v3;
				std::istringstream iss(line);
				iss >> type >> v1 >> v2 >> v3;
				int index1 = atoi(v1.c_str()) - 1;
				int index2 = atoi(v2.c_str()) - 1;
				int index3 = atoi(v3.c_str()) - 1;
				newModel.triangleMesh.push_back(Triangle(vertices[index1], vertices[index2], vertices[index3]));
			}
		}	
		newModel.updateTrianglePointers();
		newModel.generateKdTree();
		std::cout << "KdTree Depth " << newModel.getKdTreeDepth(newModel.tree->root) << std::endl;
		infile.close();
	}
};
