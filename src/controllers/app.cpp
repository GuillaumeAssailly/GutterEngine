#include "app.h"


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

    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;

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




std::tuple<unsigned int, unsigned int> App::make_model(const char * filePath) {

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
    return std::make_tuple(VAO, indices.size());
}

std::tuple<unsigned int, unsigned int> App::make_cube_mesh(glm::vec3 size) {

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

    // Return a tuple of the VAO and the number of indices
    return std::make_tuple(VAO, indices.size());
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

unsigned int App::make_normal(const char* filename, const bool flipTex) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(flipTex);
    unsigned char* data = stbi_load(
        filename, &width, &height, &channels, STBI_rgb); // Use STBI_rgb for normal map (optional)

    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;  // Return 0 if texture loading fails
    }

    // Generate and bind the texture to texture unit 2
    unsigned int texture;
    glGenTextures(1, &texture);
    textures.push_back(texture);
    glActiveTexture(GL_TEXTURE2); // Activate texture unit 2
    glBindTexture(GL_TEXTURE_2D, texture);

    // Load texture data into OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // GL_RGB format

    // Free the image data after loading
    stbi_image_free(data);

    // Configure texture sampler with mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps
}

//ImGui variables
unsigned int selectedEntityID = 0;


///<summary>
/// run methods launching the renderer pipeline :
///</summary>
void App::run() {
    // Variables to track FPS display
    float fpsTimeCounter = 0.0f;
    int frameCount = 0;

    //while loop iterating on the renderer pipeline : 


	shadowSystem->Initialize(lightComponents);
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        // -------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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

        // Update systems
        motionSystem->update(transformComponents, physicsComponents, deltaTime);
        bool should_close = cameraSystem->update(transformComponents, cameraID, *cameraComponent, deltaTime);
        if (should_close) {
            break;
        }
        lightSystem->update(lightComponents, transformComponents, cameraID);

        renderSystem->update(transformComponents, renderComponents, lightComponents);
        shadowSystem->GenerateShadowMap(lightComponents, transformComponents, renderComponents, screenWidth, screenHeight, *cameraComponent);

		manageImGui();  
    }

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void App::manageImGui() {

    // Start ImGui window for debugging
    ImGui::Begin("Debug");

    // Display FPS
    ImGui::Text("FPS: %f", 1.0f / deltaTime);

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

    // --- Inspector Window ---
    ImGui::Begin("Inspector");

    // If an entity is selected, show its components
    if (selectedEntityID < entity_count) {

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
            ImGui::DragFloat3("Velocity", &physics.velocity[0]);
            ImGui::DragFloat3("Euler Velocity", &physics.eulerVelocity[0]);
        }

        // Display LightComponent if present
        if (lightComponents.find(selectedEntityID) != lightComponents.end()) {
            ImGui::Text("Light Component");
            LightComponent& light = lightComponents[selectedEntityID];
            ImGui::ColorEdit3("Light Color", &light.color[0]);
            ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
			ImGui::Checkbox("Is Directional", &light.isDirectional);
            if (light.isDirectional == true) {
                ImGui::DragFloat3("Light Direction", &light.direction[0], 0.1);
            }
        }

        // Display RenderComponent if present
        if (renderComponents.find(selectedEntityID) != renderComponents.end()) {
            ImGui::Text("Render Component");
            RenderComponent& render = renderComponents[selectedEntityID];
            ImGui::InputInt("Mesh ID", (int*)&render.mesh);
        }
    }
    else {
        ImGui::Text("No entity selected.");
    }

    ImGui::End(); // End of Inspector window

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers to display the frame
    glfwSwapBuffers(window);
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

    shadowShader = make_shader(
        "shaders/shadowShader.vert",
        "shaders/shadowShader.frag");

	depthMapDebugShader = make_shader(
		"shaders/depthMap.vert",
		"shaders/depthMap.frag");



    glUseProgram(shader);
    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    //TODO : add configurable perspective :
    glm::mat4 projection = glm::perspective(  45.0f, (float)screenWidth / screenHeight, 0.01f, 100.0f);
	cameraComponent->projectionMatrix = projection;
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
}   

void App::make_systems() {
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
	lightSystem = new LightSystem(shader);
	shadowSystem = new ShadowSystem(shader,shadowShader, depthMapDebugShader);
    renderSystem = new RenderSystem(shader, window);
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