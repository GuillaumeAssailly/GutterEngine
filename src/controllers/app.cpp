#include "app.h"

int screenWidth = 1920;
int screenHeight = 1080;

GLuint FBO, texture_id, RBO, resolveFBO, resolveTexture;
void create_framebuffer()
{
    int samples = 4;

    // Création du framebuffer multisample
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Création d'une texture multisample pour la couleur
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, screenWidth, screenHeight, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_id, 0);

    // Création d'un renderbuffer multisample pour la profondeur et le stencil
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer multisample is not complete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Création d'un framebuffer standard pour résoudre l'anti-aliasing
    glGenFramebuffers(1, &resolveFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, resolveFBO);

    glGenTextures(1, &resolveTexture);
    glBindTexture(GL_TEXTURE_2D, resolveTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Resolve framebuffer is not complete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void rescale_framebuffer(float width, float height)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

App::App() {
    set_up_glfw();
    set_up_opengl();
    create_framebuffer();
    entityManager = new EntityManager();
    systemManager = new SystemManager(window, shader, shadowShader, depthMapDebugShader, reflectionShader);
    meshManager = new MeshManager(entityManager);
    inputManager = new InputManager(systemManager, entityManager);
    ui = new UI(window, entityManager, meshManager, systemManager);

}

App::~App() {
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
    glDeleteProgram(reflectionShader);
    glDeleteProgram(shader);

    delete inputManager;
    delete ui;
    delete systemManager;
    delete meshManager;
    delete entityManager;

    glfwTerminate();
}

/*
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

static std::string error_msg = "";*/

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
    systemManager->reflectionSystem->Initialize(entityManager->renderComponents, 1024, 1024);
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


        bool should_close = inputManager->getInput(window, hasPhysics);
        if (should_close) {
            break;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        ui->update(screenWidth, screenHeight, resolveTexture, deltaTime);
        // Update systems
        if (accumulatedTime >= 0.00833) {
            systemManager->motionSystem->update(entityManager->transformComponents, entityManager->physicsComponents, accumulatedTime);
            accumulatedTime = 0.;
        }
        systemManager->lightSystem->update(entityManager->lightComponents, entityManager->transformComponents, entityManager->cameraID);
        systemManager->reflectionSystem->RenderReflection(entityManager->transformComponents, entityManager->renderComponents, systemManager->cameraSystem->GetViewMatrix(), systemManager->cameraSystem->GetProjectionMatrix(), screenWidth, screenHeight, entityManager->cameraID);
        systemManager->renderSystem->update(entityManager->transformComponents, entityManager->renderComponents, entityManager->lightComponents);
        systemManager->shadowSystem->GenerateShadowMap(entityManager->lightComponents, entityManager->transformComponents, entityManager->renderComponents, screenWidth, screenHeight, entityManager->cameraID);
        systemManager->lineSystem->render_lines_ref_frame_grid(type_reference_frame, grid_display, entityManager->transformComponents[entityManager->cameraID].position, shader);
        systemManager->cameraSystem->update(entityManager->transformComponents, entityManager->cameraComponents, entityManager->cameraID, deltaTime);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
 

       // End of Entity Tree window
        /*
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
            }j

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
            addRender = false;



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

        */
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
    glEnable(GL_SCISSOR_TEST);

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

	reflectionShader = make_shader(
		"shaders/reflection.vert",
		"shaders/reflection.frag");

    glUseProgram(shader);
    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    //TODO : add configurable perspective :
}   

void App::loadModelsAndTextures()
{
    // Lane
    meshManager->loadGLTF("obj/nashville/Piste.gltf", "obj/nashville/");

    // Lane 2
    meshManager->loadGLTF("obj/nashville/Piste2.gltf", "obj/nashville/");
   
    //Ball Return
    meshManager->loadGLTF("obj/nashville/BallReturn.gltf", "obj/nashville/");

    //TV
    meshManager->loadGLTF("obj/nashville/TV.gltf", "obj/nashville/");

    //Pin Statue
    meshManager->loadGLTF("obj/nashville/PinStatue.gltf", "obj/nashville/");

    //Ball1
    meshManager->loadGLTF("obj/nashville/Ball1.gltf", "obj/nashville/");

	//Ball2
    meshManager->loadGLTF("obj/nashville/Ball2.gltf", "obj/nashville/");

	//Ball3
    meshManager->loadGLTF("obj/nashville/Ball3.gltf", "obj/nashville/");


    // Ball
    std::pair<unsigned int, unsigned int> ballModel = meshManager->make_model("obj/servoskull/boule.obj");
    renderModels["Ball"] = ballModel;
    texturesList["Ball"] = meshManager->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
    normalMapsList["Ball"] = meshManager->make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);

    // Pin
    //motionSystem->concaveToConvex("obj/servoskull/quille.obj", "obj/convexMesh/", "quille");
    std::vector<physx::PxConvexMesh*> meshes;
    systemManager->motionSystem->loadObjToPhysX("obj/convexMesh/quille.obj", meshes);

    std::pair<unsigned int, unsigned int> pinModel = meshManager->make_model("obj/servoskull/quille.obj");
    physicsModels["Pin"] = meshes;
    renderModels["Pin"] = pinModel;
    texturesList["Pin"] = meshManager->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
    normalMapsList["Pin"] = meshManager->make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);

    // Light
    std::pair<unsigned int, unsigned int> defaultCubeModel = meshManager->make_cube_mesh({ 0.1f, 0.1f, 0.1f });
    renderModels["Light"] = defaultCubeModel;
    texturesList["Light"] = meshManager->make_texture("tex/lightTex.png", false);
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

    LightComponent light2;
    //Second light: 
    unsigned int lightEntity2 = entityManager->make_entity("Second Light");
    transform.position = { 0.0f, 4.0f, 4.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    entityManager->transformComponents[lightEntity2] = transform;

    light.color = { 1.0f, 1.0f, 1.0f };
    light.intensity = 1.0f;
    //light.isDirectional = true;
    light.direction = { 1.0f, -6.0f, 4.0f };
    entityManager->lightComponents[lightEntity2] = light;

    render.mesh = renderModels["Light"].first;
    render.indexCount = renderModels["Light"].second;
    render.material = texturesList["Light"];
    entityManager->renderComponents[lightEntity2].push_back(render);

}

EntityManager* App::getEntityManager()
{
    return entityManager;
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
}
