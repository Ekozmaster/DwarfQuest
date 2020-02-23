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