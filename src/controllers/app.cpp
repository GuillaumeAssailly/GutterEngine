#include "app.h"
#include <queue>


int screenWidth = 1920;
int screenHeight = 1080;

App::App() {
    set_up_glfw();
}

App::~App() {
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
    glDeleteProgram(shader);
    glDeleteProgram(shadowShader);
	glDeleteProgram(depthMapDebugShader);
	glDeleteProgram(reflectionShader);



    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;
    delete lineSystem;

    glfwTerminate();
}

unsigned int App::make_entity(const std::string& name) {
	entityNames.insert(std::make_pair(entity_count, name));
    return entity_count++;
}






static int totalFaces = 0;

void processNode(const aiScene* scene, aiNode* node, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    
  
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        // Extract vertices
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            vertices.push_back(mesh->mVertices[j].x);
            vertices.push_back(mesh->mVertices[j].y);
            vertices.push_back(mesh->mVertices[j].z);

            vertices.push_back(mesh->mNormals[j].x);
            vertices.push_back(mesh->mNormals[j].y);
            vertices.push_back(mesh->mNormals[j].z);

            vertices.push_back(mesh->mTextureCoords[0][j].x);
            vertices.push_back(mesh->mTextureCoords[0][j].y);

			vertices.push_back(mesh->mTangents[j].x);
			vertices.push_back(mesh->mTangents[j].y);
			vertices.push_back(mesh->mTangents[j].z);

		    vertices.push_back(mesh->mBitangents[j].x);
		    vertices.push_back(mesh->mBitangents[j].y);
		    vertices.push_back(mesh->mBitangents[j].z);


        

        }

        // Extract indices
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            const aiFace& face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }

        std::cout << "Mesh name : " << node->mName.C_Str() << std::endl;
		std::cout << "Faces number : " << mesh->mNumFaces << std::endl;
        std::cout << "Vertices: " << vertices.size() << std::endl;

        totalFaces += mesh->mNumFaces;


    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i], vertices, indices);
    }
    std::cout << "Total faces number : " << totalFaces << std::endl;
	
}


std::pair<unsigned int, unsigned int> App::make_model(const char* filePath) {

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( filePath, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        exit(-1);
    }

    processNode(scene, scene->mRootNode, vertices, indices);

    // Create VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    // Create VBO for vertices
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Create EBO for indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0); // 8 * sizeof(float) stride (3 for pos, 3 for normal, 2 for tex)


    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float[3]))); // Normal starts after 3 floats for position
    

    // Normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float[6]))); // Tex coords start after 6 floats (3 for pos, 3 for normal)

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float))); // Tangent starts after 8 floats (3 for pos, 3 for normal, 2 for tex)

    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float))); // Bitangent starts after 11 floats (3 for pos, 3 for normal, 2 for tex, 3 for tangent)
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Return a tuple of the VAO and the number of indices
    return std::make_pair(VAO, indices.size());
}

