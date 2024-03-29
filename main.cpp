// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Modifications by: Shannon TJ 10101385
// Date:    Fall 2016
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "camera.h"

#define PI 3.14159265359

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

vec2 mousePos;
bool mousePressed = false;

bool pause = false;
bool restart = false;

Camera cam;

vec3 camPos;
vec3 camDir;
vec3 camRight;
vec3 camUp;

float x = 0.f;
float y = 0.f;
float speed = 1.f;

mat3 rotateX = mat3(1.f);
mat3 rotateY = mat3(1.f);

GLFWwindow* window = 0;

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

// handles keyboard input events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);


   /* if(key == GLFW_KEY_W)
    	cam.pos += cam.dir*move;
    	
    else if(key == GLFW_KEY_S)
    	cam.pos -= cam.dir*move;
    	
    else if(key == GLFW_KEY_D)
    	cam.pos += cam.right*move;
    	
    else if(key == GLFW_KEY_A)
    	cam.pos -= cam.right*move;
    	
     else if(key == GLFW_KEY_E)
    	cam.pos += cam.up*move;
    	
    else if(key == GLFW_KEY_Q)
    	cam.pos -= cam.up*move;*/
    	
    //Move forward	
    if(key == GLFW_KEY_W)
    {
		if(camPos.y > 5)
			camPos.y = camPos.y - 0.05f;
	}
    
    //Move backward	
    else if(key == GLFW_KEY_S)
    {
		if(camPos.y < 9)
			camPos.y = camPos.y + 0.05f;
	}
    
    //Go right	
    else if(key == GLFW_KEY_D)	
		x = x - 0.05f;
    
    //Go left	
    else if(key == GLFW_KEY_A)
		x = x + 0.05f;
	
	//Go up	
    else if(key == GLFW_KEY_Q)
    	y = y - 0.05f;
	
	//Go down
    else if(key == GLFW_KEY_E)
    	y = y + 0.05f;
	
	//Pause or unpause the animation
	else if(key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if(pause)
			pause = false;
		else
			pause = true;
	}
	
	//Restart the animation
	else if(key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		restart = true;
	}
    	
    //Decrease speed
	else if(key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		if(speed > 0.5f)
			speed = speed - 0.5f;
	}
	
	//Increase speed 
	else if(key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		if(speed < 4)
			speed = speed + 0.5f;
	}
    				
    	
    rotateY = mat3(1.f, 0.f, 0.f,
				   0.f, cos(PI*y), -sin(PI*y),
				   0.f, sin(PI*y), cos(PI*y));
    
    rotateX = mat3(cos(PI*x), -sin(PI*x), 0.f,
				   sin(PI*x), cos(PI*x), 0.f,
			       0.f, 0.f, 1.f);
	
	cam.pos = rotateX*rotateY*camPos;
	cam.right = rotateX*rotateY*camRight;	
	cam.dir	= rotateX*rotateY*camDir;
	cam.up = rotateX*rotateY*camUp;   


						
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) )
		mousePressed = !mousePressed;
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	/*int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);
	vec2 diff = newPos - mousePos;
	
	if(mousePressed)
		cam.rotateCamera(-diff.x, diff.y);

	mousePos = newPos;*/
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glViewport(0, 0, width, height);
}





//==========================================================================
// TUTORIAL STUFF


//vec2 and vec3 are part of the glm math library. 
//Include in your own project by putting the glm directory in your project, 
//and including glm/glm.hpp as I have at the top of the file.
//"using namespace glm;" will allow you to avoid writing everyting as glm::vec2
vector<vec3> points;
vector<vec2> uvs;

//Structs are simply acting as namespaces
//Access the values like so: VAO::LINES
struct VAO{
	enum {GEOMETRY=0, COUNT};		//Enumeration assigns each name a value going up
										//LINES=0, COUNT=1
};

struct VBO{
	enum {POINTS=0, NORMALS, UVS, INDICES, COUNT};	//POINTS=0, COLOR=1, COUNT=2
};

struct SHADER{
	enum {DEFAULT=0, COUNT};		//LINE=0, COUNT=1
};

GLuint vbo [VBO::COUNT];		//Array which stores OpenGL's vertex buffer object handles
GLuint vao [VAO::COUNT];		//Array which stores Vertex Array Object handles
GLuint shader [SHADER::COUNT];		//Array which stores shader program handles

