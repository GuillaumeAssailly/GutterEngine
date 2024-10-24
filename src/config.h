#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp> 

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

#include <vector>
#include <unordered_map>

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btVector3.h>

#include <fstream>
#include <sstream>
#include <string>

const int MESH = 0;
const int BOX = 1;
const int SPHERE = 2;