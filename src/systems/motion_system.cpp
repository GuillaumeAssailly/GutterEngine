#include "motion_system.h"
#define ENABLE_VHACD_IMPLEMENTATION 1
#include "VHACD.h"

MotionSystem::MotionSystem() {
    static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;

    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
        gDefaultErrorCallback);
    if (!mFoundation) {
        std::cerr << "PxCreateFoundation failed!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    bool recordMemoryAllocations = true;

    mPvd = physx::PxCreatePvd(*mFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);


    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
        physx::PxTolerancesScale(), recordMemoryAllocations, mPvd);
    if (!mPhysics) {
        std::cerr << "PxCreatePhysics failed!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(mNbThreads);
    if (!mCpuDispatcher) {
        std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    sceneDesc.cpuDispatcher = mCpuDispatcher;
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    mScene = mPhysics->createScene(sceneDesc);

}

MotionSystem::~MotionSystem() {
    if (mScene) {
        mScene->release();
    }
    if (mCpuDispatcher) {
        mCpuDispatcher->release();
    }
    if (mPhysics) {
        mPhysics->release();
    }
}

void saveConvexHullToObj(VHACD::IVHACD* interfaceVHACD, const std::string& outputDir, std::string baseName) {
    std::string filePath = outputDir + baseName + ".obj";
    std::ofstream outFile(filePath);

    if (!outFile.is_open()) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << filePath << " pour écrire." << std::endl;
        return;
    }

    std::cout << "Saving Convex Decomposition results of " << interfaceVHACD->GetNConvexHulls() << " convex hulls to '" << filePath << "'\n";
    uint32_t baseIndex = 1;

    for (uint32_t i = 0; i < interfaceVHACD->GetNConvexHulls(); i++) {
        outFile << "o " << baseName << std::setw(3) << std::setfill('0') << i << "\n";

        VHACD::IVHACD::ConvexHull ch;
        interfaceVHACD->GetConvexHull(i, ch);

        for (const auto& pos : ch.m_points) {
            outFile << "v " << std::fixed << std::setprecision(9)
                << pos.mX << " " << pos.mY << " " << pos.mZ << "\n";
        }

        for (const auto& tri : ch.m_triangles) {
            uint32_t i1 = tri.mI0 + baseIndex;
            uint32_t i2 = tri.mI1 + baseIndex;
            uint32_t i3 = tri.mI2 + baseIndex;
            outFile << "f " << i1 << " " << i2 << " " << i3 << "\n";
        }

        baseIndex += static_cast<uint32_t>(ch.m_points.size());
    }

    outFile.close();
}

void MotionSystem::concaveToConvex(const char* filePath, std::string outputDir, std::string baseName) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasMeshes()) {
        printf("Failed to load valid mesh from file: %s\n", filePath);
        return;
    }

    const aiMesh* mesh = scene->mMeshes[0]; // On suppose qu'il y a au moins un maillage
    uint32_t vertexCount = mesh->mNumVertices;
    uint32_t triangleCount = mesh->mNumFaces;

    VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
    if (!interfaceVHACD) {
        std::cerr << "Erreur : impossible de créer une instance de VHACD." << std::endl;
        exit(1);
    }

    VHACD::IVHACD::Parameters params;
    params.m_maxConvexHulls = 2;                          // Limite raisonnable pour obtenir des coques sans excès
    params.m_resolution = 1000000;                        // Bonne résolution pour un équilibre entre vitesse et précision
    params.m_minimumVolumePercentErrorAllowed = 1.0;      // Tolérance modérée pour une approximation correcte
    params.m_maxRecursionDepth = 10;                      // Profondeur de récursion modérée
    params.m_shrinkWrap = true;                           // Réduction pour coller au modèle sans perte excessive
    params.m_fillMode = VHACD::FillMode::SURFACE_ONLY;    // Remplissage complet
    params.m_maxNumVerticesPerCH = 256;                   // Nombre modéré de sommets pour chaque coque
    params.m_asyncACD = false;                            // Mode synchrone pour éviter les problèmes de threading
    params.m_minEdgeLength = 2;                           // Longueur d’arête permettant des détails raisonnables
    params.m_findBestPlane = false;                       // Plan standard pour un traitement plus rapide


    double* points = new double[vertexCount * 3];
    for (uint32_t i = 0; i < vertexCount; ++i) {
        points[i * 3] = mesh->mVertices[i].x;
        points[i * 3 + 1] = mesh->mVertices[i].y;
        points[i * 3 + 2] = mesh->mVertices[i].z;
    }

    uint32_t* indices = new uint32_t[triangleCount * 3];
    for (uint32_t i = 0; i < triangleCount; ++i) {
        const aiFace& face = mesh->mFaces[i];
        indices[i * 3] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    interfaceVHACD->Compute(points, vertexCount, indices, triangleCount, params);
    saveConvexHullToObj(interfaceVHACD, outputDir, baseName);

    interfaceVHACD->Clean();
    interfaceVHACD->Release();
}

