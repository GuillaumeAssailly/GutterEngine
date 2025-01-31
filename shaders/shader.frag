#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in vec4 FragPosLightSpace;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ClipSpacePos;

// texture samplers
uniform sampler2D texture1;    // First texture sampler
uniform sampler2D texture2;    // Second texture sampler
uniform sampler2D shadowMap;   // Shadow map sampler
uniform sampler2D normalMap;   // Normal map sampler
uniform sampler2D reflectionTexture; //Plannar reflection texture sampler

// Phong material properties
uniform vec3 ambientStrength;  // Ambient reflectivity
uniform vec3 diffuseStrength;  // Diffuse reflectivity
uniform vec3 specularStrength; // Specular reflectivity
uniform float shininess;       // Shininess factor
uniform bool isLine;           // Drawing a line or not
uniform int hasNormalMap;
uniform bool isPlanarReflectable;

// Light properties
#define MAX_LIGHTS 50
uniform int numLights;         // Number of lights
uniform vec3 lightPos[MAX_LIGHTS];     // Array of light positions (max 10 lights)
uniform vec3 lightColor[MAX_LIGHTS];   // Array of light colors
uniform int lightType[MAX_LIGHTS]; // Array of boolean values indicating if light is directional
uniform vec3 lightsDir[MAX_LIGHTS];      // Array of light directions
uniform float spotLightOuterCutoff[MAX_LIGHTS]; // Array of spot light outer cutoff angles
uniform float spotLightCutoff[MAX_LIGHTS]; // Array of spot light cutoff angles
uniform vec3 viewPos;          // Position of the viewer (camera)

// Attenuation factors and intensity
uniform float constant;          // Constant attenuation factor
uniform float linear;            // Linear attenuation factor
uniform float quadratic;         // Quadratic attenuation factor
uniform float intensity[MAX_LIGHTS]; // Intensity for each light

float shadowCalculation(vec4 fragPosLightSpace) {
    // Convert to NDC and then to [0, 1] space
    vec3 shadowCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadowCoords = shadowCoords * 0.5 + 0.5;

    if (shadowCoords.z > 1.0 || shadowCoords.z < 0.0 || shadowCoords.x < 0.0 || shadowCoords.x > 1.0 || shadowCoords.y < 0.0 || shadowCoords.y > 1.0) {
        return 0.0; // Fully lit if outside the shadow map bounds (we currently consider nothing is shadowed outside of shadowmap range).
    }

    // Get current fragment depth in light space
    float currentDepth = shadowCoords.z;

    // Set the shadow bias
    float bias = 0.005;

    // Define the size of the PCF kernel
    float pcfRadius = 1.0 / 1024.0; // Adjust this based on shadow map resolution

    // Initialize shadow accumulator
    float shadow = 0.0;
    int samples = 4; // Increase for a smoother shadow, but higher performance cost

    // Sample points in a 3x3 grid around the current fragment's shadow coords
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            // Calculate the offset for the PCF sample
            vec2 offset = vec2(x, y) * pcfRadius;

            // Sample the depth from the shadow map
            float closestDepth = texture(shadowMap, shadowCoords.xy + offset).r;

            // Check if the fragment is in shadow
            shadow += currentDepth > closestDepth + bias ? 1.0 : 0.0;
        }
    }

    // Average the shadow factor over all samples to get a softer shadow
    shadow /= (samples * 2);

    // Return the final shadow factor
    return shadow;
}

