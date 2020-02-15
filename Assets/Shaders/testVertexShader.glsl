#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
    //gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
    gl_Position = vec4(aPos, 1.0);
}
