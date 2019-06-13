#include "objParser.h"
#include "model.h"
#include "sahKDTree.h"
#include "Vec.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}




void displayMe()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glutPostRedisplay();
}

void initializeWindow(int argc, char ** argv)
{
	GLFWwindow * window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	if (!window) glfwTerminate();

	if (!glfwInit())
		throw "Error initializing glfw";
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize glew" << std::endl;
		throw "Error initializing glew";
	}

}

float * convertToVerticesArr(const std::vector<Vec> & vertices)
{
	float * vert = new float[vertices.size() * 4];	
	for (int i = 0;i < vertices.size(); ++i)
	{
		vert[i * 4] = vertices[i].x; 
		vert[i * 4 + 1] = vertices[i].y;
		vert[i * 4 + 2] = vertices[i].z;
		vert[i * 4 + 3] = 1.0;
	}
	return vert;
}

unsigned int * convertToOrderArr(const std::vector<unsigned int> & order)
{
	unsigned int * ord = new unsigned int[order.size()];
	for (int i = 0; i < order.size(); ++i)
		ord[i] = order[i];	
	return ord;
}

void drawBoundingBox(float * vertices, unsigned int * order, GLuint & programId)
{
	float vert[] = {
	     0.5f,  0.5f, 0.0f,  // top right
	     0.5f, -0.5f, 0.0f,  // bottom right
	    -0.5f, -0.5f, 0.0f,  // bottom left
	    -0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
	    0, 1, 3,   // first triangle
	    1, 2, 3    // second triangle
	};  

	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLuint attribute_v_coord = glGetAttribLocation(programId, "aPos");
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);	
	
	GLuint ibo_elements;
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	
	glEnableVertexAttribArray(attribute_v_coord);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid *)(4 * sizeof(unsigned int)));
	//glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid *)(8 * sizeof(unsigned int)));
}



int main(int argc, char ** argv)
{
	model test;
	objParser::parseObjFile("bunny.obj", test);
	std::vector<Vec> vertices;
	std::vector<unsigned int> order;
	(test.tree)->getVertices((test.tree)->root, vertices, order);
	initializeWindow(argc, argv);
	float * vert = convertToVerticesArr(vertices);
	unsigned int * ord = convertToOrderArr(order);		
	//GLuint programId = LoadShaders("vertexShader", "fragmentShader");
	//glUseProgram(programId);
	//drawBoundingBox(vert, ord, programId);
	delete[] vert;
	delete[] ord;
}


