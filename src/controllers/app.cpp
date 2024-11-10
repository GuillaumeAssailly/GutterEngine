#include "app.h"
#define ENABLE_VHACD_IMPLEMENTATION 1
#include "VHACD.h"


App::App() {
    set_up_glfw();
}

App::~App() {
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
    glDeleteProgram(shader);

    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;

    glfwTerminate();
}


// Fonction principale pour décomposer un maillage concave en plusieurs parties convexes
void App::decomposeMeshWithVHACD(const std::string& inputFilePath, const std::string& outputDir, physx::PxPhysics* physics) {
    std::vector<double> vertices;
    std::vector<unsigned int> indices;


    VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
    VHACD::IVHACD::Parameters params;

    // Configure les paramètres en fonction des membres disponibles
    params.m_callback = nullptr;                       // Interface optionnelle pour les callbacks de progression
    params.m_logger = nullptr;                         // Interface optionnelle pour les messages de log
    params.m_taskRunner = nullptr;                     // Interface optionnelle pour créer des tâches
    params.m_maxConvexHulls = 64;                      // Nombre maximum de coques convexes
    params.m_resolution = 400000;                      // Résolution des voxels
    params.m_minimumVolumePercentErrorAllowed = 1.0;   // Pourcentage d'erreur de volume des voxels pour approximation
    params.m_maxRecursionDepth = 10;                   // Profondeur maximale de récursion
    params.m_shrinkWrap = true;                        // Réduction des positions des voxels sur le maillage source
    params.m_fillMode = VHACD::FillMode::FLOOD_FILL;   // Mode de remplissage du maillage voxelisé
    params.m_maxNumVerticesPerCH = 64;                 // Nombre max de sommets par coque convexe
    params.m_asyncACD = true;                          // Décomposition asynchrone pour tirer parti des cœurs supplémentaires
    params.m_minEdgeLength = 2;                        // Longueur minimale des arêtes de la voxelisation
    params.m_findBestPlane = false;                    // Tentative expérimentale de séparation sur le meilleur plan

    // Appliquer VHACD pour décomposer le maillage
    bool success = interfaceVHACD->Compute(
        vertices.data(),                       // Pointeur vers les données de sommets
        static_cast<unsigned int>(vertices.size() / 3),  // Nombre de sommets
        indices.data(),                        // Pointeur vers les indices des triangles
        static_cast<unsigned int>(indices.size() / 3),   // Nombre de triangles
        params                                 // Paramètres de décomposition
    );

    // Sauvegarder chaque morceau convexe dans un fichier .obj
    //saveConvexHullToObj(interfaceVHACD, outputDir);

    // Nettoyage
    interfaceVHACD->Clean();
    interfaceVHACD->Release();
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
        }

        // Extract indices
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
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

struct Vec3Hash {
    std::size_t operator()(const physx::PxVec3& vec) const {
        return std::hash<float>()(vec.x) ^ std::hash<float>()(vec.y) ^ std::hash<float>()(vec.z);
    }
};

void processNode(const aiScene* scene, aiNode* node, std::vector<physx::PxVec3>& vertices, std::unordered_set<physx::PxVec3, Vec3Hash>& uniqueVertices) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        if (mesh->mNumVertices > 0) {
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                physx::PxVec3 vertex = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };

                // Vérifiez si le sommet est déjà présent
                if (uniqueVertices.insert(vertex).second) {
                    vertices.push_back(vertex);
                }
            }

            std::cout << "Mesh name : " << node->mName.C_Str() << std::endl;
            std::cout << "Faces number : " << mesh->mNumFaces << std::endl; // Correction ici, pour le nombre de faces
            std::cout << "Vertices: " << vertices.size() << std::endl;
        }
        else {
            std::cout << "Warning: Mesh " << node->mName.C_Str() << " has no vertices!" << std::endl;
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        printf("Changement de child \n\n");
        processNode(scene, node->mChildren[i], vertices, uniqueVertices);
    }
}
physx::PxConvexMesh* App::make_physics_model(const char* filename) {
    physx::PxConvexMesh* convexMesh = nullptr;
    std::vector<physx::PxVec3> physxVertices;
    std::unordered_set<physx::PxVec3, Vec3Hash> uniqueVertices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        exit(-1);
    }

    processNode(scene, scene->mRootNode, physxVertices, uniqueVertices);
    printf("Taille: %d,    tailleUnique : %d\n", physxVertices.size(), uniqueVertices.size());

    convexMesh = motionSystem->createMesh(physxVertices);

    return convexMesh;
}

