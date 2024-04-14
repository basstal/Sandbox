#include "pch.hpp"

#include "PhysicsSystem.hpp"

#include "Editor/ImGuiWindows/Logs.hpp"
#include "EntityComponent/Components/Transform.hpp"
#include "Misc/Ray.hpp"
#include "Misc/TypeCasting.hpp"
#include "Model.hpp"

using namespace JPH;


static void TraceImpl(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);  // 初始化args指向fmt之后的第一个可变参数
    Sandbox::Logs::Instance->AddLog(fmt, args);  // 调用AddLog
    va_end(args);  // 清理工作
}

std::shared_ptr<Sandbox::PhysicsSystem> Sandbox::PhysicsSystem::Instance = nullptr;

void Sandbox::PhysicsSystem::Prepare()
{
    Instance = shared_from_this();
    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
    // This needs to be done before any other Jolt function is called.
    RegisterDefaultAllocator();

    physicsSystem = std::make_shared<JPH::PhysicsSystem>();

    // Install trace and assert callbacks
    Trace = TraceImpl;
    // JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
    // It is not directly used in this example but still required.
    Factory::sInstance = new Factory();


    // Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
    // If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
    // If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
    RegisterTypes();

    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    TempAllocatorImpl tempAllocator(10 * 1024 * 1024);

    // We need a job system that will execute physics jobs on multiple threads. Typically
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    JobSystemThreadPool jobSystem(cMaxPhysicsJobs, cMaxPhysicsBarriers, ToInt32(thread::hardware_concurrency()) - 1);

    // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
    const uint cMaxBodies = 1024;

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
    const uint cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
    // too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
    const uint cMaxBodyPairs = 1024;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
    // number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
    // Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
    const uint cMaxContactConstraints = 1024;


    // Now we can create the actual physics system.
    physicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectVsObjectLayerFilter);


    physicsSystem->SetBodyActivationListener(&bodyActivationListener);


    physicsSystem->SetContactListener(&contactListener);
}

void Sandbox::PhysicsSystem::Cleanup()
{
    BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();
    for (auto &bodyId : registeredBodies)
    {
        bodyInterface.RemoveBody(bodyId);
        bodyInterface.DestroyBody(bodyId);
    }
    // Unregisters all types with the factory and cleans up the default material
    UnregisterTypes();

    // Destroy the factory
    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}

BodyID Sandbox::PhysicsSystem::RegisterModelToPhysicsWorld(const std::shared_ptr<Model> &inModel)
{
    size_t   pointCount = inModel->vertices.size();
    Vec3    *points     = new Vec3[pointCount];
    uint32_t i          = 0;
    for (auto &vertex : inModel->vertices)
    {
        auto &position = vertex.position;
        points[i++]    = Vec3(position.x, position.y, position.z);
    }
    // 创建凸包形状
    JPH::ConvexHullShapeSettings shapeSettings(points, ToInt32(pointCount));
    delete[] points;

    // The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
    // variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
    BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();

    JPH::Ref<JPH::Shape> shape = shapeSettings.Create().Get();

    // 创建刚体设置（静态）
    JPH::BodyCreationSettings bodySettings(shape, JPH::Vec3(0, 0, 0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);

    BodyID bodyId = bodyInterface.CreateAndAddBody(bodySettings, EActivation::DontActivate);
    registeredBodies.insert(bodyId);
    return bodyId;
}

void Sandbox::PhysicsSystem::SyncTransform(JPH::BodyID &bodyId, const std::shared_ptr<Transform> &inTransform)
{
    BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();
    auto           position      = inTransform->position;
    auto           rotation      = inTransform->rotation;
    bodyInterface.SetPositionAndRotation(bodyId, JPH::Vec3(position.x, position.y, position.z), JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
                                         EActivation::DontActivate);
}

void Sandbox::PhysicsSystem::SetActive(JPH::BodyID &bodyId, bool flag)
{
    BodyInterface &bodyInterface = physicsSystem->GetBodyInterface();
    auto           isActive      = bodyInterface.IsActive(bodyId);
    if (isActive != flag)
    {
        flag ? bodyInterface.ActivateBody(bodyId) : bodyInterface.DeactivateBody(bodyId);
    }
}


// 执行射线检测
bool Sandbox::PhysicsSystem::PerformRayCast(const Ray &raycast, JPH::AllHitCollisionCollector<JPH::CastRayCollector> &collector)
{
    auto origin    = raycast.origin;
    auto direction = raycast.direction;
    // 定义射线的起点和方向
    JPH::Vec3 rayStart(origin.x, origin.y, origin.z);
    JPH::Vec3 rayDirection(direction.x, direction.y, direction.z);  // 例如沿x轴方向

    // 射线参数
    float rayLength = 1000.0f;  // 射线长度

    // 执行射线检测
    JPH::RayCast rayCast(rayStart, rayDirection * rayLength);

    // Ray cast settings
    RayCastSettings settings;
    settings.mBackFaceMode       = EBackFaceMode::IgnoreBackFaces;
    settings.mTreatConvexAsSolid = true;
    collector.Reset();
    physicsSystem->GetNarrowPhaseQuery().CastRay(RRayCast(rayCast), settings, collector);

    // // 处理结果
    // for (const JPH::RayCastResult &hit : collector.mHits)
    // {
    //     LOGD("Engine", "Hit at: {} units along the ray.", hit.mFraction * rayLength )
    //     LOGD("Engine", "Body ID: {}", hit.mBodyID.GetIndexAndSequenceNumber())
    // }
    if (collector.mHits.size() > 0)
    {
        return true;
    }
    return false;
}