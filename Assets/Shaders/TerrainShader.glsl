#VERTEX
#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

out vec3 normal;
out vec2 uv;
out vec3 color;

void main()
{
    uv = aUV;
    color = aColor;
    normal = vec3(modelMatrix * vec4(aNormal, 0));
    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}



#FRAGMENT
#version 330

in vec3 normal;
in vec2 uv;
in vec3 color;

uniform sampler2D uTexture0;
uniform vec4 ambientLight;
uniform float ambientLightIntensity;

out vec4 FragColor;

void main()
{
    vec3 lightDir = vec3(-0.3, -1, -0.5);
    float dirLightIntensity = 1.0f;

    float lightFactor = dot(normalize(normal), normalize(-lightDir));
    if (lightFactor < 0) lightFactor = lightFactor * 0.2 + 0.2;
    else lightFactor = lightFactor * 0.8 + 0.2;

    vec4 textureColor = texture(uTexture0, uv);

    // Applying the inverse of reinhard's tone mapping operator to fake a HDR texture.
    textureColor = -textureColor / (textureColor - 1);

    vec4 finalColor = (textureColor * lightFactor * dirLightIntensity + ambientLight * ambientLightIntensity) * (vec4(color, 1.0) * 0.75 + 0.25);

    // Applying reinhard's tone mapping operator to map colors back to 0-1 range.
    FragColor = finalColor / (finalColor + 1.0);
}