physx::PxRigidDynamic* MotionSystem::createDynamic(const std::vector<physx::PxConvexMesh*>& convexMeshes, glm::vec3 mat, glm::vec3 transf, float mass, float sleepT, float linearDamp, float angularDamp, int solverPosition, int solverVelocity) {
    physx::PxMaterial* material = mPhysics->createMaterial(mat.x, mat.y, mat.z);
    physx::PxTransform transform = { transf.x, transf.y, transf.z };
    physx::PxRigidDynamic* actor = mPhysics->createRigidDynamic(transform);

    for (auto& convexMesh : convexMeshes) {
        physx::PxConvexMeshGeometry geometry(convexMesh);
        physx::PxShape* shape = mPhysics->createShape(geometry, *material);
        actor->attachShape(*shape);
        shape->release();
    }

    actor->setSleepThreshold(sleepT);
    actor->setLinearDamping(linearDamp);
    actor->setAngularDamping(angularDamp);

    actor->setSolverIterationCounts(solverPosition, solverVelocity);

    physx::PxRigidBodyExt::setMassAndUpdateInertia(*actor, mass);
    mScene->addActor(*actor);

    /*physx::PxTransform centerOfMassPose = actor->getCMassLocalPose();
    physx::PxVec3 offset(0.f, 0.f, 0.f);
    physx::PxTransform centerOfMassOffset(offset);
    actor->setCMassLocalPose(centerOfMassOffset);*/

    return actor;
}

physx::PxRigidDynamic* MotionSystem::createDynamic(const physx::PxGeometry& geometry, glm::vec3 mat, glm::vec3 transf, float mass, float sleepT, float linearDamp, float angularDamp, int solverPosition, int solverVelocity) {
    physx::PxMaterial* material = mPhysics->createMaterial(mat.x, mat.y, mat.z);
    physx::PxTransform transform = { transf.x, transf.y, transf.z };
    physx::PxRigidDynamic* actor = mPhysics->createRigidDynamic(transform);

    physx::PxShape* shape = mPhysics->createShape(geometry, *material);
    actor->attachShape(*shape);
    shape->release();

    actor->setSleepThreshold(sleepT);
    actor->setLinearDamping(linearDamp);
    actor->setAngularDamping(angularDamp);

    actor->setSolverIterationCounts(solverPosition, solverVelocity);

    physx::PxRigidBodyExt::setMassAndUpdateInertia(*actor, mass);

    mScene->addActor(*actor);

    /*physx::PxTransform centerOfMassPose = actor->getCMassLocalPose();
    physx::PxVec3 offset(0.f, 0.f, 0.f);
    physx::PxTransform centerOfMassOffset(offset);
    actor->setCMassLocalPose(centerOfMassOffset);*/

    return actor;
}

