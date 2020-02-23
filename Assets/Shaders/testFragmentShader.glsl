#version 330

out vec4 FragColor;
in vec3 normal;

void main()
{
    vec3 lightDir = vec3(-0.3, -1, -0.5);

    float lightFactor = dot(normalize(normal), normalize(-lightDir));
    if (lightFactor < 0) lightFactor = lightFactor * 0.2 + 0.2;
    else lightFactor = lightFactor * 0.8 + 0.2;

    FragColor = vec4(1,1,1,1) * lightFactor;
}