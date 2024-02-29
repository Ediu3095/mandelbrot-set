#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexCoords;

out vec2 fragmentCoords;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vertexPosition, 1.0f);
	fragmentCoords = vertexCoords;
}
