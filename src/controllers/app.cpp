#include "app.h"


int screenWidth = 1920;
int screenHeight = 1080;

App::App() {
    set_up_glfw();
    set_up_opengl();
    entityManager = new EntityManager();
    systemManager = new SystemManager(window, shader, shadowShader, depthMapDebugShader);
}

App::~App() {
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
    glDeleteProgram(shader);

    delete systemManager;
    delete entityManager;

    glfwTerminate();
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
    entityManager->transformComponents[obj] = transform;


    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        aiMesh* mesh = scene->mMeshes[i];

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

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            diffuseTexturePath =  std::string(texDir) + path.C_Str();
            std::cout << "Diffuse texture: " << path.C_Str() << std::endl;
            texturesList[mesh->mName.C_Str()] = make_texture(diffuseTexturePath.c_str(), false);
        }

		if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
			normalTexturePath = std::string(texDir) + path.C_Str();
			std::cout << "Normal texture: " << std::string(texDir) +  path.C_Str() << std::endl;
			normalMapsList[mesh->mName.C_Str()] = make_texture(normalTexturePath.c_str(), false);
		}


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
        entityManager->renderComponents[obj].push_back(render);
    }
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
    systemManager->shadowSystem->Initialize(entityManager->lightComponents);
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
            systemManager->motionSystem->applyForceToActor(entityManager->physicsComponents[1].rigidBody, force);
        }

        // Update systems
        if (accumulatedTime >= 0.00833) {
            systemManager->motionSystem->update(entityManager->transformComponents, entityManager->physicsComponents, accumulatedTime);
            accumulatedTime = 0.;
        }
        bool should_close = systemManager->cameraSystem->update(entityManager->transformComponents, entityManager->cameraComponents, entityManager->cameraID, deltaTime);
        if (should_close) {
            break;
        }
        systemManager->lightSystem->update(entityManager->lightComponents, entityManager->transformComponents, entityManager->cameraID);
        systemManager->renderSystem->update(entityManager->transformComponents, entityManager->renderComponents, entityManager->lightComponents);
        systemManager->shadowSystem->GenerateShadowMap(entityManager->lightComponents, entityManager->transformComponents, entityManager->renderComponents, screenWidth, screenHeight, entityManager->cameraID);

        //Draw Lines
        //Add here more lines to draw...
        systemManager->lineSystem->render_lines_ref_frame_grid(type_reference_frame, grid_display, entityManager->transformComponents[entityManager->cameraID].position, shader);


        // Start //ImGui window for debugging
        ImGui::Begin("Debug");

        // Display FPS
        ImGui::Text("FPS: %f", 1.0f / deltaTime);

        ImGui::End();

        // --- Entity Tree Window ---
        ImGui::Begin("Entity Tree");

        // Loop through all entities to create a tree view
        for (int entityID = 0; entityID < entityManager->entity_count; entityID++) {
            std::string entityLabel = entityManager->entityNames.at(entityID);

            // Display each entity as selectable
            if (ImGui::Selectable(entityLabel.c_str(), selectedEntityID == entityID)) {
                selectedEntityID = entityID; // Set the selected entity when clicked

            }
        }

        ImGui::End(); // End of Entity Tree window

        // If an entity is selected, show its components
        if (selectedEntityID < entityManager->entity_count && selectedEntityID != -1) {
            // --- Inspector Window ---
            ImGui::Begin("Inspector");


            // Display TransformComponent if present
            if (entityManager->transformComponents.find(selectedEntityID) != entityManager->transformComponents.end()) {
                ImGui::Text("Transform Component");
                TransformComponent& transform = entityManager->transformComponents[selectedEntityID];
                ImGui::DragFloat3("Position", &transform.position[0]);
                ImGui::DragFloat3("Rotation", &transform.eulers[0]);
            }

            // Display PhysicsComponent if present
            if (entityManager->physicsComponents.find(selectedEntityID) != entityManager->physicsComponents.end()) {
                ImGui::Text("Physics Component");
                PhysicsComponent& physics = entityManager->physicsComponents[selectedEntityID];
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
                    auto it = entityManager->physicsComponents.find(selectedEntityID);
                    if (it != entityManager->physicsComponents.end()) {
                        PhysicsComponent& physicsComponent = it->second;
                        physicsComponent.rigidBody->addForce(force, physx::PxForceMode::eIMPULSE);
                    }
                }
            }

            // Display LightComponent if present
            if (entityManager->lightComponents.find(selectedEntityID) != entityManager->lightComponents.end()) {
                ImGui::Text("Light Component");
                LightComponent& light = entityManager->lightComponents[selectedEntityID];
                ImGui::ColorEdit3("Light Color", &light.color[0]);
                ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
                ImGui::Checkbox("Is Directional", &light.isDirectional);
                if (light.isDirectional == true) {
                    ImGui::DragFloat3("Light Direction", &light.direction[0], 0.1);
                }

            }

            // Display RenderComponent if present
            if (entityManager->renderComponents.find(selectedEntityID) != entityManager->renderComponents.end()) {
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
                unsigned int id = entityManager->make_entity(newEntityName);
                TransformComponent transform;
                transform.position = newPosition;
                transform.eulers = newEulers;
                entityManager->transformComponents[id] = transform;

                if (addPhysics) {
                    PhysicsComponent physics;
                    glm::vec3 material = { newEntityStaticFriction, newEntityDynamicFriction, newEntityRestitution };
                    const physx::PxBoxGeometry boxGeometry(physx::PxVec3(boxSize.x / 2.f, boxSize.y / 2.f, boxSize.z / 2.f));
                    const physx::PxSphereGeometry sphereGeometry(sphereRadius);


                    switch (geometry) {
                    case 0:
                        if (!isStatic)
                            physics.rigidBody = systemManager->motionSystem->createDynamic(physicsModels[selectedModelName], material, newPosition, newEntityMass, newEntitySleepT, newEntityLinearDamping, newEntityAngularDamping);
                        else
                            systemManager->motionSystem->createStatic(physicsModels[selectedModelName], material, newPosition);
                        break;
                    case 1:
                        if (!isStatic)
                            physics.rigidBody = systemManager->motionSystem->createDynamic(boxGeometry, material, newPosition, newEntityMass, newEntitySleepT, newEntityLinearDamping, newEntityAngularDamping);
                        else
                            systemManager->motionSystem->createStatic(boxGeometry, material, newPosition);
                        break;
                    case 2:
                        if (!isStatic)
                            physics.rigidBody = systemManager->motionSystem->createDynamic(sphereGeometry, material, newPosition, newEntityMass, newEntitySleepT, newEntityLinearDamping, newEntityAngularDamping);
                        else
                            systemManager->motionSystem->createStatic(sphereGeometry, material, newPosition);
                        break;
                    }
                    if (!isStatic)
                        entityManager->physicsComponents[id] = physics;
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
                    entityManager->lightComponents[id] = light;
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

        for (auto const& camera : entityManager->cameraComponents) {
            std::string entityLabel = entityManager->entityNames.at(camera.first);
            if (ImGui::Selectable(entityLabel.c_str(), entityManager->cameraID == camera.first)) {
                entityManager->cameraID = camera.first;
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
                systemManager->lineSystem->reset_reference_frame();
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
                    systemManager->lineSystem->reset_reference_frame();
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

            auto camera = entityManager->transformComponents[entityManager->cameraID];
            glm::mat4 cameraView = systemManager->cameraSystem->GetViewMatrix();
            glm::mat4 cameraProjection = systemManager->cameraSystem->GetProjectionMatrix();
            glm::mat4 transformSelectedEntity = glm::translate(glm::mat4(1.0f), entityManager->transformComponents[selectedEntityID].position) *
                glm::toMat4(entityManager->transformComponents[selectedEntityID].eulers) *
                glm::scale(glm::mat4(1.0f), entityManager->transformComponents[selectedEntityID].size);

            // Manipulation de l'entité sélectionnée avec ImGuizmo
            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)gizmo_type, (ImGuizmo::MODE)gizmo_world,
                glm::value_ptr(transformSelectedEntity));

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;

                DecomposeTransform(transformSelectedEntity, translation, rotation, scale);

                glm::quat currentRotation = glm::quat(rotation);

                if (entityManager->physicsComponents.find(selectedEntityID) != entityManager->physicsComponents.end())
                {
                    physx::PxTransform transform = entityManager->physicsComponents[selectedEntityID].rigidBody->getGlobalPose();

                    transform.p.x = translation.x;
                    transform.p.y = translation.y;
                    transform.p.z = translation.z;

                    transform.q = physx::PxQuat(currentRotation.x, currentRotation.y, currentRotation.z, currentRotation.w);
                    entityManager->physicsComponents[selectedEntityID].rigidBody->setGlobalPose(transform);
                }
                else if (entityManager->staticPhysicsComponents.find(selectedEntityID) != entityManager->staticPhysicsComponents.end()) {
                    physx::PxTransform transform = entityManager->staticPhysicsComponents[selectedEntityID].rigidBody->getGlobalPose();

                    transform.p.x = translation.x;
                    transform.p.y = translation.y;
                    transform.p.z = translation.z;

                    transform.q = physx::PxQuat(currentRotation.x, currentRotation.y, currentRotation.z, currentRotation.w);
                    entityManager->staticPhysicsComponents[selectedEntityID].rigidBody->setGlobalPose(transform);
                }

                entityManager->transformComponents[selectedEntityID].position = translation;
                entityManager->transformComponents[selectedEntityID].eulers = currentRotation;
                entityManager->transformComponents[selectedEntityID].size = scale;
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Set the rendering region to the actual screen size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    //(left, top, width, height)
    glViewport(0, 0, w, h);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //WIREFRAME MOD : 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shader = make_shader(
        "shaders/shader.vert",
        "shaders/shader.frag");
    shader = make_shader(
        "shaders/shader.vert",
        "shaders/shader.frag");

    shadowShader = make_shader(
        "shaders/shadowShader.vert",
        "shaders/shadowShader.frag");

	depthMapDebugShader = make_shader(
		"shaders/depthMap.vert",
		"shaders/depthMap.frag");



    glUseProgram(shader);
    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    //TODO : add configurable perspective :
}   