std::tuple<unsigned int, unsigned int> App::make_model(const char* filePath) {

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( filePath, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_FlipUVs);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // 8 * sizeof(float) stride (3 for pos, 3 for normal, 2 for tex)


    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float[3]))); // Normal starts after 3 floats for position
    


    // Normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float[6]))); // Tex coords start after 6 floats (3 for pos, 3 for normal)



    // Return a tuple of the VAO and the number of indices
    return std::make_tuple(VAO, indices.size());
}

unsigned int App::make_cube_mesh(glm::vec3 size) {

    float l = size.x;
    float h = size.y;
    float w = size.z;

    std::vector<float> vertices = {
         l,  w, -h, 1.0f, 1.0f,
         l, -w, -h, 1.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l,  w, -h, 0.0f, 1.0f,
         l,  w, -h, 1.0f, 1.0f,

        -l, -w,  h, 0.0f, 0.0f,
         l, -w,  h, 1.0f, 0.0f,
         l,  w,  h, 1.0f, 1.0f,
         l,  w,  h, 1.0f, 1.0f,
        -l,  w,  h, 0.0f, 1.0f,
        -l, -w,  h, 0.0f, 0.0f,

        -l,  w,  h, 1.0f, 1.0f,
        -l,  w, -h, 1.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l, -w,  h, 0.0f, 1.0f,
        -l,  w,  h, 1.0f, 1.0f,

         l, -w, -h, 0.0f, 0.0f,
         l,  w, -h, 1.0f, 0.0f,
         l,  w,  h, 1.0f, 1.0f,
         l,  w,  h, 1.0f, 1.0f,
         l, -w,  h, 0.0f, 1.0f,
         l, -w, -h, 0.0f, 0.0f,

        -l, -w, -h, 0.0f, 0.0f,
         l, -w, -h, 1.0f, 0.0f,
         l, -w,  h, 1.0f, 1.0f,
         l, -w,  h, 1.0f, 1.0f,
        -l, -w,  h, 0.0f, 1.0f,
        -l, -w, -h, 0.0f, 0.0f,

         l,  w,  h, 1.0f, 1.0f,
         l,  w, -h, 1.0f, 0.0f,
        -l,  w, -h, 0.0f, 0.0f,
        -l,  w, -h, 0.0f, 0.0f,
        -l,  w,  h, 0.0f, 1.0f,
         l,  w,  h, 1.0f, 1.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
        vertices.data(), GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);
    glEnableVertexAttribArray(0);
    //texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
    glEnableVertexAttribArray(1);

    return VAO;
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

void App::run() {
    // Variables to track FPS display
    float fpsTimeCounter = 0.0f;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        // -------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Count the number of frames and accumulate time
        frameCount++;
        fpsTimeCounter += deltaTime;

        // Calculate and display FPS in window title every second
        if (fpsTimeCounter >= 1.0f) {
            float fps = frameCount / fpsTimeCounter;
            std::string title = "FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, title.c_str());

            // Reset counters for the next FPS calculation
            frameCount = 0;
            fpsTimeCounter = 0.0f;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            physx::PxVec3 force(0, 0, 0.02);
            motionSystem->applyForceToActor(physicsComponents[1].rigidBody, force);
        }

        // Update systems
        motionSystem->update(transformComponents, physicsComponents, deltaTime);
        bool should_close = cameraSystem->update(transformComponents, cameraID, *cameraComponent, deltaTime);
        if (should_close) {
            break;
        }

        renderSystem->update(transformComponents, renderComponents);
    }
}

unsigned int App::make_entity()
{
    return entity_count++;
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



    glUseProgram(shader);
    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    glm::mat4 projection = glm::perspective(
       45.0f, 1920.0f / 1080.0f, 0.01f, 1000.0f);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
}   

void App::make_systems() {
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
    renderSystem = new RenderSystem(shader, window);
}



physx::PxRigidDynamic* App::createDynamic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform, float mass, float sleepT, float linearDamp, float angularDamp){
    return motionSystem->createDynamic(geometry, material, transform, mass, sleepT,linearDamp, angularDamp);
}

void App::createStatic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform) {
    motionSystem->createStatic(geometry, material, transform);
}

/// <summary>
/// Callback function for when the window is resized
/// </summary>
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}