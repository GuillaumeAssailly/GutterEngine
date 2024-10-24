#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

// texture samplers
uniform sampler2D texture1;    // First texture sampler
uniform sampler2D texture2;    // Second texture sampler

// Phong material properties
uniform vec3 ambientStrength;  // Ambient reflectivity
uniform vec3 diffuseStrength;  // Diffuse reflectivity
uniform vec3 specularStrength; // Specular reflectivity
uniform float shininess;       // Shininess factor

// Light properties
#define MAX_LIGHTS 50
uniform int numLights;         // Number of lights
uniform vec3 lightPos[MAX_LIGHTS];     // Array of light positions (max 10 lights)
uniform vec3 lightColor[MAX_LIGHTS];   // Array of light colors
uniform vec3 viewPos;          // Position of the viewer (camera)

// Attenuation factors and intensity
uniform float constant;          // Constant attenuation factor
uniform float linear;            // Linear attenuation factor
uniform float quadratic;         // Quadratic attenuation factor
uniform float intensity[MAX_LIGHTS]; // Intensity for each light

void main()
{
    // Combine textures using mix
    vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    // Normalize the input normal
    vec3 norm = normalize(Normal);

    // Initialize lighting components
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Loop over all lights
    for (int i = 0; i < numLights; i++) {
        
         // Compute lighting direction for this light
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float distance = length(lightPos[i] - FragPos);

        // Compute attenuation
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

        // Ambient lighting
        ambient += ambientStrength * lightColor[i] * intensity[i] * attenuation;

        // Diffuse lighting
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += diffuseStrength * diff * lightColor[i] * intensity[i] * attenuation;

        // Specular lighting
        float spec = 0;
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        if(dot(norm, lightDir) >=0.0)
        {
            spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }        
        specular += specularStrength * spec * lightColor[i] * intensity[i] * attenuation;
       

       
        
    }

    // Combine the lighting components
    vec3 phong = ambient + diffuse + specular;

    // Apply the final color with Phong lighting and texture
    FragColor = texColor * vec4(phong, 1.0);
}