//Gets handles from OpenGL
void generateIDs()
{
	glGenVertexArrays(VAO::COUNT, vao);		//Tells OpenGL to create VAO::COUNT many
														// Vertex Array Objects, and store their
														// handles in vao array
	glGenBuffers(VBO::COUNT, vbo);		//Tells OpenGL to create VBO::COUNT many
													//Vertex Buffer Objects and store their
													//handles in vbo array
}

//Clean up IDs when you're done using them
void deleteIDs()
{
	for(int i=0; i<SHADER::COUNT; i++)
	{
		glDeleteProgram(shader[i]);
	}
	
	glDeleteVertexArrays(VAO::COUNT, vao);
	glDeleteBuffers(VBO::COUNT, vbo);	
}


//Describe the setup of the Vertex Array Object
bool initVAO()
{
	glBindVertexArray(vao[VAO::GEOMETRY]);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO::POINTS] );		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);
	
	glEnableVertexAttribArray(2);		//Tell opengl you're using layout attribute 1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec2),
		(void*)0
		);	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const vector<vec3>& points, const vector<vec3> normals, 
				const vector<vec2>& uvs, const vector<unsigned int>& indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::POINTS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*points.size(),	//Size of data in array (in bytes)
		&points[0],							//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
												//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*normals.size(),	//Size of data in array (in bytes)
		&normals[0],							//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
												//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vec2)*uvs.size(),
		&uvs[0],
		GL_STATIC_DRAW
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*indices.size(),
		&indices[0],
		GL_STATIC_DRAW
		);

	return !CheckGLErrors("loadBuffer");	
}

//Compile and link shaders, storing the program ID in shader array
bool initShader()
{	
	string vertexSource = LoadSource("vertex.glsl");		//Put vertex file text into string
	string fragmentSource = LoadSource("fragment.glsl");		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	shader[SHADER::DEFAULT] = LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array

	return !CheckGLErrors("initShader");
}

//For reference:
//	https://open.gl/textures
GLuint createTexture(const char* filename)
{
	int components;
	GLuint texID;
	int tWidth, tHeight;

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &tWidth, &tHeight, &components, 0);
	
	if(data != NULL)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		if(components==3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(components==4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Clean up
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texID;
	} 
	
	return 0;	//Error
}

//Use program before loading texture
//	texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
bool loadTexture(GLuint texID, GLuint texUnit, GLuint program, const char* uniformName)
{
	glUseProgram(program);
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
	
	GLuint uniformLocation = glGetUniformLocation(program, uniformName);
	
	if(texUnit == GL_TEXTURE0)
		glUniform1i(uniformLocation, 0);
		
	else if(texUnit == GL_TEXTURE1)
		glUniform1i(uniformLocation, 1);
		
	else if(texUnit == GL_TEXTURE2)
		glUniform1i(uniformLocation, 2);
		
	else if(texUnit == GL_TEXTURE3)
		glUniform1i(uniformLocation, 3);
		
	return !CheckGLErrors("loadTexture");
}


//u parameterizes in the big circle, v parameterizes in the little circle
//c_r is the circle radius, t_r is the tube radius
void generateSphere(vector<vec3>& positions, vector<vec3>& normals, 
					vector<vec2>& uvs, vector<unsigned int>& indices,
					float c_r, float t_r, 
					int uDivisions, int vDivisions)
{
					
					float uStep = 1.f/((float)(uDivisions-1)*2);
					float vStep = 1.f/(float)(vDivisions-1);
					
					float u = 0.25f;
					
					for(int i =0; i<uDivisions; i++)
					{
						vec3 center = vec3(c_r*cos(2.f*PI*u),0.f,c_r*sin(2.f*PI*u));
						
						float v = 0.f;
						
						
						for(int j=0;j<vDivisions;j++)
						{
							vec3 pos = vec3(c_r*cos(2.f*PI*u)*sin(2.f*PI*v), c_r*cos(2.f*PI*u)*cos(2.f*PI*v), c_r*sin(2.f*PI*u));
						
							vec3 normal = normalize(center - pos);
							positions.push_back(pos);
							normals.push_back(normal);
							
							
							//SOURCE USED: https://en.wikipedia.org/wiki/UV_mapping
							//uvs.push_back(vec2((0.5f+atan2(-normal.z,-normal.x))/2*PI, (0.5f-asin(-normal.y))/PI));
							 
							uvs.push_back(vec2(v,(-u+0.25f)*2));
							v += vStep;
						}
						
						u += uStep;
					}


				for(int i=0;i<uDivisions-1;i++)
				{
					for(int j =0; j<vDivisions-1;j++)
					{
						unsigned int p00 = i*vDivisions+j;
						unsigned int p01 = i*vDivisions+j+1;
						unsigned int p10 = (i+1)*vDivisions+j;
						unsigned int p11 = (i+1)*vDivisions+j+1;
						
						indices.push_back(p00);
						indices.push_back(p01);
						indices.push_back(p10);
						indices.push_back(p11);
						
						/*indices.push_back(p01);
						indices.push_back(p10);
						indices.push_back(p11);*/
					}
				}
	
}