void App::loadModelsAndTextures()
{
    // Lane
	const int lane = entityManager->make_entity("Lane");
    loadGLTF("obj/nashville/Piste.gltf", "obj/nashville/", lane);

    // Lane 2
	const int lane2 = entityManager->make_entity("Lane2");
    loadGLTF("obj/nashville/Piste2.gltf", "obj/nashville/", lane2);
   
    //Ball Return
	const int ballreturn = entityManager->make_entity("BallReturn");
	loadGLTF("obj/nashville/BallReturn.gltf", "obj/nashville/", ballreturn);

    //TV
    const int TV = entityManager->make_entity("TV");
	loadGLTF("obj/nashville/TV.gltf", "obj/nashville/", TV);

    //Pin Statue
    const int PinStatue = entityManager->make_entity("PinStatue");
	loadGLTF("obj/nashville/PinStatue.gltf", "obj/nashville/", PinStatue);

    //Ball1
	const int Ball1 = entityManager->make_entity("Ball1");
	loadGLTF("obj/nashville/Ball1.gltf", "obj/nashville/", Ball1);

	//Ball2
	const int Ball2 = entityManager->make_entity("Ball2");
	loadGLTF("obj/nashville/Ball2.gltf", "obj/nashville/", Ball2);

	//Ball3
	const int Ball3 = entityManager->make_entity("Ball3");
	loadGLTF("obj/nashville/Ball3.gltf", "obj/nashville/", Ball3);


    // Ball
    std::pair<unsigned int, unsigned int> ballModel = make_model("obj/servoskull/boule.obj");
    renderModels["Ball"] = ballModel;
    texturesList["Ball"] = make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
    normalMapsList["Ball"] = make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);

    // Pin
    //motionSystem->concaveToConvex("obj/servoskull/quille.obj", "obj/convexMesh/", "quille");
    std::vector<physx::PxConvexMesh*> meshes;
    systemManager->motionSystem->loadObjToPhysX("obj/convexMesh/quille.obj", meshes);

    std::pair<unsigned int, unsigned int> pinModel = make_model("obj/servoskull/quille.obj");
    physicsModels["Pin"] = meshes;
    renderModels["Pin"] = pinModel;
    texturesList["Pin"] = make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
    normalMapsList["Pin"] = make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);

    // Light
    std::pair<unsigned int, unsigned int> defaultCubeModel = make_cube_mesh({ 0.1f, 0.1f, 0.1f });
    renderModels["Light"] = defaultCubeModel;
    texturesList["Light"] = make_texture("tex/lightTex.png", false);
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
    unsigned int cameraEntity = entityManager->make_entity("Camera");
    transform.position = { 0.0f, 0.0f, 0.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    entityManager->transformComponents[cameraEntity] = transform;
    
    camera.fov = 45.0f;
    camera.aspectRatio = 16.0f / 9.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.sensitivity = 0.5f;
    camera.initialForward = { 0,0,1,0 };
    entityManager->cameraComponents[cameraEntity] = camera;
    entityManager->cameraID = cameraEntity;

    //First light
    unsigned int lightEntity1 = entityManager->make_entity("First Light");
    transform.position = { 0.0f, 0.0f, 0.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    entityManager->transformComponents[lightEntity1] = transform;

    light.color = { 0.0f, 1.0f, 1.0f };
    light.intensity = 1.0f;
    entityManager->lightComponents[lightEntity1] = light;

    render.mesh = renderModels["Light"].first;
    render.indexCount = renderModels["Light"].second;
    render.material = texturesList["Light"];
    entityManager->renderComponents[lightEntity1].push_back(render);

    //Second light: 
    unsigned int lightEntity2 = entityManager->make_entity("Second Light");
    transform.position = { 0.0f, 4.0f, 4.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    entityManager->transformComponents[lightEntity2] = transform;

    light.color = { 1.0f, 1.0f, 1.0f };
    light.intensity = 1.0f;
    light.isDirectional = true;
    light.direction = { 1.0f, -6.0f, 4.0f };
    entityManager->lightComponents[lightEntity2] = light;

    render.mesh = renderModels["Light"].first;
    render.indexCount = renderModels["Light"].second;
    render.material = texturesList["Light"];
    entityManager->renderComponents[lightEntity2].push_back(render);

}

MotionSystem* App::getMotionSystem()
{
    return systemManager->motionSystem;
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
    for (const auto& pair : entityManager->entityNames) {
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

    window = glfwCreateWindow(1920, 1080, "Gutter Engine", NULL, NULL);
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
