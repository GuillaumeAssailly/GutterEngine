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

void ShadowSystem::Initialize(std::unordered_map<unsigned int, LightComponent>& lightComponents)
{



    for (auto& entity : lightComponents) {
        LightComponent& light = entity.second;


        glGenFramebuffers(1, &light.shadowFBO);
        glGenTextures(1, &light.depthMap);
        glBindTexture(GL_TEXTURE_2D, light.depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, light.shadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

}

void ShadowSystem::RenderDepthMap(unsigned int depthMap, int screenWidth, int screenHeight)
{
    // Set the viewport to the bottom-right corner
    int viewportWidth = screenWidth / 4;  // Adjust the size as needed
    int viewportHeight = screenHeight / 4; // Adjust the size as needed
    glViewport(0, 0, viewportWidth, viewportHeight);

    // Use the depth map shader
    glUseProgram(depthMapShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(depthMapShader, "depthMap"), 0);

    // Render the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Reset the viewport to the full screen
    glViewport(0, 0, screenWidth, screenHeight);
}


void ShadowSystem::GenerateShadowMap(std::unordered_map<unsigned int, LightComponent>& lightComponents,
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, RenderComponent>& renderComponents, int screenWidth, int screenHeight,
    CameraComponent& camera
)
{

    for (auto& entity : lightComponents) {
        glUseProgram(shadowShader);

        if (entity.second.isDirectional == false) continue; //Only directional lights cast shadows (for now 
        unsigned int entityID = entity.first;
        LightComponent& light = entity.second;

        glBindFramebuffer(GL_FRAMEBUFFER, light.shadowFBO);
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


        lightView = glm::lookAt(transformComponents[entityID].position, transformComponents[entityID].position + glm::normalize(light.direction), glm::vec3(0.0f, 1.0f, 0.0f));



        lightProjection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.01f, 8.0f);


        lightSpaceMatrix = lightProjection * lightView;

        //Pass the light space matrix to the shadow shader : 
        glUniformMatrix4fv(glGetUniformLocation(shadowShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));



        //Render each object to the shadow map : 
        for (auto& renderEntity : renderComponents) {
            unsigned int renderEntityID = renderEntity.first;

            //TODO : check potential existence of a component before rendering it
            TransformComponent& transform = transformComponents[renderEntityID];
            RenderComponent& render = renderComponents[renderEntityID];

            //Calculate the model matrix for this object: 
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            model = model * glm::mat4_cast(glm::quat(glm::radians(transform.eulers)));
            //Pass the model matrix to the shadow shader :
            glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

            //Bind VAO and draw the entity : 
            glBindVertexArray(render.mesh);
            glDrawElements(GL_TRIANGLES, render.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

        }


        //Unbinding framebuffer : 
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render the depth map to the screen
        RenderDepthMap(light.depthMap, screenWidth, screenHeight);

    }

    glViewport(0, 0, screenWidth, screenHeight);
    //Give back control to the main shader :
    glUseProgram(shader);




}
void ShadowSystem::CalculateShadowOrthoBounds(
    const glm::mat4& lightView,
    CameraComponent& camera,
    float& left, float& right, float& bottom, float& top,
    float& near_plane, float& far_plane
) {
    // Define frustum corners in clip space
    std::vector<glm::vec4> frustumCorners = {
        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // Near Bottom Left
        glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),   // Near Bottom Right
        glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),    // Near Top Right
        glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),   // Near Top Left
        glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),   // Far Bottom Left
        glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),    // Far Bottom Right
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),     // Far Top Right
        glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)     // Far Top Left
    };

    // Calculate the inverse of the camera's view-projection matrix
    glm::mat4 invCameraViewProjMatrix = glm::inverse(camera.viewMatrix);

    // Transform frustum corners to world space using the inverse view-projection matrix
    for (auto& corner : frustumCorners) {
        corner = invCameraViewProjMatrix * corner;
        corner /= corner.w; // Convert from homogeneous coordinates to 3D coordinates
    }

    // Transform frustum corners to light's view space
    for (auto& corner : frustumCorners) {
        corner = lightView * corner;
    }

    // Initialize bounds in light space
    left = right = frustumCorners[0].x;
    bottom = top = frustumCorners[0].y;
    near_plane = far_plane = frustumCorners[0].z;

    // Find the bounding box of the frustum corners in light's view space
    for (const auto& corner : frustumCorners) {
        left = std::min(left, corner.x);
        right = std::max(right, corner.x);
        bottom = std::min(bottom, corner.y);
        top = std::max(top, corner.y);
        near_plane = std::min(near_plane, corner.z);
        far_plane = std::max(far_plane, corner.z);
    }

    // Adjust near and far planes for shadow precision
    near_plane = std::max(near_plane, 0.01f); // Avoid too close to zero
    far_plane += 1.0f; // Small buffer to cover scene
}

std::vector<glm::vec4> ShadowSystem::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                    inv * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

