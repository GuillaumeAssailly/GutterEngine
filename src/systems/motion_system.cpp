#include "motion_system.h"

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
    // Nettoyage des ressources
    if (mScene) {
        mScene->release();
    }
    if (mCpuDispatcher) {
        mCpuDispatcher->release();
    }
    if (mPhysics) {
        mPhysics->release(); // Libération de l'objet PhysX
    }
}

physx::PxRigidDynamic* MotionSystem::createDynamic(const physx::PxGeometry& geometry, glm::vec3 mat, glm::vec3 transf, float mass, float sleepT, float linearDamp, float angularDamp) {
    physx::PxMaterial* material = mPhysics->createMaterial(mat.x, mat.y, mat.z);
    physx::PxTransform transform = { transf.x, transf.y, transf.z };
    physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(transform);
    body->setSleepThreshold(sleepT);
    physx::PxShape* shape = mPhysics->createShape(geometry, *material);
    body->attachShape(*shape);
    body->setLinearDamping(linearDamp);  // Amortissement linéaire modéré
    body->setAngularDamping(angularDamp); // Amortissement angulaire modéré
    physx::PxRigidBodyExt::updateMassAndInertia(*body, mass);
    shape->release();
    mScene->addActor(*body);
    return body;
}


void MotionSystem::createStatic(const physx::PxGeometry& geometry, glm::vec3 mat, glm::vec3 transf) {
    physx::PxMaterial* material = mPhysics->createMaterial(mat.x, mat.y, mat.z);
    physx::PxTransform transform = { transf.x, transf.y, transf.z };
    physx::PxRigidStatic* body = mPhysics->createRigidStatic(transform);
    physx::PxShape* shape = mPhysics->createShape(geometry, *material);
    body->attachShape(*shape);
    shape->release();
    mScene->addActor(*body);
}

void printVerticesData(const std::vector<physx::PxVec3>& vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::cout << "Vertex " << i << ": ("
            << vertices[i].x << ", "
            << vertices[i].y << ", "
            << vertices[i].z << ")"
            << std::endl;
    }
}
void printConvexMeshVertices(physx::PxConvexMesh* convexMesh) {
    if (!convexMesh) {
        std::cerr << "Convex mesh is null!" << std::endl;
        return;
    }

    // Obtenez le nombre de sommets dans le maillage convexe
    physx::PxU32 numVertices = convexMesh->getNbVertices();
    const physx::PxVec3* vertices = convexMesh->getVertices();

    std::cout << "Nombre de sommets dans le maillage convexe : " << numVertices << std::endl;

    // Parcourez et affichez chaque sommet
    for (physx::PxU32 i = 0; i < numVertices; ++i) {
        std::cout << "Vertex " << i << ": ("
            << vertices[i].x << ", "
            << vertices[i].y << ", "
            << vertices[i].z << ")"
            << std::endl;
    }
}

physx::PxConvexMesh* MotionSystem::createMesh(std::vector<physx::PxVec3> vertices) {

    printVerticesData(vertices);

    physx::PxConvexMeshDesc convexMeshDesc;
    convexMeshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
    printf("VERTICES : %d\n\n", vertices.size());
    convexMeshDesc.points.stride = sizeof(physx::PxVec3);
    convexMeshDesc.points.data = vertices.data(); // Utiliser data() pour obtenir un pointeur
    convexMeshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

    physx::PxDefaultMemoryOutputStream outputStream;
    physx::PxConvexMeshCookingResult::Enum result;
    
    physx::PxTolerancesScale scale;
    physx::PxCookingParams params(scale);
    params.convexMeshCookingType = physx::PxConvexMeshCookingType::eQUICKHULL; // Utiliser l'algorithme QuickHull
    params.gaussMapLimit = 256;
    params.buildGPUData = false;
    params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH; // Désactive le nettoyage de maillage
    params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

    if (!PxCookConvexMesh(params, convexMeshDesc, outputStream, &result))
        return NULL;

    physx::PxDefaultMemoryInputData inputStream(outputStream.getData(), outputStream.getSize());
    physx::PxConvexMesh* convexMesh = mPhysics->createConvexMesh(inputStream);


    if (!convexMesh) {
        // Gérer l'erreur de création du maillage
        std::cerr << "Erreur lors de la création du maillage convexe." << std::endl;
        return nullptr;
    }

    printConvexMeshVertices(convexMesh);

    return convexMesh;
}

void MotionSystem::applyForceToActor(physx::PxRigidDynamic* actor, const physx::PxVec3& force, const physx::PxForceMode::Enum mode) {
    if (actor) {
        // Applique une force dans la direction spécifiée
        actor->addForce(force, mode);
    }
}

void MotionSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
    float dt) {

    printf("dt : %f\n", dt);
    if (dt > 0.1)
        return;
    mScene->simulate(dt);
    mScene->fetchResults(true);

    for (auto& entity : physicsComponents) {
        PhysicsComponent& physicsComponent = entity.second;
        TransformComponent& transform = transformComponents[entity.first];

        physx::PxTransform pxTransform = physicsComponent.rigidBody->getGlobalPose();

        transform.position = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
        printf("Pos : %f, %f, %f\n", pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
        transform.eulers = glm::eulerAngles(glm::quat(pxTransform.q.w, pxTransform.q.x, pxTransform.q.y, pxTransform.q.z));
    }
    printf("\n");
}