//Initialization
void initGL()
{

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Only call these once - don't call again every time you change geometry
	generateIDs();		//Create VertexArrayObjects and Vertex Buffer Objects and store their handles
	initShader();		//Create shader and store program ID

	initVAO();			//Describe setup of Vertex Array Objects and Vertex Buffer Object
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

//Draws buffers to screen
void render(Camera* cam, mat4 perspectiveMatrix, mat4 modelview, int startElement, int numElements)
{
	
	//Don't need to call these on every draw, so long as they don't change
	glUseProgram(shader[SHADER::DEFAULT]);		//Use LINE program
	glBindVertexArray(vao[VAO::GEOMETRY]);		//Use the LINES vertex array

	glUseProgram(shader[SHADER::DEFAULT]);

	mat4 camMatrix = cam->getMatrix();

	glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "cameraMatrix"),
						1,
						false,
						&camMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "perspectiveMatrix"),
						1,
						false,
						&perspectiveMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "modelviewMatrix"),
						1,
						false,
						&modelview[0][0]);
	
	CheckGLErrors("loadUniforms");

	glDrawElements(
			GL_TRIANGLE_STRIP,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			numElements,		//How many indices
			GL_UNSIGNED_INT,	//Type
			(void*)0			//Offset
			);

	CheckGLErrors("render");
}




// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{   
    // initialize the GLFW windowing system
    if (!glfwInit()) {
        cout << "ERROR: GLFW failed to initilize, TERMINATING" << endl;
        return -1;
    }
    glfwSetErrorCallback(ErrorCallback);

    // attempt to create a window with an OpenGL 4.1 core profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(512, 512, "CPSC 453 OpenGL Boilerplate", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwMakeContextCurrent(window);

    // query and print out information about our OpenGL environment
    QueryGLVersion();

	initGL();
	
	
	cam = Camera(vec3(0, -50, 5), vec3(0, 6.f, -0.6f));
	camPos = cam.pos;
	camDir = cam.dir;
	camRight = cam.right; 
	camUp = cam.up;
	
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 20.f);

	vector<vec3> sunPoints;
	vector<vec3> sunNormals;
	vector<vec2> sunUvs;
	vector<unsigned int> sunIndices;
	
	vector<vec3> earthPoints;
	vector<vec3> earthNormals;
	vector<vec2> earthUvs;
	vector<unsigned int> earthIndices;
	
	vector<vec3> moonPoints;
	vector<vec3> moonNormals;
	vector<vec2> moonUvs;
	vector<unsigned int> moonIndices;
	
	vector<vec3> starPoints;
	vector<vec3> starNormals;
	vector<vec2> starUvs;
	vector<unsigned int> starIndices;
	
	glUseProgram(shader[SHADER::DEFAULT]);

	//create and load star texture
	GLuint starTex = createTexture("Stars.jpg");
		//create and load sun texture
	GLuint sunTex = createTexture("Sun.jpg");
		//create and load earth texture
	GLuint earthTex = createTexture("Earth.jpg");
		//create and load moon texture
	GLuint moonTex = createTexture("Moon.jpg");
	
	loadTexture(starTex, GL_TEXTURE3, shader[SHADER::DEFAULT],"Stars");
	loadTexture(sunTex, GL_TEXTURE0, shader[SHADER::DEFAULT],"Sun");
	loadTexture(earthTex, GL_TEXTURE1, shader[SHADER::DEFAULT],"Earth");
	loadTexture(moonTex, GL_TEXTURE2, shader[SHADER::DEFAULT],"Moon");
	
		generateSphere(starPoints,starNormals,starUvs,starIndices,10.f,10.f,20,20);
		generateSphere(sunPoints,sunNormals,sunUvs,sunIndices,1.f,1.f,20,20);
		generateSphere(earthPoints,earthNormals,earthUvs,earthIndices,0.48f,0.48f,20,20);
		generateSphere(moonPoints,moonNormals,moonUvs,moonIndices,0.229f,0.229f,20,20);
		
	float s = 0;
	float e = 0;
	float e2 = 0;
	float m = 0;
		
	
    // run an event-triggered main loop
    while (!glfwWindowShouldClose(window))
    {
		
		//Restart the animation
		if(restart)
		{
			s = 0;
			e = 0;
			e2 = 0;
			m = 0;
			restart = false;
		}
		
    	glClearColor(0.f, 0.f, 1.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)	
		
	GLint locs = glGetUniformLocation(shader[SHADER::DEFAULT], "theta");
		glUniform1f(locs, s);
	
	GLint loce = glGetUniformLocation(shader[SHADER::DEFAULT], "theta2");
	glUniform1f(loce, e);
	
	GLint loce2 = glGetUniformLocation(shader[SHADER::DEFAULT], "theta3");
	glUniform1f(loce2, e2);
	
	GLint locm = glGetUniformLocation(shader[SHADER::DEFAULT], "theta4");
	glUniform1f(locm, m);
	
	//generate star field
	loadBuffer(starPoints, starNormals, starUvs, starIndices);
	GLint loc = glGetUniformLocation(shader[SHADER::DEFAULT], "spaceSpheres");
		glUniform1i(loc, 3);
     render(&cam, perspectiveMatrix, mat4(1.f), 0, starIndices.size());


	//generate sun
	loadBuffer(sunPoints, sunNormals, sunUvs, sunIndices);
    loc = glGetUniformLocation(shader[SHADER::DEFAULT], "spaceSpheres");
		glUniform1i(loc, 0);
     render(&cam, perspectiveMatrix, mat4(1.f), 0, sunIndices.size());

	
	//generate earth
	loadBuffer(earthPoints, earthNormals, earthUvs, earthIndices);	
	glGetUniformLocation(shader[SHADER::DEFAULT], "spaceSpheres");
		glUniform1i(loc, 1);
     render(&cam, perspectiveMatrix, mat4(1.f), 0, earthIndices.size());
	
	
	//generate moon
	loadBuffer(moonPoints, moonNormals, moonUvs, moonIndices);
	glGetUniformLocation(shader[SHADER::DEFAULT], "spaceSpheres");
		glUniform1i(loc, 2);
     render(&cam, perspectiveMatrix, mat4(1.f), 0, moonIndices.size());
    
    //Unpaused animation
    if(!pause)
    {
		 //Sun's rotation will be the guideline for other rotations (0.01 = 25.4 days)
		 //Sun rotation = 100%
		 s = s + 0.01f*speed;
		 
		 //Earth rotates 7% as fast as the sun's rotation (25.4 / 365 = 7%)
		 //Earth rotation = 7% = 0.07 x 0.01 = 0.0007
		 e = e + 0.0007f*speed;
		 
		 //Earth rotates about its own axis 25.4% faster than the sun's rotation (25.4 / 1 = 25400%)
		 //Earth's axial rotation = 25400% = 25.4 x 0.01 = 0.254
		 e2 = e2 + 0.254f*speed;
		 
		 //Moon rotates around the earth slightly slower than the sun's rotation (25.4 days / 27.3 days = 93.0%)
		 m = m + 0.00930f*speed;
	}	
        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(window);

        // sleep until next event before drawing again
        glfwPollEvents();
	}

	// clean up allocated resources before exit
   deleteIDs();
	glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
    // query opengl version and renderer information
    string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    cout << "OpenGL [ " << version << " ] "
         << "with GLSL [ " << glslver << " ] "
         << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
        case GL_INVALID_ENUM:
            cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE:
            cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION:
            cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY:
            cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
        default:
            cout << "[unknown error code]" << endl;
        }
        error = true;
    }
    return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
    string source;

    ifstream input(filename.c_str());
    if (input) {
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
             << filename << endl;
    }

    return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);

    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }

    return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();

    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);

    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }

    return programObject;
}


// ==========================================================================