physx::PxRigidStatic* MotionSystem::createStatic(const physx::PxGeometry& geometry, glm::vec3 mat, glm::vec3 transf) {
    physx::PxMaterial* material = mPhysics->createMaterial(mat.x, mat.y, mat.z);
    physx::PxTransform transform = { transf.x, transf.y, transf.z };
    physx::PxRigidStatic* actor = mPhysics->createRigidStatic(transform);

    physx::PxShape* shape = mPhysics->createShape(geometry, *material);
    actor->attachShape(*shape);
    shape->release();
    mScene->addActor(*actor);
    return actor;
}

physx::PxRigidStatic* MotionSystem::createStatic(const std::vector<physx::PxConvexMesh*>& convexMeshes, glm::vec3 mat, glm::vec3 transf) {
    physx::PxMaterial* material = mPhysics->createMaterial(mat.x, mat.y, mat.z);
    physx::PxTransform transform = { transf.x, transf.y, transf.z };
    physx::PxRigidStatic* actor = mPhysics->createRigidStatic(transform);

    for (auto& convexMesh : convexMeshes) {
        physx::PxConvexMeshGeometry geometry(convexMesh);
        physx::PxShape* shape = mPhysics->createShape(geometry, *material);
        actor->attachShape(*shape);
        shape->release();
    }
    mScene->addActor(*actor);
    return actor;
}

void MotionSystem::loadObjToPhysX(const std::string& filePath, std::vector<physx::PxConvexMesh*>& convexMeshes) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        std::cerr << "Erreur lors du chargement du fichier .obj avec Assimp : " << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

        std::vector<physx::PxVec3> vertices;
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            vertices.emplace_back(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
        }

        std::vector<physx::PxU32> indices;
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            aiFace face = mesh->mFaces[f];
            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
        }

        physx::PxConvexMeshDesc convexDesc;
        convexDesc.points.count = static_cast<physx::PxU32>(vertices.size());
        convexDesc.points.stride = sizeof(physx::PxVec3);
        convexDesc.points.data = vertices.data();

        convexDesc.indices.count = static_cast<physx::PxU32>(indices.size());
        convexDesc.indices.stride = sizeof(physx::PxU32);
        convexDesc.indices.data = indices.data();

        convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

        physx::PxDefaultMemoryOutputStream buf;
        physx::PxConvexMeshCookingResult::Enum result;

        physx::PxTolerancesScale scale;
        physx::PxCookingParams params(scale);
        params.convexMeshCookingType = physx::PxConvexMeshCookingType::eQUICKHULL;
        params.gaussMapLimit = 256;
        params.buildGPUData = false;
        params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
        params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

        if (!PxCookConvexMesh(params, convexDesc, buf, &result))
            return;

        physx::PxDefaultMemoryInputData inputStream(buf.getData(), buf.getSize());
        physx::PxConvexMesh* convexMesh = mPhysics->createConvexMesh(inputStream);


        if (!convexMesh) {
            std::cerr << "Erreur lors de la création du maillage convexe." << std::endl;
            return;
        }
        convexMeshes.push_back(convexMesh);
    }
}

void MotionSystem::applyForceToActor(physx::PxRigidDynamic* actor, const physx::PxVec3& force, const physx::PxForceMode::Enum mode) {
    if (actor) {
        actor->addForce(force, mode);
    }
}

void MotionSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
    float dt) {
    if (dt > 0.1)
        return;
    mScene->simulate(dt);
    mScene->fetchResults(true);

    for (auto& entity : physicsComponents) {
        PhysicsComponent& physicsComponent = entity.second;
        TransformComponent& transform = transformComponents[entity.first];

        physx::PxTransform pxTransform = physicsComponent.rigidBody->getGlobalPose();

        transform.position = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
        transform.eulers = glm::eulerAngles(glm::quat(pxTransform.q.w, pxTransform.q.x, pxTransform.q.y, pxTransform.q.z));
    }
}