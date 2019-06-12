#include "objParser.h"
#include "model.h"
#include "sahKDTree.h"

int main()
{
	model test;
	objParser::parseObjFile("bunny.obj", test);
}
