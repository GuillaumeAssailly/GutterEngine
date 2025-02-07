#include "mesh_manager.h"


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

std::pair<unsigned int, unsigned int> MeshManager::make_model(const char* filePath) {

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
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

void MeshManager::loadGLTF(const char* filePath, const char* texDir) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        exit(-1);
    }

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
        Material mat;
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            diffuseTexturePath = std::string(texDir) + path.C_Str();
            std::cout << "Diffuse texture: " << path.C_Str() << std::endl;
            mat.baseColor = make_texture(diffuseTexturePath.c_str(), false);
        }

        if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
            normalTexturePath = std::string(texDir) + path.C_Str();
            std::cout << "Normal texture: " << std::string(texDir) + path.C_Str() << std::endl;
            mat.normalMap = make_texture(normalTexturePath.c_str(), false);
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
        std::cout << mesh->mName.C_Str() << std::endl;

        renderList[scene->mRootNode->mName.C_Str()].push_back(std::make_tuple(VAO, indices.size(), mat));
    }
}

MeshManager::MeshManager(EntityManager* em) : entityManager(em)
{
}

MeshManager::~MeshManager()
{
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
}

std::pair<unsigned int, unsigned int> MeshManager::make_cube_mesh(glm::vec3 size) {

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

unsigned int MeshManager::make_texture(const char* filename, const bool flipTex) {

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

void MeshManager::applyRenderModel(unsigned int entity, std::string model)
{
    for (auto render : renderList[model]) {
        Material mat = std::get<2>(render);
        RenderComponent renderComponent;
        renderComponent.mesh = std::get<0>(render);
        renderComponent.indexCount = std::get<1>(render);
        renderComponent.material = mat.baseColor;
        renderComponent.normalMap = mat.normalMap;
        entityManager->renderComponents[entity].push_back(renderComponent);
    }


}
