#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in float ClipDist;

out vec4 FragColor;

uniform sampler2D materialTexture;


void main() {

   
    if (ClipDist < 0.0f) {
        discard;  // Discard fragments behind the reflection plane
    }
    // Sample the reflection texture using the adjusted coordinates
    FragColor = texture(materialTexture, TexCoord);
}