#include "light_system.h"

LightSystem::LightSystem(unsigned int shader) : shaderProgram(shader)
{
	
}

void LightSystem::update(std::unordered_map<unsigned int, LightComponent>&  lights, 
	std::unordered_map<unsigned int, TransformComponent>& transforms, unsigned int cameraID)
{
	int nbLights = lights.size();
	//Pass the current number of light sources to the shader : 
	glUniform1i(glGetUniformLocation(shaderProgram, "numLights"), nbLights);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(transforms.at(cameraID).position));

	//Update basic light color : 
	for (int i = 0; i < nbLights; i++) {
		// Set light position
		std::string lightPosUniform = "lightPos[" + std::to_string(i) + "]";
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, lightPosUniform.c_str());
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(transforms.at(i).position));

		// Set light color
		std::string lightColorUniform = "lightColor[" + std::to_string(i) + "]";
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, lightColorUniform.c_str());
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lights[i].color));

		//Set light intensity
		std::string lightIntensityUniform = "intensity[" + std::to_string(i) + "]";
		GLint lightIntensityLoc = glGetUniformLocation(shaderProgram, lightIntensityUniform.c_str());
		glUniform1f(lightIntensityLoc, lights[i].intensity);
	}
}


