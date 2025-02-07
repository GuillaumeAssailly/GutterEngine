#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <ImGuiFileDialog.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <iomanip>

#include <PxPhysicsAPI.h>

#include <vector>
#include <unordered_map>

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

struct Material {
    unsigned int baseColor = -1;
    unsigned int normalMap = -1;
    unsigned int emissiveMap = -1;
};