// Function to perform blur
vec3 blurReflection(sampler2D textureSampler, vec2 uv) {
    float blurStrength = 8.0;
    float blurFactor = 3.0;
    // Offset determines the distance between samples based on blurFactor
    vec2 offset = vec2(1.0) / vec2(textureSize(textureSampler, 0)) * blurFactor;
    
    // Initialize the color accumulator
    vec3 color = vec3(0.0);
    
    // Sample surrounding pixels (horizontal, vertical)
    color += texture(textureSampler, uv + vec2(-offset.x, 0.0)).rgb * blurStrength; // Left
    color += texture(textureSampler, uv + vec2(offset.x, 0.0)).rgb * blurStrength;  // Right
    color += texture(textureSampler, uv + vec2(0.0, -offset.y)).rgb * blurStrength; // Bottom
    color += texture(textureSampler, uv + vec2(0.0, offset.y)).rgb * blurStrength;  // Top
    
    // Optionally, you can add more samples for higher blur quality (diagonal)
    color += texture(textureSampler, uv + vec2(-offset.x, -offset.y)).rgb * blurStrength; // Bottom-left
    color += texture(textureSampler, uv + vec2(offset.x, -offset.y)).rgb * blurStrength; // Bottom-right
    color += texture(textureSampler, uv + vec2(-offset.x, offset.y)).rgb * blurStrength; // Top-left
    color += texture(textureSampler, uv + vec2(offset.x, offset.y)).rgb * blurStrength; // Top-right
    
    // Return the averaged color
    return color / (8.0 * blurStrength); // Normalize by total number of samples
}


void main()
{
    // Combine textures using mix
    vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    vec3 norm;
    if(hasNormalMap == 1){
        // Retrieve the normal from the normal map
        vec3 normalMapValue = texture(normalMap, TexCoord).rgb;
        normalMapValue = normalMapValue * 2.0 - 1.0f; // Transform from [0,1] to [-1,1]

        // Construct the TBN matrix
        vec3 T = normalize(Tangent);
        vec3 B = normalize(Bitangent);
        vec3 N = normalize(Normal);
        mat3 TBN = mat3(T, B, N);

        // Transform the normal from tangent space to world space
        norm = normalize(TBN * normalMapValue);
    }
    else {
        norm = normalize(Normal);
    }
    

        // Initialize lighting components
        vec3 ambient = vec3(0.0);
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);

        // Loop over all lights
        for (int i = 0; i < numLights; i++) {
        
        vec3 lightDir;
        float attenuation = 1.0;
        float shadow = 0.0f;
        //If directionnal :
        if(lightType[i] == 1)
        {
            lightDir = normalize(-lightsDir[i]);
            
            //Calculate the shadow factor
            shadow = shadowCalculation(FragPosLightSpace);
        }
        else if(lightType[i] == 2)
        {
            lightDir = normalize(lightPos[i] - FragPos);
            float distance = length(lightPos[i] - FragPos);
            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

             // Spotlight intensity calculation
            float theta = dot(lightDir, normalize(-lightsDir[i])); // Angle between light direction and fragment
            float epsilon = spotLightCutoff[i] - spotLightOuterCutoff[i]; // Soft transition range
            float intensityFactor = clamp((theta - spotLightOuterCutoff[i]) / epsilon, 0.0, 1.0); // Smooth transition

            attenuation *= intensityFactor; // Apply spotlight effect

        } else {
            lightDir = normalize(lightPos[i] - FragPos);
            float distance = length(lightPos[i] - FragPos);
            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        }


        
       
        // Ambient lighting
        ambient += ambientStrength * lightColor[i] * intensity[i] * attenuation;

        // Diffuse lighting
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += (1.0 - shadow) * diffuseStrength * diff * lightColor[i] * intensity[i] * attenuation;

            // Specular lighting
            float spec = 0;
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        if(dot(norm, lightDir) >=0.0)
        {
            spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }        
        specular += (1.0 - shadow) * specularStrength * spec * lightColor[i] * intensity[i] * attenuation;
       

       
        
        }



    // Combine the lighting components
    vec3 phong = ambient + diffuse + specular;

    if(isPlanarReflectable){

        // Convert clip space position to texture coordinates
        vec2 reflectionUV = ClipSpacePos.xy / ClipSpacePos.w * 0.5 + 0.5;
        reflectionUV.y = 1.0 - reflectionUV.y; // Flip Y to match OpenGL convention

        vec3 reflectionColor = blurReflection(reflectionTexture, reflectionUV);
        
        // Blend reflection with the original texture
        FragColor = mix(texColor, vec4(reflectionColor, 1.0), 0.3) * vec4(phong, 1.0); 
    }
    else {
     // Apply the final color with Phong lighting and texture
        FragColor = texColor * vec4(phong, 1.0);
    }


   

    //Debug normal : 
    //FragColor = vec4(norm * 0.5 + 0.5, 1.0);

}
