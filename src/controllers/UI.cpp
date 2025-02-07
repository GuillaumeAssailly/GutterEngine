#include "UI.h"

UI::UI(GLFWwindow* window, EntityManager* em, MeshManager* mm, SystemManager* sm)
{
    entityManager = em;
    meshManager = mm;
    systemManager = sm;

    //Init ImGui : 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

UI::~UI()
{
    delete entityManager;
    delete meshManager;
    delete systemManager;
}

void UI::displayFrameRate(double deltaTime)
{

}

void UI::displaySceneHierarchy()
{
    ImGui::Begin("Entity List", nullptr);

    // Loop through all entities to create a tree view
    for (const auto& [id, name] : entityManager->entityNames) {
        // Display each entity as selectable
        if (ImGui::Selectable(name.c_str(), selectedEntityID == id)) {
            selectedEntityID = id; // Set the selected entity when clicked
            displayInspector = true;
            remove_error_msg = "";
            strncpy_s(renameBuffer, sizeof(renameBuffer), name.c_str(), _TRUNCATE);
        }
    }
    ImGui::End();
}

void UI::displayCatalog()
{
    ImGui::Begin("Objects list");
    for (const auto& obj : meshManager->renderList)
    {
        // Détecter si l'élément est en train d'être glissé
        ImGui::Selectable(obj.first.c_str());
        ImGuiDragDropFlags src_flags = 0;
        src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
        src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
        //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
        if (ImGui::BeginDragDropSource(src_flags))
        {
            if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                ImGui::Text("Moving \"%s\"", obj.first.c_str());
            ImGui::SetDragDropPayload("OBJECT_TYPE", obj.first.c_str(), obj.first.size() + 1);
            ImGui::EndDragDropSource();
        }
    }
    ImGui::End();
}


void UI::CreateDockspace()
{

    // Obtenir le viewport principal (toute la fenêtre)
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Définir la position et la taille du dockspace (collé à gauche)
    ImVec2 dockspace_pos = ImVec2(0, 30);
    ImVec2 dockspace_size = ImVec2(viewport->WorkSize.x, viewport->WorkSize.y);

    ImGui::SetNextWindowPos(dockspace_pos);
    ImGui::SetNextWindowSize(dockspace_size);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Désactiver le scroll et empêcher les ajustements non contrôlés
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    // Style pour un rendu propre
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("DockSpace", nullptr, window_flags))
    {
        // Créer un espace de docking
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
}

void UI::displayEntityDetail()
{
    if (displayInspector && ImGui::Begin("Inspector", &displayInspector, ImGuiWindowFlags_NoCollapse)) {
        ImGui::SeparatorText("Name");

        if (ImGui::InputText(" ", renameBuffer, sizeof(renameBuffer))) {
        }

        ImGui::SameLine();

        if (ImGui::Button("Rename")) {
            entityManager->renameEntity(selectedEntityID, renameBuffer);
        }

        ImGui::SeparatorText("Transform");
        TransformComponent& transform = entityManager->transformComponents[selectedEntityID];
        ImGui::DragFloat3("Position", &transform.position[0]);
        ImGui::DragFloat3("Rotation", &transform.eulers[0]);

        ImGui::SeparatorText("Components");
        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
            if (entityManager->renderComponents[selectedEntityID].size() > 0 && ImGui::BeginTabItem("Render"))
            {
                int index = 0;
                for (auto& renderComponentList : entityManager->renderComponents[selectedEntityID]) {
                    ImGui::PushID(index);
                    if (ImGui::BeginCombo("Material", std::to_string(renderComponentList.material).c_str())) {
                        for (const auto& catalog : meshManager->renderList) {
                            int i = 0;
                            for (const auto& material : catalog.second) {
                                bool isSelected = (std::get<2>(material).baseColor == renderComponentList.material);
                                if (ImGui::Selectable((catalog.first + "_" + std::to_string(i)).c_str(), isSelected)) {
                                    renderComponentList.material = std::get<2>(material).baseColor;
                                }
                                if (isSelected)
                                    ImGui::SetItemDefaultFocus();
                                i++;
                            }       
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopID();
                    index++;
                }
                ImGui::EndTabItem();
            }

            if (entityManager->physicsComponents.count(selectedEntityID) > 0 && ImGui::BeginTabItem("Physics"))
            {
                ImGui::EndTabItem();
            }

            if (entityManager->lightComponents.count(selectedEntityID) > 0 && ImGui::BeginTabItem("Light")) {
                LightComponent& light = entityManager->lightComponents[selectedEntityID];
                ImGui::ColorEdit3("Light Color", &light.color[0]);
                ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
                ImGui::Checkbox("Is Directional", &light.isDirectional);
                if (light.isDirectional == true) {
                    ImGui::DragFloat3("Light Direction", &light.direction[0], 0.1);
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        if (ImGui::Button("Delete")) {
            remove_error_msg = entityManager->removeEntity(selectedEntityID);
            if (remove_error_msg.empty()) {
                displayInspector = false;
            }
        }
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", remove_error_msg.c_str());
        ImGui::End();
    }
}


void UI::displayNavBar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 30.0f));
    if (ImGui::BeginMainMenuBar()) // Barre de menu en haut de la fenêtre
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            ImGui::MenuItem("Open");
            ImGui::MenuItem("Save");
            ImGui::PopStyleVar(2);
            ImGui::EndMenu();

        }
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            ImGui::MenuItem("Cut");
            ImGui::MenuItem("Copy");
            ImGui::MenuItem("Paste");
            ImGui::PopStyleVar(2);
            ImGui::EndMenu();
        }
        ImGui::PopStyleVar(2);
        ImGui::EndMainMenuBar();
    }
}

