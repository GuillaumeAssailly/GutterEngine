#version 460 core

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;    // Tangent vector
layout(location = 4) in vec3 aBitangent;  // Bitangent vector


out vec3 ourColor;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec4 FragPosLightSpace;
out vec3 Tangent;            
out vec3 Bitangent;        
out vec4 ClipSpacePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isLine;
//uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
   	FragPos =  vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
	// Transform the tangent and bitangent vectors to world space
    Tangent = mat3(transpose(inverse(model))) * aTangent;
    Bitangent = mat3(transpose(inverse(model))) * aBitangent;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	//FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    ClipSpacePos = projection * view * model * vec4(aPos, 1.0);
}