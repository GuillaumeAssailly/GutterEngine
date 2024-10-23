#include "shader.h"

unsigned int make_shader(
	const std::string& vertex_filepath, const std::string& fragment_filepath) {

	//To store all the shader modules
	std::vector<unsigned int> modules;

	//Add a vertex shader module
	modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));

	//Add a fragment shader module
	modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

	//Attach all the modules then link the program
	unsigned int shader = glCreateProgram();
	for (unsigned int shaderModule : modules) {
		glAttachShader(shader, shaderModule);
	}
	glLinkProgram(shader);

	set_shader_uniform(shader, "ambientStrength", glm::vec3(0.07f, 0.07f, 0.07f));
	set_shader_uniform(shader, "diffuseStrength", glm::vec3(0.9f, 0.9f, 0.9f));
	set_shader_uniform(shader, "specularStrength", glm::vec3(2.0f, 2.0f, 2.0f));
	set_shader_uniform(shader, "shininess", 32.0f);

	//Check the linking worked
	int success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		char errorLog[1024];
		glGetProgramInfoLog(shader, 1024, NULL, errorLog);
		std::cout << "Shader linking error:\n" << errorLog << '\n';
	}

	//Modules are now unneeded and can be freed
	for (unsigned int shaderModule : modules) {
		glDeleteShader(shaderModule);
	}

	return shader;

}

unsigned int make_module(const std::string& filepath, unsigned int module_type) {

	std::ifstream file;
	std::stringstream bufferedLines;
	std::string line;

	file.open(filepath);

	while (std::getline(file, line)) {
		//std::cout << line << std::endl;
		bufferedLines << line << '\n';
	}
	std::string shaderSource = bufferedLines.str();
	const char* shaderSrc = shaderSource.c_str();
	bufferedLines.str("");
	file.close();

	unsigned int shaderModule = glCreateShader(module_type);
	glShaderSource(shaderModule, 1, &shaderSrc, NULL);
	glCompileShader(shaderModule);


	int success;
	glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
	if (!success) {
		char errorLog[1024];
		glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
		std::cout << "Shader Module compilation error:\n" << errorLog << std::endl;
	}

	return shaderModule;
}

void set_shader_uniform(unsigned int shader, const std::string& name, float value) {
	int location = glGetUniformLocation(shader, name.c_str());
	if (location == -1) {
		std::cout << "Warning: Uniform '" << name << "' doesn't exist in the shader program!" << std::endl;
		return;
	}
	//Set the uniform value (for a float)
	glUseProgram(shader);
	glUniform1f(location, value);
}

 void set_shader_uniform(unsigned int shader, const std::string& name, const glm::vec3& vec) {
	// Locate the uniform in the shader program
	int location = glGetUniformLocation(shader, name.c_str());
	if (location == -1) {
		std::cout << "Warning: Uniform '" << name << "' doesn't exist in the shader program!" << std::endl;
		return;
	}

	// Set the uniform value (for a vec3)
	glUseProgram(shader);
	glUniform3fv(location, 1, glm::value_ptr(vec));  // Send vector data to shader
}