void UI::displayScene(GLuint texture_id, double deltaTime) {
    ImGui::Begin("Scene");

    const float window_width = ImGui::GetContentRegionAvail().x;
    const float window_height = ImGui::GetContentRegionAvail().y;

    int cropX = (screenWidth - window_width) / 2;
    int cropY = (screenHeight - window_height) / 2;

    ImVec2 pos = ImGui::GetCursorScreenPos();

    float crop_x = (screenWidth - window_width) / (2 * screenWidth);
    float crop_y = (screenHeight - window_height) / (2 * screenHeight);

    ImVec2 uv0 = ImVec2(crop_x, 1 - crop_y);         
    ImVec2 uv1 = ImVec2(1.0f - crop_x, crop_y);

    ImGui::GetWindowDrawList()->AddImage(
        texture_id,
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + window_width, pos.y + window_height),
        uv0,
        uv1
    );
    
    //ImGui::InvisibleButton("##drop_target", ImVec2(window_width, window_height));

    if (ImGui::BeginDragDropTarget())
    {
        ImGuiDragDropFlags target_flags = 0;
        target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
        target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_TYPE"))
        {
            std::string model = (const char*)payload->Data;
            std::string basename = model;
            std::string name = basename;

            int count = 0;

            for (const auto& [key, value] : entityManager->entityNames) {
                if (value == name) {
                    count++;
                    name = basename + "_" + std::to_string(count);
                }
            }
            int newEntity = entityManager->make_entity(name);

            TransformComponent transform;
            transform.position = { 0.f , 0.f, 0.f };
            transform.eulers = { 0.f, 0.f, 0.f, 0.f };
            entityManager->transformComponents[newEntity] = transform;

            meshManager->applyRenderModel(newEntity, model);

            std::cout << "Added new object: " << basename << std::endl;
        }
        ImGui::EndDragDropTarget();
    }
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(pos.x, pos.y, window_width, window_height);

    if (gizmo_type != -1) {
        ImGuizmo::SetOrthographic(false);

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

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    // Start //ImGui window for debugging
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking);

    // Display FPS
    ImGui::Text("FPS: %0.2f", 1.0f / deltaTime);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::End();


}
void UI::displaySettings()
{
    ImGui::Begin("Settings");

    if (ImGui::BeginCombo("Current Camera", entityManager->entityNames[entityManager->cameraID].c_str())) {
        for (auto& camera : entityManager->cameraComponents) {
            bool isSelected = (camera.first == entityManager->cameraID);
            if (ImGui::Selectable(entityManager->entityNames[camera.first].c_str(), isSelected)) {
                entityManager->cameraID = camera.first;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
            ImGui::EndCombo();
        }
    }
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

        ImGui::End();
        
}
void UI::update(int screenW, int screenH, GLuint tex, double deltaTime) {
    screenWidth = screenW;
    screenHeight = screenH;

    CreateDockspace();
    if (displayInspector == false) {
        selectedEntityID = -1;
    }
    displaySceneHierarchy();
    displayEntityDetail();
    displayCatalog();
    displayNavBar();
    displayScene(tex, deltaTime);
    displaySettings();


    /*
    if (ImGui::Button("Open File Dialog")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".gltf, .obj ,.fbx", config);
    }
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            ImGui::Text(filePathName.c_str());
            ImGui::Text(filePath.c_str());
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }*/
}

// TODO: Place this function in a specific ImGui file
bool UI::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
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
