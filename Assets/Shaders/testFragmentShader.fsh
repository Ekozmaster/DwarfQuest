#version 120

out vec4 FragColor;
in vec4 vertexColor;

void main()
{
    FragColor = vertexColor;
}