std::pair<unsigned int, unsigned int> App::make_cube_mesh(glm::vec3 size) {

    // Cube vertex data: each vertex has a position, normal, and texture coordinate
    // The cube will be centered at (0, 0, 0) and scaled by the given size
    std::vector<float> vertices = {
        // Positions            // Normals           // TexCoords
        -0.5f * size.x, -0.5f * size.y, -0.5f * size.z,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  // Back face
         0.5f * size.x, -0.5f * size.y, -0.5f * size.z,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f * size.x,  0.5f * size.y, -0.5f * size.z,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f * size.x,  0.5f * size.y, -0.5f * size.z,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

        -0.5f * size.x, -0.5f * size.y,  0.5f * size.z,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  // Front face
         0.5f * size.x, -0.5f * size.y,  0.5f * size.z,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f * size.x,  0.5f * size.y,  0.5f * size.z,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f * size.x,  0.5f * size.y,  0.5f * size.z,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

        -0.5f * size.x,  0.5f * size.y,  0.5f * size.z, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  // Left face
        -0.5f * size.x,  0.5f * size.y, -0.5f * size.z, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f * size.x, -0.5f * size.y, -0.5f * size.z, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f * size.x, -0.5f * size.y,  0.5f * size.z, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

         0.5f * size.x,  0.5f * size.y,  0.5f * size.z,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  // Right face
         0.5f * size.x,  0.5f * size.y, -0.5f * size.z,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f * size.x, -0.5f * size.y, -0.5f * size.z,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f * size.x, -0.5f * size.y,  0.5f * size.z,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

        -0.5f * size.x, -0.5f * size.y, -0.5f * size.z,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  // Bottom face
         0.5f * size.x, -0.5f * size.y, -0.5f * size.z,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f * size.x, -0.5f * size.y,  0.5f * size.z,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        -0.5f * size.x, -0.5f * size.y,  0.5f * size.z,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f * size.x,  0.5f * size.y, -0.5f * size.z,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  // Top face
         0.5f * size.x,  0.5f * size.y, -0.5f * size.z,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f * size.x,  0.5f * size.y,  0.5f * size.z,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        -0.5f * size.x,  0.5f * size.y,  0.5f * size.z,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // Cube indices (corrected to counterclockwise winding order for back-face culling)
    std::vector<unsigned int> indices = {
        // Back face (now clockwise)
        3, 1, 0,
        3, 2, 1,

        // Front face
        5, 7, 4,
        6, 7, 5,

        // Left face
        9, 11, 8,
        10, 11, 9,

        // Right face
        15, 13, 12,
        15, 14, 13,

        // Bottom face
        16, 17, 19,
        17, 18, 19,

        // Top face
        20,23,21,
        21, 23, 22
    };

    // Create VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    // Create VBO for vertices
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Create EBO for indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Return a pair of the VAO and the number of indices
    return std::make_pair(VAO, indices.size());
}

unsigned int App::make_texture(const char* filename, const bool flipTex) {

    int width, height, channels;
    stbi_set_flip_vertically_on_load(flipTex);
    unsigned char* data = stbi_load(
        filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;  // Return 0 if texture loading fails
    }

    //make the texture
    unsigned int texture;
    glGenTextures(1, &texture);
    textures.push_back(texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //load data
    glTexImage2D(GL_TEXTURE_2D,
        0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
    stbi_image_free(data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    

    return texture;
}

// TODO: Place this function in a specific ImGui file
bool App::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (
        epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
    Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
    if (dot(Row[0], Pdum3) < 0)
    {
        for (length_t i = 0; i < 3; i++)
        {
            scale[i] *= static_cast<T>(-1);
            Row[i] *= static_cast<T>(-1);
        }
    }
#endif

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }


    return true;
}


//ImGui variables
unsigned int selectedEntityID = 0;
int gizmo_type = -1;
bool gizmo_world = true;
physx::PxVec3 force = { 0.f, 0.f, 0.f };
float targetMass;
glm::vec2 targetSolverIteration;
glm::vec2 targetDamping;
float targetSleepT = 0.f;
physx::PxVec3 targetCMass;

// Imgui Create Entity Variables
    //Global
char newEntityName[128] = "NewObject";
static bool addTransform = false;
static bool addPhysics = false;
static bool addRender = false;
static bool addLight = false;

    // Transform
static glm::vec3 newPosition = { 0,0,0 };
static glm::quat newEulers = { 0,0,0,0 };

    // Physics
static int isStatic = 1;
static int geometry = 0;
static physx::PxVec3 boxSize = { 0, 0, 0 };
static float sphereRadius = 0.0f;
static std::string selectedModelName = "";
static float newEntityStaticFriction = 0.5f;
static float newEntityDynamicFriction = 0.5f;
static float newEntityRestitution = 0.5f;
static float newEntityMass = 1.f;
static int newEntitySolverPosition = 4;
static int newEntitySolverVelocity = 4;
static float newEntityLinearDamping = 0.5f;
static float newEntityAngularDamping = 0.5f;
static float newEntitySleepT = 0.1f;

static std::string selectedRModelName = "";
static std::string selectedTexturesName = "";

    // Light
static glm::vec3 newEntityColor = { 0,0,0 };
static float newEntityIntensity = 0.f;

static std::string error_msg = "";

//Lines related variables
short type_reference_frame = 2;
bool grid_display = true;

///<summary>
/// run methods launching the renderer pipeline :
///</summary>
void App::run() {
    // Variables to track FPS display
    float fpsTimeCounter = 0.0f;
    int frameCount = 0;

    //while loop iterating on the renderer pipeline : 
    shadowSystem->Initialize(lightComponents);
	reflectionSystem->Initialize(renderComponents,1024,1024);
    //We will initialize after every other system : 
    renderSystem->Initialize(shadowSystem->getShadowMapArray());
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        // -------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Count the number of frames and accumulate time
        frameCount++;
        fpsTimeCounter += deltaTime;

        if (hasPhysics)
            accumulatedTime += deltaTime;

        // Calculate and display FPS in window title every second
        if (fpsTimeCounter >= 1.0f) {
            float fps = frameCount / fpsTimeCounter;
            std::string title = "FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, title.c_str());

            // Reset counters for the next FPS calculation
            frameCount = 0;
            fpsTimeCounter = 0.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && hasPhysics) {
            physx::PxVec3 force(0, 0, 0.5);
            motionSystem->applyForceToActor(physicsComponents[1].rigidBody, force);
        }

        // Update systems
        if (accumulatedTime >= 0.00833) {
            motionSystem->update(transformComponents, physicsComponents, accumulatedTime);
            accumulatedTime = 0.;
        }
        bool should_close = cameraSystem->update(transformComponents, cameraComponents, cameraID, deltaTime);
        if (should_close) {
            break;
        }
        lightSystem->update(lightComponents, transformComponents, cameraID);
		reflectionSystem->RenderReflection(transformComponents, renderComponents, cameraSystem->GetViewMatrix(), cameraSystem->GetProjectionMatrix(), screenWidth, screenHeight, cameraID);
        renderSystem->update(transformComponents, renderComponents, lightComponents);
        shadowSystem->GenerateShadowMap(lightComponents, transformComponents, renderComponents, screenWidth, screenHeight, cameraID);

        //Draw Lines
        //Add here more lines to draw...
        lineSystem->render_lines_ref_frame_grid(type_reference_frame, grid_display, transformComponents[cameraID].position, shader);


        // Start //ImGui window for debugging
        ImGui::Begin("Debug");

        // Display FPS
        ImGui::Text("FPS: %f", 1.0f / deltaTime);

        ImGui::End();

        // --- Entity Tree Window ---
        ImGui::Begin("Entity Tree");

        // Loop through all entities to create a tree view
        for (int entityID = 0; entityID < entity_count; entityID++) {
            std::string entityLabel = entityNames.at(entityID);

            // Display each entity as selectable
            if (ImGui::Selectable(entityLabel.c_str(), selectedEntityID == entityID)) {
                selectedEntityID = entityID; // Set the selected entity when clicked

            }
        }

        ImGui::End(); // End of Entity Tree window

        // If an entity is selected, show its components
        if (selectedEntityID < entity_count && selectedEntityID != -1) {
            // --- Inspector Window ---
            ImGui::Begin("Inspector");


            // Display TransformComponent if present
            if (transformComponents.find(selectedEntityID) != transformComponents.end()) {
                ImGui::Text("Transform Component");
                TransformComponent& transform = transformComponents[selectedEntityID];
                ImGui::DragFloat3("Position", &transform.position[0]);
                ImGui::DragFloat3("Rotation", &transform.eulers[0]);
            }

            // Display PhysicsComponent if present
            if (physicsComponents.find(selectedEntityID) != physicsComponents.end()) {
                ImGui::Text("Physics Component");
                PhysicsComponent& physics = physicsComponents[selectedEntityID];
                targetMass = physics.rigidBody->getMass();
                physx::PxU32 minPositionIters, minVelocityIters;
                physics.rigidBody->getSolverIterationCounts(minPositionIters, minVelocityIters);
                targetSolverIteration = { minPositionIters, minVelocityIters };
                targetDamping = { physics.rigidBody->getLinearDamping(), physics.rigidBody->getAngularDamping() };
                targetSleepT = physics.rigidBody->getSleepThreshold();
                targetCMass = physics.rigidBody->getCMassLocalPose().p;

                if (ImGui::DragFloat("Mass", &targetMass))
                    physx::PxRigidBodyExt::setMassAndUpdateInertia(*physics.rigidBody, targetMass);
                if (ImGui::DragFloat3("CMass", &targetCMass.x))
                    physics.rigidBody->setCMassLocalPose(physx::PxTransform(targetCMass));
                if (ImGui::DragFloat2("Damping", &targetDamping.x)) {
                    physics.rigidBody->setLinearDamping(targetDamping.x);
                    physics.rigidBody->setAngularDamping(targetDamping.y);
                }
                if (ImGui::DragFloat("Sleep Threshold", &targetSleepT))
                    physics.rigidBody->setSleepThreshold(targetSleepT);
                if (ImGui::DragFloat2("Solver Iters", &targetSolverIteration.x)) {
                    physics.rigidBody->setSolverIterationCounts(targetSolverIteration.x, targetSolverIteration.x);
                }

                ImGui::DragFloat3("Force", &force.x);
                if (hasPhysics && ImGui::Button("Apply Force")) {
                    auto it = physicsComponents.find(selectedEntityID);
                    if (it != physicsComponents.end()) {
                        PhysicsComponent& physicsComponent = it->second;
                        physicsComponent.rigidBody->addForce(force, physx::PxForceMode::eIMPULSE);
                    }
                }
            }

            // Display LightComponent if present
            if (lightComponents.find(selectedEntityID) != lightComponents.end()) {
                ImGui::Text("Light Component");
                LightComponent& light = lightComponents[selectedEntityID];
                ImGui::ColorEdit3("Light Color", &light.color[0]);
                ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
                const char* lightTypeNames[] = { "Point", "Directional", "Spot" };
				ImGui::Combo("Light Type", (int*)&light.type, lightTypeNames, IM_ARRAYSIZE(lightTypeNames));
                if (light.type == DIRECTIONAL || light.type == SPOT) {
                    ImGui::DragFloat3("Light Direction", &light.direction[0], 0.1);
                }

            }

            // Display RenderComponent if present
            if (renderComponents.find(selectedEntityID) != renderComponents.end()) {
                ImGui::Text("Render Component");
                //RenderComponent& render = renderComponents[selectedEntityID];
               // ImGui::InputInt("Mesh ID", (int*)&render.mesh);
            }
            if (ImGui::Button("Close")) {
                selectedEntityID = -1;
            }
            ImGui::End();    // End of Inspector window
        }

        ImGui::Begin("Object Creator");

        ImGui::InputText("Object Name", newEntityName, IM_ARRAYSIZE(newEntityName));

        ImGui::Text("Transform Component");
        ImGui::DragFloat3("Position", &newPosition.x);
        ImGui::DragFloat4("Rotation", &newEulers.x);
        ImGui::Checkbox("Physics Component", &addPhysics);
        if (addPhysics) {
            ImGui::Text("Select a physics mode");
            ImGui::RadioButton("Static", &isStatic, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Dynamic", &isStatic, 0);

            ImGui::Text("Select a geometry");
            ImGui::RadioButton("Custom", &geometry, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Box", &geometry, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Sphere", &geometry, 2);

            switch (geometry) {
            case 0:
                if (ImGui::BeginCombo("Select Physical Mesh", selectedModelName.c_str())) {
                    for (const auto& pair : physicsModels) {
                        const std::string& modelName = pair.first;
                        const auto& meshes = pair.second;
                        bool isSelected = (modelName == selectedModelName);
                        if (ImGui::Selectable(modelName.c_str(), isSelected)) {
                            selectedModelName = modelName;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                break;
            case 1:
                ImGui::DragFloat3("Size", &boxSize.x);
                break;
            case 2:
                ImGui::DragFloat("Radius", &sphereRadius);
            }

            ImGui::Text("Settings");
            ImGui::DragFloat("Static Friction", &newEntityStaticFriction);
            ImGui::DragFloat("Dynamic Friction", &newEntityDynamicFriction);
            ImGui::DragFloat("Restitution", &newEntityRestitution);

            ImGui::DragFloat("Mass", &newEntityMass);
            ImGui::DragFloat("Sleep Threshold", &newEntitySleepT);

            ImGui::DragFloat("Linear Damping", &newEntityLinearDamping);
            ImGui::DragFloat("Angular Damping", &newEntityAngularDamping);

            ImGui::DragInt("Position Solver", &newEntitySolverPosition);
            ImGui::DragInt("Velocity Solver", &newEntitySolverVelocity);
        }

        ImGui::Checkbox("Render Component", &addRender);
        if (addRender) {
            if (ImGui::BeginCombo("Select Rendering Mesh", selectedRModelName.c_str())) {
                for (const auto& pair : renderModels) {
                    const std::string& modelName = pair.first;
                    const auto& meshes = pair.second;
                    bool isSelected = (modelName == selectedRModelName);
                    if (ImGui::Selectable(modelName.c_str(), isSelected)) {
                        selectedRModelName = modelName;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::BeginCombo("Select Texture", selectedTexturesName.c_str())) {
                for (const auto& pair : texturesList) {
                    const std::string& modelName = pair.first;
                    const auto& meshes = pair.second;
                    bool isSelected = (modelName == selectedTexturesName);
                    if (ImGui::Selectable(modelName.c_str(), isSelected)) {
                        selectedTexturesName = modelName;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        ImGui::Checkbox("Light Component", &addLight);
        if (addLight) {
            ImGui::ColorEdit3("Light Color", &newEntityColor.x);
            ImGui::SliderFloat("Intensity", &newEntityIntensity, 0.0f, 10.0f);
        }

        if (ImGui::Button("Create Object")) {
            if (!((selectedModelName == "" && addPhysics && geometry == 0) || ((selectedRModelName == "" || selectedTexturesName == "") && addRender))) {
                unsigned int id = make_entity(newEntityName);
                TransformComponent transform;
                transform.position = newPosition;
                transform.eulers = newEulers;
                transformComponents[id] = transform;

                if (addPhysics) {
                    PhysicsComponent physics;
                    glm::vec3 material = { newEntityStaticFriction, newEntityDynamicFriction, newEntityRestitution };
                    const physx::PxBoxGeometry boxGeometry(physx::PxVec3(boxSize.x / 2.f, boxSize.y / 2.f, boxSize.z / 2.f));
                    const physx::PxSphereGeometry sphereGeometry(sphereRadius);


                    switch (geometry) {
                    case 0:
                        if (!isStatic)
                            physics.rigidBody = motionSystem->createDynamic(physicsModels[selectedModelName], material, newPosition, newEntityMass, newEntitySleepT, newEntityLinearDamping, newEntityAngularDamping);
                        else
                            motionSystem->createStatic(physicsModels[selectedModelName], material, newPosition);
                        break;
                    case 1:
                        if (!isStatic)
                            physics.rigidBody = motionSystem->createDynamic(boxGeometry, material, newPosition, newEntityMass, newEntitySleepT, newEntityLinearDamping, newEntityAngularDamping);
                        else
                            motionSystem->createStatic(boxGeometry, material, newPosition);
                        break;
                    case 2:
                        if (!isStatic)
                            physics.rigidBody = motionSystem->createDynamic(sphereGeometry, material, newPosition, newEntityMass, newEntitySleepT, newEntityLinearDamping, newEntityAngularDamping);
                        else
                            motionSystem->createStatic(sphereGeometry, material, newPosition);
                        break;
                    }
                    if (!isStatic)
                        physicsComponents[id] = physics;
                }
                if (addRender) {
                    RenderComponent render;
                    render.mesh = renderModels[selectedRModelName].first;
                    render.indexCount = renderModels[selectedRModelName].second;
                    render.material = texturesList[selectedTexturesName];
                    //renderComponents[id] = render;
                }
                if (addLight) {
                    LightComponent light;
                    light.color = newEntityColor;
                    light.intensity = newEntityIntensity;
                    lightComponents[id] = light;
                }
                error_msg = "";
            }
            else {
                error_msg = "Please fill all the fields";
            }



            /*strcpy_s(newEntityName, sizeof(newEntityName), "NewObject");
            addTransform = false;
            addPhysics = false;
            addRender = false;
            addLight = false;

            // Transform
            glm::vec3 newPosition = { 0,0,0 };
            glm::quat newEulers = { 0,0,0,0 };

            // Physics
            isStatic = 1;
            geometry = 0;
            boxSize = { 0, 0, 0 };
            sphereRadius = 0.0f;
            selectedModelIndex = 0;
            newEntityStaticFriction = 0.5f;
            newEntityDynamicFriction = 0.5f;
            newEntityRestitution = 0.5f;
            newEntityMass = 1.f;
            newEntitySolverPosition = 4;
            newEntitySolverVelocity = 4;
            newEntityLinearDamping = 0.5f;
            newEntityAngularDamping = 0.5f;
            newEntitySleepT = 0.1f;

            selectedRModelIndex = 0;
            selectedTexturesIndex = 0;

            // Light
            newEntityColor = { 0,0,0 };
            newEntityIntensity = 0.f;

            // Réinitialiser les champs
            addTransform = false;
            addPhysics = false;
            addRender = false;*/



        }
        ImGui::Text(error_msg.c_str());

        ImGui::End();

        // --- Physics Window
        ImGui::Begin("Physics Window");

        if (hasPhysics) {
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.43f, 0.7f, 0.75f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.43f, 0.9f, 0.9f));
            if (ImGui::Button(" Physics Activated ", ImVec2(-1.0f, 0.0f)))
                hasPhysics = false;
            ImGui::PopStyleColor(2);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.75f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.9f, 0.9f));
            if (ImGui::Button(" Physics Desactivated ", ImVec2(-1.0f, 0.0f)))
                hasPhysics = true;
            ImGui::PopStyleColor(2);
        }

        ImGui::End(); // End of Physics window


        // --- Camera Window
        ImGui::Begin("Current Camera");

        for (auto const& camera : cameraComponents) {
            std::string entityLabel = entityNames.at(camera.first);
            if (ImGui::Selectable(entityLabel.c_str(), cameraID == camera.first)) {
                cameraID = camera.first;
            }
        }

        ImGui::End(); // End of Camera window


        // --- Settings Window ---
        ImGui::Begin("Settings");
        ImGui::Text("Reference Frame");
        switch (type_reference_frame) {
        case 2:
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.43f, 0.7f, 0.75f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.43f, 0.9f, 0.9f));
            if (ImGui::Button(" Full Reference Frame ", ImVec2(-1.0f, 0.0f)))
                type_reference_frame = 0;
            ImGui::PopStyleColor(2);
            break;
        case 1:
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.43f, 0.5f, 0.55f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.43f, 0.7f, 0.7f));
            if (ImGui::Button(" Partial Reference Frame ", ImVec2(-1.0f, 0.0f)))
            {
                type_reference_frame = 2;
                lineSystem->reset_reference_frame();
            }
            ImGui::PopStyleColor(2);
            break;
        default:
            if (ImGui::Button(" Hidden Reference Frame ", ImVec2(-1.0f, 0.0f)))
                type_reference_frame = (grid_display) ? 1 : 2;
            break;
        }

        ImGui::Text("Grid");
        if (grid_display)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.43f, 0.7f, 0.75f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.43f, 1.0f, 1.0f));
            if (ImGui::Button(" Displayed Grid ", ImVec2(-1.0f, 0.0f)))
            {
                grid_display = false;
                if (type_reference_frame == 1)
                {
                    type_reference_frame = 2;
                    lineSystem->reset_reference_frame();
                }
            }
            ImGui::PopStyleColor(2);
        }
        else
        {
            if (ImGui::Button(" Hidden Grid ", ImVec2(-1.0f, 0.0f)))
                grid_display = true;
        }

        // TODO: Make scale work
        ImGui::Text("Gizmo Settings");
        switch (gizmo_type) {
        case ImGuizmo::OPERATION::SCALE:
            if (ImGui::Button(" Scaling #LOCK ", ImVec2(-1.0f, 0.0f)))
                gizmo_type = -1;
            break;
        case ImGuizmo::OPERATION::ROTATE:
            if (ImGui::Button(" Rotation ", ImVec2(-1.0f, 0.0f)))
                gizmo_type = ImGuizmo::OPERATION::SCALE;
            break;
        case ImGuizmo::OPERATION::TRANSLATE:
            if (ImGui::Button(" Translation ", ImVec2(-1.0f, 0.0f)))
            {
                gizmo_type = ImGuizmo::OPERATION::ROTATE;
            }
            break;
        default:
            if (ImGui::Button(" None ", ImVec2(-1.0f, 0.0f)))
                gizmo_type = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }

        if (ImGui::Button((gizmo_world) ? " Gizmo World " : " Gizmo Local ", ImVec2(-1.0f, 0.0f)))
            gizmo_world = !gizmo_world;

        ImGui::End(); // End of Settings window


        // ImGuizmo
        if (gizmo_type != -1) {
            ImGuizmo::SetOrthographic(false);
            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

            auto camera = transformComponents[cameraID];
            glm::mat4 cameraView = cameraSystem->GetViewMatrix();
            glm::mat4 cameraProjection = cameraSystem->GetProjectionMatrix();
            glm::mat4 transformSelectedEntity = glm::translate(glm::mat4(1.0f), transformComponents[selectedEntityID].position) *
                glm::toMat4(transformComponents[selectedEntityID].eulers) *
                glm::scale(glm::mat4(1.0f), transformComponents[selectedEntityID].size);

            // Manipulation de l'entité sélectionnée avec ImGuizmo
            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)gizmo_type, (ImGuizmo::MODE)gizmo_world,
                glm::value_ptr(transformSelectedEntity));

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;

                DecomposeTransform(transformSelectedEntity, translation, rotation, scale);

                glm::quat currentRotation = glm::quat(rotation);

                if (physicsComponents.find(selectedEntityID) != physicsComponents.end())
                {
                    physx::PxTransform transform = physicsComponents[selectedEntityID].rigidBody->getGlobalPose();

                    transform.p.x = translation.x;
                    transform.p.y = translation.y;
                    transform.p.z = translation.z;

                    transform.q = physx::PxQuat(currentRotation.x, currentRotation.y, currentRotation.z, currentRotation.w);
                    physicsComponents[selectedEntityID].rigidBody->setGlobalPose(transform);
                }
                else if (staticPhysicsComponents.find(selectedEntityID) != staticPhysicsComponents.end()) {
                    physx::PxTransform transform = staticPhysicsComponents[selectedEntityID].rigidBody->getGlobalPose();

                    transform.p.x = translation.x;
                    transform.p.y = translation.y;
                    transform.p.z = translation.z;

                    transform.q = physx::PxQuat(currentRotation.x, currentRotation.y, currentRotation.z, currentRotation.w);
                    staticPhysicsComponents[selectedEntityID].rigidBody->setGlobalPose(transform);
                }

                transformComponents[selectedEntityID].position = translation;
                transformComponents[selectedEntityID].eulers = currentRotation;
                transformComponents[selectedEntityID].size = scale;
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/// <summary>
/// Set up the OpenGL context including Shader linkage and projection matrix
/// </summary>
void App::set_up_opengl() {

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //Set the rendering region to the actual screen size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    //(left, top, width, height)
    glViewport(0, 0, w, h);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
  /*  glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);*/

    //WIREFRAME MOD : 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shader = make_shader(
        "shaders/shader.vert",
        "shaders/shader.frag");

    shadowShader = make_shader(
        "shaders/shadowShader.vert",
        "shaders/shadowShader.frag");

	depthMapDebugShader = make_shader(
		"shaders/depthMap.vert",
		"shaders/depthMap.frag");
	reflectionShader = make_shader(
		"shaders/reflection.vert",
		"shaders/reflection.frag");


    glUseProgram(shader);
    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    //TODO : add configurable perspective :
}   

void App::make_systems() {
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
	shadowSystem = new ShadowSystem(shader,shadowShader, depthMapDebugShader);
    renderSystem = new RenderSystem(shader, window, shadowSystem->getShadowMapArray());
    lineSystem = new LineSystem();
	reflectionSystem = new ReflectionSystem(shader,reflectionShader);
	lightSystem = new LightSystem(shader, reflectionShader);

}

void App::loadModelsAndTextures()
{
 //   // Lane
	//const int lane = make_entity("Lane");
 //   loadGLTF("obj/nashville/Piste.gltf", "obj/nashville/", lane);
	//auto it = renderComponents[lane].begin();   
 //   it->isPlanarReflectable = true;
 //   std::advance(it, 1); 
 //   it->isPlanarReflectable = true;


 //   // Lane 2
	//const int lane2 = make_entity("Lane2");
 //   loadGLTF("obj/nashville/Piste2.gltf", "obj/nashville/", lane2);
 //   auto it2 = renderComponents[lane2].begin();
 //   std::advance(it2, 1);
 //   it2->isPlanarReflectable = true;
 //   std::advance(it2, 1);

 //   it2->isPlanarReflectable = true;
 //   transformComponents[lane2].position = { 0.0f, 0.0f, 13.0f };
 //  
 //   //Ball Return
	//const int ballreturn = make_entity("BallReturn");
	//loadGLTF("obj/nashville/BallReturn.gltf", "obj/nashville/", ballreturn);
 //   transformComponents[ballreturn].position = { -3.861, 0, 9.00 };
 //   transformComponents[ballreturn].eulers = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

 //   //TV
 //   const int TV = make_entity("TV");
	//loadGLTF("obj/nashville/TV.gltf", "obj/nashville/", TV);
 //   transformComponents[TV].position = { -3.568f, 3.356f, 9.0f };
 //   transformComponents[TV].eulers = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

 //   //Pin Statue
 //   const int PinStatue = make_entity("PinStatue");
 //   loadGLTF("obj/nashville/PinStatue.gltf", "obj/nashville/", PinStatue); 
 //   transformComponents[PinStatue].position = { -22.778, 1.668, 9.00f };
 //   transformComponents[PinStatue].eulers = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

 //   //Ball1
	//const int Ball1 = make_entity("Ball1");
	//loadGLTF("obj/nashville/Ball1.gltf", "obj/nashville/", Ball1);
 //   transformComponents[Ball1].position = { -1.913, 0.474, 8.776 };

	////Ball2
	//const int Ball2 = make_entity("Ball2");
	//loadGLTF("obj/nashville/Ball2.gltf", "obj/nashville/", Ball2);
	//transformComponents[Ball2].position = { 1.0f, 0.f, 0.5f };

	////Ball3
	//const int Ball3 = make_entity("Ball3");
	//loadGLTF("obj/nashville/Ball3.gltf", "obj/nashville/", Ball3);
	//transformComponents[Ball3].position = { 1.0f, 0.f, 1.0f };

    const int Dragon = make_entity("Dragon");
    loadGLTF("obj/avocado/Avocado.gltf", "obj/avocado/", Dragon);


    //// Ball
    //std::pair<unsigned int, unsigned int> ballModel = make_model("obj/servoskull/boule.obj");
    //renderModels["Ball"] = ballModel;
    //texturesList["Ball"] = make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
    //normalMapsList["Ball"] = make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);

    //// Pin
    ////motionSystem->concaveToConvex("obj/servoskull/quille.obj", "obj/convexMesh/", "quille");
    //std::vector<physx::PxConvexMesh*> meshes;
    //motionSystem->loadObjToPhysX("obj/convexMesh/quille.obj", meshes);

    //std::pair<unsigned int, unsigned int> pinModel = make_model("obj/servoskull/quille.obj");
    //physicsModels["Pin"] = meshes;
    //renderModels["Pin"] = pinModel;
    //texturesList["Pin"] = make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
    //normalMapsList["Pin"] = make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);

    //// Light
    //std::pair<unsigned int, unsigned int> defaultCubeModel = make_cube_mesh({ 0.1f, 0.1f, 0.1f });
    //renderModels["Light"] = defaultCubeModel;
    //texturesList["Light"] = make_texture("tex/lightTex.png", false);

    

}

void App::loadEntities()
{
    TransformComponent transform;
    RenderComponent render;
    PhysicsComponent physics;
    StaticPhysicsComponent SPhysics;
    LightComponent light;
    CameraComponent camera;

    // Lane
   /* unsigned int lane = make_entity("Lane");
    transform.position = { 0.f, 0.f, 9.f };
    transform.eulers = { 0.f, 0.f, 0.0f, 0.f };
    transform.size = { 1.0f, 0.168f, 18.0f };
    transformComponents[lane] = transform;*/

    //glm::vec3 laneMaterial = { 0.05f, 0.05f, 0.0f };
    //physx::PxBoxGeometry laneGeometry(physx::PxVec3(transform.size.x / 2.0f, transform.size.y / 2.0f, transform.size.z / 2.0f));

    //SPhysics.rigidBody = motionSystem->createStatic(laneGeometry, laneMaterial, transform.position);
    //staticPhysicsComponents[lane] = SPhysics;

    //render.mesh = renderModels["SM_Speaker_18-2"].first;
    //render.indexCount = renderModels["SM_Speaker_18-2"].second;
    ////render.material = texturesList["Lane"];
    //renderComponents[lane] = render;

 //   // Ball
 //   unsigned int ball = make_entity("Ball");
 //   transform.position = { 0.0f, 0.105f, 3.0f };
 //   transform.eulers = { 0.0f, 0.0f, 0.0f , 0.f };
 //   transformComponents[ball] = transform;

 //   glm::vec3 ballMaterial = { 0.05f, 0.05f, 0.2f };
 //   const physx::PxSphereGeometry ballGeometry(0.105f);

 //   physics.rigidBody = motionSystem->createDynamic(ballGeometry, ballMaterial, transform.position, 6.8f);
 //   physicsComponents[ball] = physics;

 //   std::tuple<unsigned int, unsigned int> ballModel = make_model("obj/servoskull/boule.obj");

 //   render.mesh = renderModels["Ball"].first;
 //   render.indexCount = renderModels["Ball"].second;
 //   render.material = texturesList["Ball"];
	//render.normalMap = normalMapsList["Ball"];
 //   renderComponents[ball] = render;

 //   camera.fov = 60.0f;
 //   camera.aspectRatio = 16.0f / 9.0f;
 //   camera.nearPlane = 0.1f;
 //   camera.farPlane = 100.0f;
 //   camera.sensitivity = 0.5f;
 //   camera.initialForward = { 0,0,1,0 };
 //   cameraComponents[ball] = camera;

 //   // Pins
 //   glm::vec3 first_pin = { 0.f, 0.22f, 15.f };
 //   glm::vec3 vectors[10];
 //   vectors[0] = first_pin;
 //   vectors[1] = glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.2635f);
 //   vectors[2] = glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.2635f);
 //   vectors[3] = glm::vec3(first_pin.x - 0.3048f, first_pin.y, first_pin.z + 0.527f);
 //   vectors[4] = glm::vec3(first_pin.x, first_pin.y, first_pin.z + 0.527f);
 //   vectors[5] = glm::vec3(first_pin.x + 0.3048f, first_pin.y, first_pin.z + 0.527f);
 //   vectors[6] = glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.7905f);
 //   vectors[7] = glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.7905f);
 //   vectors[8] = glm::vec3(first_pin.x + 0.4572f, first_pin.y, first_pin.z + 0.7905f);
 //   vectors[9] = glm::vec3(first_pin.x - 0.4572f, first_pin.y, first_pin.z + 0.7905f);

 //   glm::vec3 pinMaterial = { 0.5f, 0.5f, 0.5f };
 //   for (int i = 0; i < 10; i++) {
 //       unsigned int pin = make_entity("Pin " + std::to_string(i));
 //       transform.position = vectors[i];
 //       transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
 //       transformComponents[pin] = transform;

 //       physics.rigidBody = motionSystem->createDynamic(physicsModels["Pin"], pinMaterial, transform.position, 1.5f, 0.01f, 0.2f, 0.3f, 255, 255);
 //       physicsComponents[pin] = physics;

 //       render.mesh = renderModels["Pin"].first;
 //       render.indexCount = renderModels["Pin"].second;
	//	render.material = texturesList["Pin"];
	//	render.normalMap = normalMapsList["Pin"];
 //       renderComponents[pin] = render;

 //       camera.fov = 45.0f;
 //       camera.aspectRatio = 16.0f / 9.0f;
 //       camera.nearPlane = 0.1f;
 //       camera.farPlane = 100.0f;
 //       camera.sensitivity = 0.5f;
 //       camera.initialForward = { 0,0,-1,0 };
 //       cameraComponents[pin] = camera;
 //   }

 //   // Camera
    unsigned int cameraEntity = make_entity("Camera");
    transform.position = { 0.0f, 0.0f, 0.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    transformComponents[cameraEntity] = transform;
    
    camera.fov = 45.0f;
    camera.aspectRatio = 16.0f / 9.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.sensitivity = 0.5f;
    camera.initialForward = { 0,0,1,0 };
    cameraComponents[cameraEntity] = camera;
    cameraID = cameraEntity;

    //First light
    unsigned int lightEntity1 = make_entity("First Light");
    transform.position = { -1.964f, 4.687f, 8.845f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    transformComponents[lightEntity1] = transform;

    light.color = { 0.0f, 1.0f, 1.0f };
    light.intensity = 2.0f;
    light.type = SPOT;
    light.direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
    light.cutoff = glm::cos(glm::radians(15.0f));
    light.outerCutoff = glm::cos(glm::radians(25.0f));
    lightComponents[lightEntity1] = light;

    render.mesh = renderModels["Light"].first;
    render.indexCount = renderModels["Light"].second;
    render.material = texturesList["Light"];
    renderComponents[lightEntity1].push_back(render);

    LightComponent light2;
    //Second light: 
    unsigned int lightEntity2 = make_entity("Second Light");
    transform.position = { -2.396, 0.762, 9.091};
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    transformComponents[lightEntity2] = transform;

    light2.color = { 1.0f, 1.0f, 1.0f };
    light2.intensity = 1.0f;
    light2.type = POINT;
    //light2.direction = { 1.0f, -6.0f, 4.0f };
    light2.direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
	light2.cutoff = glm::cos(glm::radians(15.0f));
	light2.outerCutoff = glm::cos(glm::radians(25.0f));
    lightComponents[lightEntity2] = light2;

    render.mesh = renderModels["Light"].first;
    render.indexCount = renderModels["Light"].second;
    render.material = texturesList["Light"];
    renderComponents[lightEntity2].push_back(render);

}

MotionSystem* App::getMotionSystem()
{
    return motionSystem;
}

std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> App::getRenderModels() const
{
    return renderModels;
}

std::unordered_map<std::string, unsigned int> App::getTexturesList() const
{
    return texturesList;
}

std::unordered_map<std::string, std::vector<physx::PxConvexMesh*>> App::getPhysicsModels() const
{
    return physicsModels;
}

int App::getEntityByName(std::string name) const
{
    int id = -1;
    for (const auto& pair : entityNames) {
        if (pair.second == name) {
            id = pair.first;
        }
    }
    return id;


}



/// <summary>
/// Callback function for when the window is resized
/// </summary>
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
	screenWidth = width;
	screenHeight = height;


    glViewport(0, 0, width, height);
}

/// <summary>
/// Function that sets up the GLFW window and OpenGL context
/// </summary>
void App::set_up_glfw() {

    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(1920, 1080, "Guillaume Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Couldn't load GLAD" << std::endl;
        glfwTerminate();
    }

    //Init ImGui : 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

}
// Helper: stride is 14 floats per vertex (position, normal, tex, tangent, bitangent)
static const int VERTEX_STRIDE = 14;

// Get position of vertex 'idx' from the flat vertices array.
glm::vec3 getVertexPosition(const std::vector<float>& vertices, unsigned int idx) {
    return glm::vec3(vertices[idx * VERTEX_STRIDE + 0],
        vertices[idx * VERTEX_STRIDE + 1],
        vertices[idx * VERTEX_STRIDE + 2]);
}

// Set position of vertex 'idx' in the flat vertices array.
void setVertexPosition(std::vector<float>& vertices, unsigned int idx, const glm::vec3& pos) {
    vertices[idx * VERTEX_STRIDE + 0] = pos.x;
    vertices[idx * VERTEX_STRIDE + 1] = pos.y;
    vertices[idx * VERTEX_STRIDE + 2] = pos.z;
}

// Compute outer product of a glm::vec4 with itself.
glm::mat4 outerProduct(const glm::vec4& v) {
    glm::mat4 m(0.0f);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = v[i] * v[j];
    return m;
}

// Compute the plane equation for a triangle (p0, p1, p2).
// Returns a vec4 (A, B, C, D) such that Ax+By+Cz+D=0.
glm::vec4 computePlane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
    glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
    float d = -glm::dot(normal, p0);
    return glm::vec4(normal, d);
}

// A quadric error metric is stored as a 4x4 matrix.
typedef glm::mat4 Quadric;

// Structure to hold an edge candidate for collapse.
struct EdgeCandidate {
    unsigned int v1, v2;   // Endpoints (v1 is the surviving vertex)
    float error;           // QEM error for collapsing this edge.
    glm::vec3 newPos;      // Optimal new position after collapse.
};

// Comparator for the priority queue (min–heap based on error).
struct EdgeCandidateCompare {
    bool operator()(const EdgeCandidate& a, const EdgeCandidate& b) const {
        return a.error > b.error;
    }
};

// To avoid duplicate edges, we define an Edge structure (v1 < v2).
struct Edge {
    unsigned int v1, v2;
    Edge(unsigned int a, unsigned int b) {
        if (a < b) { v1 = a; v2 = b; }
        else { v1 = b; v2 = a; }
    }
    bool operator==(const Edge& other) const {
        return v1 == other.v1 && v2 == other.v2;
    }
};

// Custom hash for Edge.
struct EdgeHash {
    std::size_t operator()(const Edge& e) const {
        return std::hash<unsigned int>()(e.v1) ^ std::hash<unsigned int>()(e.v2);
    }
};

// Build per-vertex quadrics from the mesh data.
void computeVertexQuadrics(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, std::vector<Quadric>& quadrics) {
    size_t vertexCount = vertices.size() / VERTEX_STRIDE;
    quadrics.resize(vertexCount, Quadric(0.0f));

    // For each triangle face.
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int idx0 = indices[i];
        unsigned int idx1 = indices[i + 1];
        unsigned int idx2 = indices[i + 2];

        glm::vec3 p0 = getVertexPosition(vertices, idx0);
        glm::vec3 p1 = getVertexPosition(vertices, idx1);
        glm::vec3 p2 = getVertexPosition(vertices, idx2);

        glm::vec4 plane = computePlane(p0, p1, p2);
        glm::mat4 Kp = outerProduct(plane);

        quadrics[idx0] += Kp;
        quadrics[idx1] += Kp;
        quadrics[idx2] += Kp;
    }
}

