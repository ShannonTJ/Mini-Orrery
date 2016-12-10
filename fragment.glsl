// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Modifications by: Shannon TJ 10101385
// Date:    Fall 2016
// ==========================================================================
#version 410

// interpolated colour received from vertex stage

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

in vec3 FragNormal;
in vec2 FragUV;

uniform sampler2D Sun;
uniform sampler2D Moon;
uniform sampler2D Earth;
uniform sampler2D Stars;

uniform int spaceSpheres = 0;
vec3 lightVec = vec3(0,0,0);

void main(void)
{
	
	if(spaceSpheres == 0)
		FragmentColour = texture(Sun, 1-FragUV);
		
	else if(spaceSpheres == 1)
		FragmentColour = texture(Earth, 1-FragUV);
		
	else if(spaceSpheres == 2)
		FragmentColour = texture(Moon, 1-FragUV);
		
	else if(spaceSpheres == 3)
		FragmentColour = texture(Stars, 1-FragUV);
		
	//FragmentColour = vec4(normalize(FragNormal), 1);
}
