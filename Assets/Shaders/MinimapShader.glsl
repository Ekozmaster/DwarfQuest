#VERTEX
#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;

out vec2 uv;

void main()
{
    uv = aUV;
    vec3 newPos = aPos.xzy;
    gl_Position = modelMatrix * vec4(newPos.x, -newPos.y, newPos.z, 1.0);
}



#FRAGMENT
#version 330

//  Standard noise snippets from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

//	Classic Perlin 2D Noise 
//	by Stefan Gustavson
//
vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
float rand(vec2 c){return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);}

float noise(vec2 P){
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
    vec4 i = permute(permute(ix) + iy);
    vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;
    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);
    vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}

uniform ivec2 screenResolution;
uniform vec2 mapWorldPosition;

in vec2 uv;

out vec4 FragColor;

void main()
{
    // Finding fragment's world position.
    float screenWidth = screenResolution.x;
    float aspectRatio = screenWidth / screenResolution.y;
    float blocksOnScreen = 10000.0f; // How many blocks to render in both directions, up/down (zoom out factor).
    vec2 fragWorldPos = mapWorldPosition + (uv * 2 - 1) * vec2(aspectRatio, 1) * blocksOnScreen;

    // Drawing a circle in the middle.
    if (distance(fragWorldPos, mapWorldPosition) < 30) {
        FragColor = vec4(0.7, 0.0, 0.0, 1.0);
        return;
    }

    // Finding the voronoi cell.
    float voronoiScale = 1.0f/6000;
    ivec2 gridPos = ivec2(floor(fragWorldPos * voronoiScale));
    vec2 closestNeighbour = vec2(gridPos - 1);
    float minDistance = 100000.0f;

    for (int y = gridPos.y - 1; y <= gridPos.y + 1; y++) {
        for (int x = gridPos.x - 1; x <= gridPos.x + 1; x++) {
            ivec2 currentGrid = ivec2(x, y);
            vec2 gridSeedWorldPos = (vec2(currentGrid) + vec2(rand(currentGrid), rand(currentGrid.yx)) * 0.5f) / voronoiScale;

            float dist = distance(fragWorldPos, gridSeedWorldPos);
            if (dist < minDistance) {
                minDistance = dist;
                closestNeighbour = vec2(x, y);
            }
        }
    }

    float color = rand(closestNeighbour);
    FragColor = vec4(color, color, color, 1.0);
}