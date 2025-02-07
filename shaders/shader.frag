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
uniform sampler2DArray shadowMap;   // Shadow map sampler
uniform sampler2D normalMap;   // Normal map sampler
uniform sampler2D reflectionTexture; //Plannar reflection texture sampler
uniform sampler2D emissiveMap; // Emissive map sampler (for emissive lighting)
uniform sampler2D aoMap; // Ambient occlusion map sampler
uniform sampler2D metalRoughnessMap; // Metalness-roughness map sampler

// Phong material properties
uniform vec3 ambientStrength;  // Ambient reflectivity
uniform vec3 diffuseStrength;  // Diffuse reflectivity
uniform vec3 specularStrength; // Specular reflectivity
uniform float shininess;       // Shininess factor
uniform bool isLine;           // Drawing a line or not
uniform int hasNormalMap;
uniform bool isPlanarReflectable;
uniform int hasEmissive;
uniform float emissiveForce;
uniform int hasMetalRoughnessMap;

uniform int hasAOMap;
uniform vec3 viewPos;          // Position of the viewer (camera)

// Light properties
#define MAX_LIGHTS 50
uniform int numLights;         // Number of lights
uniform int shadowMapLayers[MAX_LIGHTS];
uniform vec3 lightPos[MAX_LIGHTS];     // Array of light positions (max 10 lights)
uniform vec3 lightColor[MAX_LIGHTS];   // Array of light colors
uniform int lightType[MAX_LIGHTS]; // Array of boolean values indicating if light is directional or spot
uniform vec3 lightsDir[MAX_LIGHTS];      // Array of light directions
uniform mat4 lightSpaceMatrix[MAX_LIGHTS]; // Array of light space matrices

// Attenuation factors and intensity
uniform float constant;          // Constant attenuation factor
uniform float linear;            // Linear attenuation factor
uniform float quadratic;         // Quadratic attenuation factor
uniform float intensity[MAX_LIGHTS]; // Intensity for each light
uniform float spotLightOuterCutoff[MAX_LIGHTS]; // Array of spot light outer cutoff angles
uniform float spotLightCutoff[MAX_LIGHTS]; // Array of spot light cutoff angles
float shadowCalculation(int lightIndex, vec4 fragPosLightSpace) {
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
    int samples = 16; // Increase for a smoother shadow, but higher performance cost

    // Sample points in a 3x3 grid around the current fragment's shadow coords
    int layer = shadowMapLayers[lightIndex];
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            // Calculate the offset for the PCF sample
            vec2 offset = vec2(x, y) * pcfRadius;

            // Sample the depth from the shadow map
            float closestDepth = texture(shadowMap, vec3(shadowCoords.xy + offset, layer)).r;

            // Accumulate shadow contribution
            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
    }

    // Average the shadow contributions
    shadow /= float(samples);

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
    if (hasNormalMap == 1) {
        // Retrieve the normal from the normal map
        vec3 normalMapValue = texture(normalMap, TexCoord).rgb;
        normalMapValue = normalMapValue * 2.0 - 1.0; // Transform from [0,1] to [-1,1]

        // Construct the TBN matrix
        vec3 T = normalize(Tangent);
        vec3 B = normalize(Bitangent);
        vec3 N = normalize(Normal);
        mat3 TBN = mat3(T, B, N);

        // Transform the normal from tangent space to world space
        norm = normalize(TBN * normalMapValue);
    } else {
        norm = normalize(Normal);
    }

    // Initialize lighting components
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Metalness and roughness
    float metalness = 0.0;
    float roughness = 0.5;
    if (hasMetalRoughnessMap == 1) {
        vec3 metalRough = texture(metalRoughnessMap, TexCoord).rgb;
        if (any(isnan(metalRough)) || any(isinf(metalRough))) {
            metalness = 0.0;
            roughness = 0.5;
        } else {
            metalness = pow(clamp(metalRough.b, 0.0, 1.0), 0.5);
            roughness = clamp(metalRough.g * 2.0, 0.0, 1.0);
        }
    }

    // Base reflectivity
    float baseReflectivity = mix(0.6, 1.0, metalness); // 0.4 for non-metals, 1.0 for metals

    // Adjust roughness for non-metals
    float adjustedRoughness = roughness;
    if (metalness < 0.5) {
        adjustedRoughness = roughness * 0.5; // Reduce roughness for non-metals
    }

    // Loop over all lights
    for (int i = 0; i < numLights; i++) {
        vec3 lightDir;
        float attenuation = 1.0;
        float shadow = 0.0;

        if (lightType[i] == 1) { // Directional light
            lightDir = normalize(-lightsDir[i]);
            vec4 fragPosLightSpace = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
            shadow = shadowCalculation(i, fragPosLightSpace);
        } else if (lightType[i] == 2) { // Spotlight
            lightDir = normalize(lightPos[i] - FragPos);
            float distance = length(lightPos[i] - FragPos);
            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

            float theta = dot(lightDir, normalize(-lightsDir[i]));
            float epsilon = spotLightCutoff[i] - spotLightOuterCutoff[i];
            float intensityFactor = clamp((theta - spotLightOuterCutoff[i]) / epsilon, 0.0, 1.0);
            vec4 fragPosLightSpace = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
            shadow = shadowCalculation(i, fragPosLightSpace);
            attenuation *= intensityFactor;
        } else { // Point light
            lightDir = normalize(lightPos[i] - FragPos);
            float distance = length(lightPos[i] - FragPos);
            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        }

        // Ambient lighting
        ambient += ambientStrength * lightColor[i] * intensity[i] * attenuation;

        // Diffuse lighting (reduced for metals)
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += (1.0 - shadow) * diffuseStrength * diff * lightColor[i] * intensity[i] * attenuation * (1.0 - metalness);

        // Specular lighting
        float spec = 0.0;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        if (dot(norm, lightDir) >= 0.0) {
            spec = pow(max(dot(viewDir, reflectDir), 0.0), mix(2.0, shininess, 1.0 - adjustedRoughness));
        }

        // Fresnel effect (Schlick approximation)
        float fresnelFactor = pow(1.0 - max(dot(viewDir, norm), 0.0), 5.0);
        fresnelFactor = mix(fresnelFactor, 0.1, metalness); // Boost Fresnel for metals

        // Apply specular and Fresnel terms
        specular += (1.0 - shadow) * specularStrength * spec * baseReflectivity * lightColor[i] * intensity[i] * attenuation;
        specular += fresnelFactor * baseReflectivity * lightColor[i] * intensity[i] * attenuation; // Boost Fresnel contribution
    }

    // Ambient occlusion
    float ao = 1.0;
    if (hasAOMap == 1) {
        ao = texture(aoMap, TexCoord).r;
    }
    ambient *= ao;
    diffuse *= ao;
    specular *= ao;

    // Combine lighting components
    vec3 phong = ambient + diffuse + specular;

    // Emissive lighting
    vec3 emissive = vec3(0.0);
    if (hasEmissive == 1) {
        emissive = texture(emissiveMap, TexCoord).rgb * emissiveForce;
    }

    // Planar reflection
    if (isPlanarReflectable) {
        vec2 reflectionUV = ClipSpacePos.xy / ClipSpacePos.w * 0.5 + 0.5;
        reflectionUV.y = 1.0 - reflectionUV.y;
        vec3 reflectionColor = blurReflection(reflectionTexture, reflectionUV);
        FragColor = mix(texColor, vec4(reflectionColor, 1.0), 0.5) * vec4(phong + emissive, 1.0);
    } else {
        FragColor = texColor * vec4(phong + emissive, 1.0);
    }
}
