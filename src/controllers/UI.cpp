#include "UI.h"

UI::UI(GLFWwindow* window, EntityManager* em, MeshManager* mm)
{
    entityManager = em;
    meshManager = mm;

    //Init ImGui : 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

UI::~UI()
{
    delete entityManager;
    delete meshManager;
}

void UI::displayFrameRate(double deltaTime)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    // Start //ImGui window for debugging
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    // Display FPS
    ImGui::Text("FPS: %0.3f", 1.0f / deltaTime);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void UI::displaySceneHierarchy()
{
    ImVec2 position = ImVec2(0, 0);
    ImVec2 size = ImVec2(screenWidth / 3, screenHeight);

    ImGui::SetNextWindowPos(position); // Définit la position
    ImGui::SetNextWindowSize(size); // Définit la taille
    ImGui::Begin("Entity Tree", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

    // Loop through all entities to create a tree view
    for (int entityID = 0; entityID < entityManager->entity_count; entityID++) {
        std::string entityLabel = entityManager->entityNames.at(entityID);

        // Display each entity as selectable
        if (ImGui::Selectable(entityLabel.c_str(), selectedEntityID == entityID)) {
            selectedEntityID = entityID; // Set the selected entity when clicked
            displayInspector = true;

        }
    }
    ImGui::End();
}

void UI::update(int screenW, int screenH, double deltaTime) {
    screenWidth = screenW;
    screenHeight = screenH;
    if (displayInspector == false) {
        selectedEntityID = -1;
    }
    displaySceneHierarchy();
    displayEntityDetail();
    displayFrameRate(deltaTime);

}

void UI::displayEntityDetail()
{
    if (displayInspector && ImGui::Begin("Inspector", &displayInspector, ImGuiWindowFlags_NoCollapse)) {
        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Transform"))
            {
                TransformComponent& transform = entityManager->transformComponents[selectedEntityID];
                ImGui::DragFloat3("Position", &transform.position[0]);
                ImGui::DragFloat3("Rotation", &transform.eulers[0]);
                ImGui::EndTabItem();
            }

            if (entityManager->renderComponents[selectedEntityID].size() > 0 && ImGui::BeginTabItem("Render"))
            {
                int index = 0;
                for (auto& renderComponentList : entityManager->renderComponents[selectedEntityID]) {
                    ImGui::PushID(index);
                    if (ImGui::BeginCombo("Material", std::to_string(renderComponentList.material).c_str())) {
                        for (const auto& material : meshManager->texturesList) {
                            bool isSelected = (material.second == renderComponentList.material);
                            if (ImGui::Selectable(material.first.c_str(), isSelected)) {
                                renderComponentList.material = material.second;
                            }

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
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
        ImGui::End();
    }
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

void UI::ShowStyleEditor()
{
    /*
    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();


    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {

        if (ImGui::BeginTabItem("Scene"))
        {
            int selectedID = displaySceneHierarchy();
            if (selectedEntityID < entityManager->entity_count && selectedEntityID != -1) {}
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Cameras"))
        {
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Options"))
        {
            ImGui::SeparatorText("Main");

            { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
            ImGui::SameLine();
            { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder", &border)) { style.FrameBorderSize = border ? 1.0f : 0.0f; } }
            ImGui::SameLine();
            { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder", &border)) { style.PopupBorderSize = border ? 1.0f : 0.0f; } }


            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

            ImGui::SeparatorText("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();*/
}