// Compute an edge candidate collapse between vertices v1 and v2.
EdgeCandidate computeEdgeCandidate(unsigned int v1, unsigned int v2,
    const std::vector<Quadric>& quadrics, const std::vector<float>& vertices)
{
    Quadric Q = quadrics[v1] + quadrics[v2];

    // Modify Q so that the last row becomes [0,0,0,1]
    glm::mat4 Qtemp = Q;
    Qtemp[0][3] = 0.0f;
    Qtemp[1][3] = 0.0f;
    Qtemp[2][3] = 0.0f;
    Qtemp[3][0] = 0.0f;
    Qtemp[3][1] = 0.0f;
    Qtemp[3][2] = 0.0f;
    Qtemp[3][3] = 1.0f;

    glm::vec3 optimalPos;
    float det = glm::determinant(Qtemp);
    if (std::fabs(det) > 1e-6f) {
        glm::vec4 v = glm::inverse(Qtemp) * glm::vec4(0, 0, 0, 1);
        optimalPos = glm::vec3(v) / v.w;
    }
    else {
        // Fallback: use the midpoint
        glm::vec3 p1 = getVertexPosition(vertices, v1);
        glm::vec3 p2 = getVertexPosition(vertices, v2);
        optimalPos = (p1 + p2) * 0.5f;
    }
    glm::vec4 vOptimal(optimalPos, 1.0f);
    float error = glm::dot(vOptimal, Q * vOptimal);

    EdgeCandidate candidate;
    candidate.v1 = v1;
    candidate.v2 = v2;
    candidate.newPos = optimalPos;
    candidate.error = error;
    return candidate;
}

