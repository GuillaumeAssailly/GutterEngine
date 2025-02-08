#include "reflection_system.h"

ReflectionSystem::ReflectionSystem(unsigned int baseShader, unsigned int reflectionShader) : baseShader(baseShader), reflectionShader(reflectionShader)
{
}

void ReflectionSystem::Initialize(std::unordered_map<unsigned int, std::list<RenderComponent>>& renderComponents, int reflectionWidth, int reflectionHeight) {

	this->reflectionWidth = reflectionWidth;
	this->reflectionHeight = reflectionHeight;

	for (auto& entity : renderComponents) {
		for (auto& renderComponent : entity.second) {
			if (renderComponent.isPlanarReflectable) {

				glGenFramebuffers(1, &renderComponent.reflectionFBO);
				glBindFramebuffer(GL_FRAMEBUFFER, renderComponent.reflectionFBO);

				glGenTextures(1, &renderComponent.reflectionTexture);
				glBindTexture(GL_TEXTURE_2D, renderComponent.reflectionTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, reflectionWidth, reflectionHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderComponent.reflectionTexture, 0);

				glGenRenderbuffers(1, &renderComponent.reflectionDepthBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, renderComponent.reflectionDepthBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, reflectionWidth, reflectionHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderComponent.reflectionDepthBuffer);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}


	}
}



void ReflectionSystem::RenderReflection(std::unordered_map<unsigned int, TransformComponent>&transformComponents, std::unordered_map<unsigned int, std::list<RenderComponent>>&renderComponents, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, int screenWidth, int screenHeight, int cameraID)
{

    for (auto& entity : renderComponents) {
        for (auto& renderComponent : entity.second) {
            if (!renderComponent.isPlanarReflectable) continue;

            // Bind the reflection framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, renderComponent.reflectionFBO);
            glViewport(0, 0, reflectionWidth, reflectionHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //float planeHeight = 0.0f;
            //// Define the reflection plane (assuming a horizontal plane at Y = 0)
            //glm::vec4 plane = glm::vec4(0, 1, 0, planeHeight);

           


            //glm::mat4 reflectedViewMatrix = ComputeReflectionMatrix(planeHeight) * viewMatrix;

          

            float planeHeight = 0.0f;
            glm::vec4 clipPlane = glm::vec4(0, 1, 0, -planeHeight);

			glm::mat4 reflectionMatrix = {  1, 0, 0, 0,
											0, -1, 0, -2 * planeHeight * -planeHeight,
											0, 0, 1, 0,
											0, 0, 0, 1 };


			glm::mat4 flipMatrix = {    1, 0, 0, 0, 
			                            0, -1, 0, 0, 
			                            0, 0, 1, 0, 
			                            0, 0, 0, 1 };

			glm::mat4 reflectedCameraViewMatrix = reflectionMatrix *  glm::inverse(viewMatrix) * flipMatrix;
			glm::mat4 reflectedViewMatrix = glm::inverse(reflectedCameraViewMatrix);

            glm::mat4 viewInvTranspose = glm::transpose(glm::inverse(reflectedViewMatrix));
            glm::vec4 eyeClipPlane = viewInvTranspose * clipPlane;


            // Use the reflection shader
            glUseProgram(reflectionShader);

            // Pass the reflection matrix and projection matrix to the shader
            glUniformMatrix4fv(glGetUniformLocation(reflectionShader, "view"), 1, GL_FALSE, glm::value_ptr(reflectedViewMatrix));
            glUniformMatrix4fv(glGetUniformLocation(reflectionShader, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
            glUniform4fv(glGetUniformLocation(reflectionShader, "clipPlane"), 1, glm::value_ptr(eyeClipPlane));


            // Render each object to the reflection framebuffer
            for (auto& renderEntity : renderComponents) {
                unsigned int renderEntityID = renderEntity.first;
                TransformComponent& transform = transformComponents[renderEntityID];

                for (const RenderComponent& render : renderComponents[renderEntityID]) {
                    // Skip the reflective surface itself to avoid infinite recursion
                    if (render.isPlanarReflectable) continue;

                    // Calculate the model matrix for this object
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
                    model = model * glm::mat4_cast(transform.eulers);  // Apply object's rotation
                    // Pass the model matrix to the reflection shader
                    glUniformMatrix4fv(glGetUniformLocation(reflectionShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, render.material);
					glUniform1i(glGetUniformLocation(reflectionShader, "materialTexture"), 0);

                    if (render.emissiveMap != 0) {
                        glActiveTexture(GL_TEXTURE5);
                        glBindTexture(GL_TEXTURE_2D, render.emissiveMap);
                        glUniform1i(glGetUniformLocation(reflectionShader, "emissiveMap"), 5);
                        glUniform1i(glGetUniformLocation(reflectionShader, "hasEmissive"), 1);
                        glUniform1f(glGetUniformLocation(reflectionShader, "emissiveForce"), render.emissiveForce);
                    }
                    else {
                        glUniform1i(glGetUniformLocation(reflectionShader, "hasEmissive"), 0);
                    }

                    // Bind VAO and draw the entity
                    glBindVertexArray(render.mesh);
                    glDrawElements(GL_TRIANGLES, render.indexCount, GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                }
            }

            // Unbind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

     
    }

    glViewport(0, 0, screenWidth, screenHeight);
    //Give back control to the main shader :
    glUseProgram(baseShader);

}



