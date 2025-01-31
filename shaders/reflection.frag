#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in float ClipDist;

out vec4 FragColor;

uniform sampler2D materialTexture;


void main() {

    // For reflections, you may need to reflect the coordinates
    // Adjust the reflection coordinates based on some logic, e.g., using the view matrix
   
    if (ClipDist < 0.0f) {
        discard;  // Discard fragments behind the reflection plane
    }
    // Sample the reflection texture using the adjusted coordinates
    FragColor = texture(materialTexture, TexCoord);
}