// Rebuild candidate edge list from current mesh (based on indices).
void buildEdgeCandidates(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
    const std::vector<Quadric>& quadrics, const std::vector<bool>& valid,
    std::priority_queue<EdgeCandidate, std::vector<EdgeCandidate>, EdgeCandidateCompare>& queue)
{
    std::unordered_set<Edge, EdgeHash> uniqueEdges;
    // Iterate over all triangles.
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int idx[3] = { indices[i], indices[i + 1], indices[i + 2] };
        // Only consider faces with valid vertices.
        if (!valid[idx[0]] || !valid[idx[1]] || !valid[idx[2]])
            continue;
        uniqueEdges.insert(Edge(idx[0], idx[1]));
        uniqueEdges.insert(Edge(idx[1], idx[2]));
        uniqueEdges.insert(Edge(idx[2], idx[0]));
    }
    // Build candidate queue.
    while (!queue.empty())
        queue.pop();
    for (const auto& edge : uniqueEdges) {
        // Ensure both vertices are still valid.
        if (!valid[edge.v1] || !valid[edge.v2])
            continue;
        EdgeCandidate candidate = computeEdgeCandidate(edge.v1, edge.v2, quadrics, vertices);
        queue.push(candidate);
    }
}

// Remove degenerate triangles (triangles where any two vertex indices are the same).
// Remove triangles with duplicate indices OR zero area.
void removeDegenerateTriangles(std::vector<unsigned int>& indices, const std::vector<float>& vertices) {
    std::vector<unsigned int> newIndices;
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        // Check for duplicate indices (index-based degeneracy)
        if (i0 == i1 || i1 == i2 || i2 == i0) {
            continue; // Skip this triangle
        }

        // Check for geometric degeneracy (zero area)
        glm::vec3 p0 = getVertexPosition(vertices, i0);
        glm::vec3 p1 = getVertexPosition(vertices, i1);
        glm::vec3 p2 = getVertexPosition(vertices, i2);
        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;
        float area = glm::length(glm::cross(edge1, edge2));

        if (area < 1e-6f) { // Threshold for "zero" area
            continue; // Skip this triangle
        }

        newIndices.push_back(i0);
        newIndices.push_back(i1);
        newIndices.push_back(i2);
    }
    indices = std::move(newIndices);
}
// Simplify the mesh until the number of valid vertices is <= targetCount.
// vertices: flat array of floats (14 per vertex)
// indices: list of triangle indices
void simplifyMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, int targetCount) {
    size_t vertexCount = vertices.size() / VERTEX_STRIDE;
    std::vector<Quadric> quadrics;
    computeVertexQuadrics(vertices, indices, quadrics);

    // Valid flag for each vertex; initially all true.
    std::vector<bool> valid(vertexCount, true);

    // Priority queue for edge candidates.
    std::priority_queue<EdgeCandidate, std::vector<EdgeCandidate>, EdgeCandidateCompare> edgeQueue;
    buildEdgeCandidates(vertices, indices, quadrics, valid, edgeQueue);

    int validCount = vertexCount;
    while (validCount > targetCount && !edgeQueue.empty()) {
        EdgeCandidate candidate = edgeQueue.top();
        edgeQueue.pop();

        // Check if both vertices are still valid.
        if (!valid[candidate.v1] || !valid[candidate.v2])
            continue;

        // Collapse edge: move v1 to the new optimal position; mark v2 as removed.
        setVertexPosition(vertices, candidate.v1, candidate.newPos);
        quadrics[candidate.v1] += quadrics[candidate.v2];
        valid[candidate.v2] = false;
        validCount--;

        // Update indices: replace occurrences of v2 with v1.
        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] == candidate.v2) {
                indices[i] = candidate.v1;
            }
        }

        // Remove degenerate triangles to avoid holes.
        removeDegenerateTriangles(indices, vertices);

        // Rebuild candidate edge list (for simplicity, we rebuild all candidates here)
        buildEdgeCandidates(vertices, indices, quadrics, valid, edgeQueue);
    }
}


