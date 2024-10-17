#pragma once
#include "../config.h"

unsigned int make_module(const std::string& filepath, unsigned int module_type);

unsigned int make_shader(
    const std::string& vertex_filepath, const std::string& fragment_filepath);


void set_shader_uniform(unsigned int shader, const std::string& name, const glm::vec3& vec);
void set_shader_uniform(unsigned int shader, const std::string& name, float);