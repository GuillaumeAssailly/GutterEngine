#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in float ClipDist;

out vec4 FragColor;

uniform sampler2D materialTexture;
uniform sampler2D emissiveMap;
uniform int hasEmissive;
uniform float emissiveForce;

#define MAX_LIGHTS 50
uniform int numLights;         // Number of lights
uniform vec3 lightPos[MAX_LIGHTS];     // Array of light positions (max 10 lights)
uniform vec3 lightColor[MAX_LIGHTS];   // Array of light colors
uniform int lightType[MAX_LIGHTS]; // Array of boolean values indicating if light is directional or spot
uniform vec3 lightsDir[MAX_LIGHTS];      // Array of light directions

uniform float constant;          // Constant attenuation factor
uniform float linear;            // Linear attenuation factor
uniform float quadratic;         // Quadratic attenuation factor
uniform float intensity[MAX_LIGHTS]; // Intensity for each light
uniform float spotLightOuterCutoff[MAX_LIGHTS]; // Array of spot light outer cutoff angles
uniform float spotLightCutoff[MAX_LIGHTS]; // Array of spot light cutoff angles

uniform float ambientStrength;

void main() {
    if (ClipDist < 0.0f) {
        discard;  // Discard fragments behind the reflection plane
    }

//     // Initialize lighting components
    vec3 ambient = vec3(0.0);
//    vec3 diffuse = vec3(0.0);
//    vec3 specular = vec3(0.0);
//
//     Loop over all lights
//    for (int i = 0; i < numLights; i++) 
//    {
//        vec3 lightDir;
//        float attenuation = 1.0;
//        float shadow = 0.0f;
//        If directionnal :
//        if(lightType[i] == 1)
//        {
//            lightDir = normalize(-lightsDir[i]);
//        }
//        else if(lightType[i] == 2)
//        {
//            lightDir = normalize(lightPos[i] - FragPos);
//            float distance = length(lightPos[i] - FragPos);
//            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
//
//              Spotlight intensity calculation
//            float theta = dot(lightDir, normalize(-lightsDir[i]));  Angle between light direction and fragment
//            float epsilon = spotLightCutoff[i] - spotLightOuterCutoff[i];  Soft transition range
//            float intensityFactor = clamp((theta - spotLightOuterCutoff[i]) / epsilon, 0.0, 1.0);  Smooth transition
//           
//        } else {
//            lightDir = normalize(lightPos[i] - FragPos);
//            float distance = length(lightPos[i] - FragPos);
//            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
//        }
//       
//         Ambient lighting
//        ambient += ambientStrength * lightColor[i] * intensity[i] * attenuation;
//
//    }
    // Sample the emissive map
    vec3 emissive = vec3(0.0);
    if (hasEmissive == 1) {
        emissive = texture(emissiveMap, TexCoord).rgb * emissiveForce * 0.2f;
    }

    // Sample the material texture
    vec4 baseColor = texture(materialTexture, TexCoord);

    // Combine the base color with the emissive color
    FragColor = (baseColor * 0.2f + vec4(emissive, 0.0));
}