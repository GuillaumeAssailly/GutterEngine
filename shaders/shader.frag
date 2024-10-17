#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

// texture samplers
uniform vec3 lightPos;         // Position of the light
uniform vec3 viewPos;          // Position of the viewer (camera)
uniform sampler2D texture1;    // First texture sampler
uniform sampler2D texture2;    // Second texture sampler

// Phong material properties
uniform vec3 lightColor;       // Color of the light
uniform vec3 ambientStrength;  // Ambient reflectivity
uniform vec3 diffuseStrength;  // Diffuse reflectivity
uniform vec3 specularStrength; // Specular reflectivity
uniform float shininess;       // Shininess factor

void main()
{
    // Combine textures using mix
    vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    // Normalize the input normal and compute lighting direction
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Compute ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Compute diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Compute specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = 0;
    if(dot(norm, lightDir) >=0.0)
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }
   
     vec3 specular = specularStrength * spec * lightColor;
     // Combine the lighting components
    vec3 phong = ambient + diffuse + specular;

    
    // Apply the final color with Phong lighting and texture
    FragColor = texColor * vec4(phong, 1.0);
   
}
