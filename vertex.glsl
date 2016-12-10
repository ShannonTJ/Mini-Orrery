// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Modifications by: Shannon TJ 10101385
// Date:    Fall 2016
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 UV;

out vec3 FragNormal;
out vec2 FragUV;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelviewMatrix = mat4(1.f);

float PI = 3.14159265359;
uniform float theta = 0;
uniform float theta2 = 0;
uniform float theta3 = 0;
uniform float theta4 = 0;


mat4 sunMatrix = mat4(cos(-PI*theta), -sin(-PI*theta), 0.f, 0.f,
							  sin(-PI*theta), cos(-PI*theta), 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f ,0.f ,0.f ,1.f);
							  
mat4 earthMatrix = mat4(cos((-PI*theta2)), -sin((-PI*theta2)), 0.f, 0.f,
							  sin((-PI*theta2)), cos((-PI*theta2)), 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f ,0.f ,0.f ,1.f);
							

mat4 axisMatrix = mat4(cos(-PI*theta3), -sin(-PI*theta3), 0.f, 0.f,
							  sin(-PI*theta3), cos(-PI*theta3), 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f ,0.f ,0.f ,1.f);
							  
mat4 moonMatrix = mat4(cos(-PI*theta4), -sin(-PI*theta4), 0.f, 0.f,
							  sin(-PI*theta4), cos(-PI*theta4), 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f ,0.f ,0.f ,1.f);
							  
//SOURCE: https://en.wikipedia.org/wiki/Axial_tilt	
//y axis rotation of 23.4 degrees				  
mat4 axialEarthTilt = mat4(cos(0.4101524f), 0.f, sin(0.4101524f), 0.f,
							  0.f, 1.0f, 0.f, 0.f,
							  -sin(0.4101524f), 0.f, cos(0.4101524f), 0.f,
							  0.f ,0.f ,0.f ,1.f);		

//SOURCE: https://en.wikipedia.org/wiki/Axial_tilt								  
//y axis rotation of 1.5424 degrees				  
mat4 axialMoonTilt = mat4(cos(0.02687807f), 0.f, sin(0.02687807f), 0.f,
							  0.f, 1.0f, 0.f, 0.f,
							  -sin(0.02687807f), 0.f, cos(0.02687807f), 0.f,
							  0.f ,0.f ,0.f ,1.f);	

//SOURCE: https://en.wikipedia.org/wiki/Axial_tilt								  
//y axis rotation of 7.25 degrees				  
mat4 axialSunTilt = mat4(cos(0.1265364f), 0.f, sin(0.1265364f), 0.f,
							  0.f, 1.0f, 0.f, 0.f,
							  -sin(0.1265364f), 0.f, cos(0.1265364f), 0.f,
							  0.f ,0.f ,0.f ,1.f);						  

//Lines up the moon's texture so that the proper side is facing the Earth
mat4 moonTilt = mat4(cos(1.0472f), -sin(1.0472f), 0.f, 0.f,
							  sin(1.0472f), cos(1.0472f), 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f ,0.f ,0.f ,1.f);


float x = VertexNormal.x;
float y = VertexNormal.y;
float z = VertexNormal.z;

vec4 moonTrans = vec4(-0.8f,0,0,0);
vec4 earthTrans = vec4(-2.795f,0,0,0);

uniform sampler2D Sun;
uniform sampler2D Moon;
uniform sampler2D Earth;
uniform sampler2D Stars;


uniform int spaceSpheres = 0;

// output to be interpolated between vertices and passed to the fragment stage

void main()
{
	FragNormal = normalize((modelviewMatrix*vec4(VertexNormal, 0.f)).xyz);
	
	FragUV = UV;
	
	if(spaceSpheres == 0)
		gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*sunMatrix*vec4(VertexPosition, 1.0);
	
	//Earth
	//Distance = log 6 (149.6 million km) = 2.795
	else if(spaceSpheres == 1)
		gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*earthMatrix*
		(axisMatrix*axialEarthTilt*vec4(VertexPosition, 1.0)+earthTrans);
		
	//Moon
	//Distance = log 6 (0.384400 million km) = 2.795 + 0.533 (had to increase the distance, the moon was inside the Earth)
	else if(spaceSpheres == 2)
		gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*earthMatrix*
					  (moonMatrix*(moonTilt*axialMoonTilt*vec4(VertexPosition, 1.0)+moonTrans)+earthTrans);
		
	//Sun, Starfield
	else
		gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*axialSunTilt*vec4(VertexPosition, 1.0);
}
