#include "objParser.h"
#include "model.h"
#include "sahKDTree.h"
#include "Vec.h"
#include "Box.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

float windowResHorizontal = 1200;
float windowResVertical = 900;

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

GLFWwindow * initializeWindow(int argc, char ** argv)
{
	GLFWwindow * window;
	if (!glfwInit())
		throw "Error initializing glfw";

	window = glfwCreateWindow(windowResHorizontal, windowResVertical, "Compsci 114 Final sahTree Viewer", NULL, NULL);

	if (!window) glfwTerminate();

	glfwMakeContextCurrent(window);

	glewExperimental = GL_FALSE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize glew" << std::endl;
		throw "Error initializing glew";
	}
	return window;
}

void convertToVerticesArr(const std::vector<Vec> & vertices, float * & result, int & size)
{
	float * vert = new float[vertices.size() * 4];	
	for (int i = 0;i < vertices.size(); ++i)
	{
		++size;
		vert[i * 4] = vertices[i].x; 
		vert[i * 4 + 1] = vertices[i].y;
		vert[i * 4 + 2] = vertices[i].z;
		vert[i * 4 + 3] = 1.0f;
	}

	size *= 4;
	result = vert;	
}

void convertToOrderArr(const std::vector<unsigned int> & order, unsigned int * & result, int & size)
{
	unsigned int * ord = new unsigned int[order.size()];
	for (int i = 0; i < order.size(); ++i)
	{
		++size;
		ord[i] = order[i];	
	}

	result = ord;
}

float toRotate = 0;

void drawObj(float * vertices, unsigned int * order, int vertSize, int ordSize, GLuint & programId)
{
	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(float), vertices, GL_STATIC_DRAW);
	GLuint attribute_v_coord = glGetAttribLocation(programId, "aPos");
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glm::mat4 trans= glm::translate(glm::mat4(1.f), glm::vec3(0.4f, -3.0f, -8.f));
	glm::mat4 rot = glm::rotate(toRotate, glm::vec3(0, 1, 0));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(25, 25, 25));
	trans *= rot * scale;
	GLint unitrans = glGetUniformLocation(programId, "trans");
	glUniformMatrix4fv(unitrans, 1, GL_FALSE, glm::value_ptr(trans));

	GLint uniContext = glGetUniformLocation(programId, "context");
	glUniform1i(uniContext, 1);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), windowResHorizontal / windowResVertical, 1.0f, 20.0f);
	GLint uniproj = glGetUniformLocation(programId, "proj");
	glUniformMatrix4fv(uniproj, 1, GL_FALSE, glm::value_ptr(proj));

	GLuint ibo_elements;
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ordSize * sizeof(unsigned int), order, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glDrawElements(GL_TRIANGLES, ordSize, GL_UNSIGNED_INT, 0);	
}

void drawBoundingBox(float * vertices, unsigned int * order, int vertSize, int ordSize, GLuint & programId)
{
	if (toRotate > 360)
	   toRotate -= 360;	
	toRotate += 0.02;

	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(float), vertices, GL_STATIC_DRAW);
	GLuint attribute_v_coord = glGetAttribLocation(programId, "aPos");
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glm::mat4 trans= glm::translate(glm::mat4(1.f), glm::vec3(0.4f, -3.0f, -8.f));
	glm::mat4 rot = glm::rotate(toRotate, glm::vec3(0, 1, 0));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(25, 25, 25));
	trans *= rot * scale;
	GLint unitrans = glGetUniformLocation(programId, "trans");
	glUniformMatrix4fv(unitrans, 1, GL_FALSE, glm::value_ptr(trans));

	GLint uniContext = glGetUniformLocation(programId, "context");
	glUniform1i(uniContext, 0);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), windowResHorizontal / windowResVertical, 1.0f, 20.0f);
	GLint uniproj = glGetUniformLocation(programId, "proj");
	glUniformMatrix4fv(uniproj, 1, GL_FALSE, glm::value_ptr(proj));

	GLuint ibo_elements;
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ordSize * sizeof(unsigned int), order, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	
	glLineWidth(2);
	for (int i = 0; i < vertSize / 32; ++i)
	{
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid *) ((i * 16) * sizeof(unsigned int)));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid *) ((i * 16 * sizeof(unsigned int)) + (4 * sizeof(unsigned int))));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid *)((i * 16 * sizeof(unsigned int)) + (8 * sizeof(unsigned int))));
	}
}

void render(GLFWwindow * window)
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main(int argc, char ** argv)
{
	model test;
	objParser::parseObjFile("bunny.obj", test);
	GLFWwindow * window = initializeWindow(argc, argv);

	std::vector<Vec> vertices;
	std::vector<unsigned int> order;
	(test.tree)->getVertices((test.tree)->root, vertices, order);

	int vertSize = 0;
	float * vert;
	convertToVerticesArr(vertices, vert, vertSize);
	int ordSize = 0;
	unsigned int * ord;
	convertToOrderArr(order, ord, ordSize);
	
	//show rabbit
	int objVertSize = 0;
	float * objVert;
	convertToVerticesArr(test.mesh2, objVert, objVertSize);
	int objOrdSize = 0;
	unsigned int * objOrd;
	convertToOrderArr(test.elem, objOrd, objOrdSize);

	GLuint programId = LoadShaders("vertexShader", "fragmentShader");
	glUseProgram(programId);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		drawObj(objVert, objOrd, objVertSize, objOrdSize, programId);
		drawBoundingBox(vert, ord, vertSize, ordSize, programId);
		render(window);
	}

	delete[] vert;
	delete[] ord;
}


