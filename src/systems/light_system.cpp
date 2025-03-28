#include "light_system.h"

LightSystem::LightSystem(unsigned int shader, unsigned int reflectionShader) : shaderProgram(shader), reflectionShaderProgram(reflectionShader)
{
}


void LightSystem::update(std::unordered_map<unsigned int, LightComponent>&  lights, 
	std::unordered_map<unsigned int, TransformComponent>& transforms, unsigned int cameraID)
{
	int nbLights = lights.size();
	//Pass the current number of light sources to the shader : 
	glUniform1i(glGetUniformLocation(shaderProgram, "numLights"), nbLights);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(transforms.at(cameraID).position));
	glUniform1i(glGetUniformLocation(reflectionShaderProgram, "numLights"), nbLights);
	glUniform3fv(glGetUniformLocation(reflectionShaderProgram, "viewPos"), 1, glm::value_ptr(transforms.at(cameraID).position));
	//Update basic light color : 
	for (int i = 0; i < nbLights; i++) {
		// Set light position
		std::string lightPosUniform = "lightPos[" + std::to_string(i) + "]";
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, lightPosUniform.c_str());
		GLint reflectionLightPosLoc = glGetUniformLocation(reflectionShaderProgram, lightPosUniform.c_str());
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(transforms.at(i).position));
		glUniform3fv(reflectionLightPosLoc, 1, glm::value_ptr(transforms.at(i).position));

		// Set light color
		std::string lightColorUniform = "lightColor[" + std::to_string(i) + "]";
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, lightColorUniform.c_str());
		GLint reflectionLightColorLoc = glGetUniformLocation(reflectionShaderProgram, lightColorUniform.c_str());
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lights[i].color));
		glUniform3fv(reflectionLightColorLoc, 1, glm::value_ptr(lights[i].color));

		//Set light intensity
		std::string lightIntensityUniform = "intensity[" + std::to_string(i) + "]";
		GLint lightIntensityLoc = glGetUniformLocation(shaderProgram, lightIntensityUniform.c_str());
		GLint reflectionLightIntensityLoc = glGetUniformLocation(reflectionShaderProgram, lightIntensityUniform.c_str());
		glUniform1f(lightIntensityLoc, lights[i].intensity);
		glUniform1f(reflectionLightIntensityLoc, lights[i].intensity);

		//Set isDirectional
		std::string lightIsDirectionalUniform = "lightType[" + std::to_string(i) + "]";
		GLint lightIsDirectionalLoc = glGetUniformLocation(shaderProgram, lightIsDirectionalUniform.c_str());
		GLint reflectionLightIsDirectionalLoc = glGetUniformLocation(reflectionShaderProgram, lightIsDirectionalUniform.c_str());
		glUniform1i(lightIsDirectionalLoc, lights[i].type);
		glUniform1i(reflectionLightIsDirectionalLoc, lights[i].type);

		//Set Direction
		std::string lightDirectionUniform = "lightsDir[" + std::to_string(i) + "]";
		GLint lightDirectionLoc = glGetUniformLocation(shaderProgram, lightDirectionUniform.c_str());
		GLint reflectionLightDirectionLoc = glGetUniformLocation(reflectionShaderProgram, lightDirectionUniform.c_str());
		glUniform3fv(lightDirectionLoc, 1, glm::value_ptr(lights[i].direction));
		glUniform3fv(reflectionLightDirectionLoc, 1, glm::value_ptr(lights[i].direction));

		//Set Cutoff
		std::string lightCutoffUniform = "spotLightCutoff[" + std::to_string(i) + "]";
		GLint lightCutoffLoc = glGetUniformLocation(shaderProgram, lightCutoffUniform.c_str());
		GLint reflectionLightCutoffLoc = glGetUniformLocation(reflectionShaderProgram, lightCutoffUniform.c_str());
		glUniform1f(lightCutoffLoc, lights[i].cutoff);
		glUniform1f(reflectionLightCutoffLoc, lights[i].cutoff);

		//Set OuterCutoff
		std::string lightOuterCutoffUniform = "spotLightOuterCutoff[" + std::to_string(i) + "]";
		GLint lightOuterCutoffLoc = glGetUniformLocation(shaderProgram, lightOuterCutoffUniform.c_str());
		GLint reflectionLightOuterCutoffLoc = glGetUniformLocation(reflectionShaderProgram, lightOuterCutoffUniform.c_str());
		glUniform1f(lightOuterCutoffLoc, lights[i].outerCutoff);
		glUniform1f(reflectionLightOuterCutoffLoc, lights[i].outerCutoff);

		//Set lightSpaceMatrix
		std::string lightSpaceMatrixUniform = "lightSpaceMatrix[" + std::to_string(i) + "]";
		GLint lightSpaceMatrixLoc = glGetUniformLocation(shaderProgram, lightSpaceMatrixUniform.c_str());
		glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lights[i].lightSpaceMatrix));

		// Pass the shadow map layer index to the shader
		std::string shadowLayerUniform = "shadowMapLayers[" + std::to_string(i) + "]";
		GLint shadowLayerLoc = glGetUniformLocation(shaderProgram, shadowLayerUniform.c_str());
		glUniform1i(shadowLayerLoc, lights[i].shadowMapLayer);
		
	}
}


