#version 120

in vec4 vertexColor;
varying out vec4 FragColor;

void main()
{
    FragColor = vertexColor;
}