void App::loadGLTF(const char* filePath, const char* texDir, const int EntityID) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    TransformComponent transform;
    RenderComponent render;
    unsigned int obj = EntityID;
    transform.position = { 0.f, 0.f, 9.f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    transform.size = { 1.0f, 0.168f, 18.0f };
    transformComponents[obj] = transform;


    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        aiMesh* mesh = scene->mMeshes[i];
        std::cout << "NUM Vertices : " << mesh->mNumVertices << std::endl;

        // Extract vertices
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            // Position
            vertices.push_back(mesh->mVertices[j].x);
            vertices.push_back(mesh->mVertices[j].y);
            vertices.push_back(mesh->mVertices[j].z);

            // Normal
            vertices.push_back(mesh->mNormals[j].x);
            vertices.push_back(mesh->mNormals[j].y);
            vertices.push_back(mesh->mNormals[j].z);

            // Texture coordinates (if available)
            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][j].x);
                vertices.push_back(mesh->mTextureCoords[0][j].y);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            // Tangent
            vertices.push_back(mesh->mTangents[j].x);
            vertices.push_back(mesh->mTangents[j].y);
            vertices.push_back(mesh->mTangents[j].z);

            // Bitangent
            vertices.push_back(mesh->mBitangents[j].x);
            vertices.push_back(mesh->mBitangents[j].y);
            vertices.push_back(mesh->mBitangents[j].z);
        }

        // Extract indices
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            const aiFace& face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }

        std::string diffuseTexturePath = "default_diffuse.jpg";
        std::string normalTexturePath = "default_normal.jpg";
        std::string emissiveTexturePath = "default_emissive.jpg";
        std::string aoTexturePath = "default_ao.jpg";
        std::string metalnessRoughness = "default_metalness_roughness.jpg";

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            diffuseTexturePath = std::string(texDir) + path.C_Str();
            std::cout << "Diffuse texture: " << path.C_Str() << std::endl;
            texturesList[mesh->mName.C_Str()] = make_texture(diffuseTexturePath.c_str(), false);
        }

        if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
            normalTexturePath = std::string(texDir) + path.C_Str();
            std::cout << "Normal texture: " << std::string(texDir) + path.C_Str() << std::endl;
            normalMapsList[mesh->mName.C_Str()] = make_texture(normalTexturePath.c_str(), false);
        }

        if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS) {
            emissiveTexturePath = std::string(texDir) + path.C_Str();
            std::cout << "Emissive texture: " << path.C_Str() << std::endl;
            emissiveList[mesh->mName.C_Str()] = make_texture(emissiveTexturePath.c_str(), false);

        }

        if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS) {
            aoTexturePath = std::string(texDir) + path.C_Str();
            std::cout << "AO texture: " << path.C_Str() << std::endl;
            aoList[mesh->mName.C_Str()] = make_texture(aoTexturePath.c_str(), false);
        }

        if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS) {
            metalnessRoughness = std::string(texDir) + path.C_Str();
            std::cout << "Roughness texture: " << path.C_Str() << std::endl;
            metalnessRoughnessList[mesh->mName.C_Str()] = make_texture(metalnessRoughness.c_str(), false);
        }

        int targetVertexCount = 350;
        simplifyMesh(vertices, indices, targetVertexCount);

        // Create VAO
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        VAOs.push_back(VAO);
        glBindVertexArray(VAO);

        // Create VBO for vertices
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        VBOs.push_back(VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Create EBO for indices
        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0); // 8 * sizeof(float) stride (3 for pos, 3 for normal, 2 for tex)


        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float[3]))); // Normal starts after 3 floats for position


        // Normal attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(sizeof(float[6]))); // Tex coords start after 6 floats (3 for pos, 3 for normal)

        // Tangent attribute
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float))); // Tangent starts after 8 floats (3 for pos, 3 for normal, 2 for tex)

        // Bitangent attribute
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float))); // Bitangent starts after 11 floats (3 for pos, 3 for normal, 2 for tex, 3 for tangent)

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Store VAO and index count
        renderModels[mesh->mName.C_Str()] = std::make_pair(VAO, indices.size());
        std::cout << mesh->mName.C_Str() << std::endl;
       

        render.mesh = renderModels[mesh->mName.C_Str()].first;
        render.indexCount = renderModels[mesh->mName.C_Str()].second;
        render.material = texturesList[mesh->mName.C_Str()];
        render.normalMap = normalMapsList[mesh->mName.C_Str()];
        render.emissiveMap = emissiveList[mesh->mName.C_Str()];
        render.aoMap = aoList[mesh->mName.C_Str()];
        render.metalnessRoughnessMap = metalnessRoughnessList[mesh->mName.C_Str()];
        renderComponents[obj].push_back(render);

    }


}