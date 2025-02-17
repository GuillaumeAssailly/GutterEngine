#include "shadow_system.h"

float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

ShadowSystem::ShadowSystem(unsigned int baseShader, unsigned int sShader, unsigned int depthMapDebug) : shader(baseShader), shadowShader(sShader), depthMapShader(depthMapDebug)
{

}

ShadowSystem::~ShadowSystem()
{
}

void ShadowSystem::Initialize(std::unordered_map<unsigned int, LightComponent>& lightComponents)
{

	glGenTextures(1, &shadowMapArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 10, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glGenFramebuffers(1, &shadowFBO);

    int layer = 0;
    for (auto& entity : lightComponents) {
        LightComponent& light = entity.second;
		light.shadowMapLayer = layer++;


        //glGenFramebuffers(1, &light.shadowFBO);
        //glGenTextures(1, &light.depthMap);
        //glBindTexture(GL_TEXTURE_2D, light.depthMap);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        //float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        //glBindFramebuffer(GL_FRAMEBUFFER, light.shadowFBO);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.depthMap, 0);
        //glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //Initialize the debug shader:  

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    GLint maxArrayLayers;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);
    std::cout << "Max texture array layers: " << maxArrayLayers << std::endl;

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "Max texture size (width/height): " << maxTextureSize << std::endl;

}
void ShadowSystem::RenderDepthMap(unsigned int depthMapArray, int screenWidth, int screenHeight)
{
    // Debug window configuration
    const int cols = 2;                     // 2 columns
    const int rows = 5;                     // 5 rows (2x5 = 10 layers)
    const int numLayers = cols * rows;
    const int padding = 10;                 // Padding from screen edges

    // Calculate dimensions based on screen size
    const int debugWidth = screenWidth / 8;         // Width of debug panel
    const int tileSize = debugWidth / cols;         // Size of each layer tile
    const int debugHeight = tileSize * rows;        // Total debug panel height

    // Position calculation
    const int startX = padding;
    const int startY = screenHeight - debugHeight - padding;

    // Set up shader and texture
    glUseProgram(depthMapShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMapArray);
    glUniform1i(glGetUniformLocation(depthMapShader, "depthMap"), 0);

    // Render each layer tile
    for (int layer = 0; layer < numLayers; ++layer) {
        const int col = layer % cols;
        const int row = layer / cols;

        const int x = startX + col * tileSize;
        const int y = startY + (rows - 1 - row) * tileSize; // Flip Y-axis

        glViewport(x, y, tileSize, tileSize);
        glUniform1i(glGetUniformLocation(depthMapShader, "layer"), layer);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // Reset viewport
    glViewport(0, 0, screenWidth, screenHeight);
}

void ShadowSystem::GenerateShadowMap(std::unordered_map<unsigned int, LightComponent>& lightComponents,
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents, int screenWidth, int screenHeight,
   int cameraID
)
{

    glUseProgram(shadowShader);
    for (auto& entity : lightComponents) {

        if (entity.second.type == POINT) continue; //Only directional lights and spotlights cast shadows (for now ;-)
        unsigned int entityID = entity.first;
        LightComponent& light = entity.second;
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArray, 0, light.shadowMapLayer);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        //Set up light's view and proj matrix : 
        glm::mat4 lightProjection, lightView;
        glm::mat4& lightSpaceMatrix = light.lightSpaceMatrix;

        //Default values for light projection and view matrices :


        //lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, near_plane, far_plane);
        /*float near_plane, far_plane, left, right, bottom, top;
        lightView = glm::lookAt(transformComponents[entityID].position, transformComponents[entityID].position + glm::normalize(light.direction), glm::vec3(0.0f, 1.0f, 0.0f));
        CalculateShadowOrthoBounds(lightView, camera, left, right, bottom, top, near_plane, far_plane);*/



        if (light.type == DIRECTIONAL) {
            lightView = glm::lookAt(transformComponents[cameraID].position, transformComponents[cameraID].position + glm::normalize(light.direction), glm::vec3(0.0f, 1.0f, 0.0f));
            lightProjection = glm::ortho(-6.0f, 6.0f, -6.0f, 6.0f, -5.0f, 8.0f);
        }
        else if (light.type == SPOT) {
            // Spotlight Shadow Mapping (Perspective Projection)

            float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
            float nearPlane = 0.1f;
            float farPlane = 10.0f;

            // Define a default spotlight field of view (FoV) if you don't have one
            float spotlightFoV = glm::radians(45.0f); // 45-degree spotlight cone

            // Use a perspective projection for the spotlight
            lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);

            // Spotlight's view matrix (from light position looking in light direction)
            lightView = glm::lookAt(
                transformComponents[entityID].position,  // Light position
                transformComponents[entityID].position + glm::normalize(light.direction), // Look at the light's target
                glm::vec3(0.0f, 0.0f, -1.0f) // Up vector
            );

            
        }
        lightSpaceMatrix = lightProjection * lightView;

        //Pass the light space matrix to the shadow shader : 
        glUniformMatrix4fv(glGetUniformLocation(shadowShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));



        //Render each object to the shadow map : 
        for (auto& renderEntity : renderComponents) {
            unsigned int renderEntityID = renderEntity.first;

            //TODO : check potential existence of a component before rendering it
            TransformComponent& transform = transformComponents[renderEntityID];

            for (const RenderComponent& render : renderComponents[renderEntityID])
            {
                //Calculate the model matrix for this object: 
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform.position);
                model = model * glm::mat4_cast(transform.eulers);
                //Pass the model matrix to the shadow shader :
                glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

                //Bind VAO and draw the entity : 
                glBindVertexArray(render.mesh);
                glDrawElements(GL_TRIANGLES, render.indexCount, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }

            

        }


       

    }

    //Unbinding framebuffer : 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render the depth map to the screen
    RenderDepthMap(shadowMapArray, screenWidth, screenHeight);

    glViewport(0, 0, screenWidth, screenHeight);
    //Give back control to the main shader :
    glUseProgram(shader);




}
