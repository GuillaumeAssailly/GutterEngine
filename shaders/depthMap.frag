#version 460 core
out vec4 FragColor;


uniform sampler2DArray depthMap;
uniform int layer;
in vec2 TexCoords;

void main()
{
    float depth = texture(depthMap, vec3(TexCoords, layer)).r;
    FragColor = vec4(vec3(depth), 1.0);
}