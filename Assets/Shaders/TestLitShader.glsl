#VERTEX
#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 normal;
out vec2 uv;

void main()
{
    uv = aUV;
    normal = vec3(modelMatrix * vec4(aNormal, 1));
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}



#FRAGMENT
#version 330

in vec3 normal;
in vec2 uv;

uniform sampler2D uTexture0;

out vec4 FragColor;

void main()
{
    vec3 lightDir = vec3(-0.3, -1, -0.5);

    float lightFactor = dot(normalize(normal), normalize(-lightDir));
    if (lightFactor < 0) lightFactor = lightFactor * 0.2 + 0.2;
    else lightFactor = lightFactor * 0.8 + 0.2;

    FragColor = texture(uTexture0, uv) * lightFactor;
}