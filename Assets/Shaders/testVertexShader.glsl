#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec4 color;

void main()
{
    color = vec4(aColor, 1.0);
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}
