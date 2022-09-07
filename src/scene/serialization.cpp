#include <flectron/scene/scene.hpp>
#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/collisions.hpp>
#include <flectron/utils/profile.hpp>
#include <flectron/application/application.hpp>

namespace flectron
{

  using Buffer = std::vector<char>;

  enum Validation : uint8_t
  {
    SCENE_START,
    ENVIRONMENT_SKIP,
    ENVIRONMENT_START,
    ENVIRONMENT_END,
    DATETIME_SKIP,
    DATETIME_START,
    DATETIME_END,
    ENTITIES_SKIP,
    ENTITIES_START,
    ENTITY_START,
    COMPONENT_VALID,
    COMPONENT_INVALID,
    ENTITY_END,
    ENTITIES_END,
    SCENE_END,
  };

  template<typename T>
  void insert(Buffer& buffer, const T& value)
  {
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&value), reinterpret_cast<const char*>(&value) + sizeof(T));
  }

  template<typename T>
  void insert(Buffer& buffer, const std::vector<T>& values)
  {
    insert(buffer, values.size());
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(values.data()), reinterpret_cast<const char*>(values.data()) + sizeof(T) * values.size());
  }

  template<>
  void insert(Buffer& buffer, const std::string& value)
  {
    insert(buffer, value.size());
    buffer.insert(buffer.end(), value.begin(), value.end());
  }

  template<typename T>
  bool validate(Buffer& buffer, Entity entity)
  {
    if (entity.has<T>())
    {
      insert(buffer, COMPONENT_VALID);
      return true;
    }

    insert(buffer, COMPONENT_INVALID);
    return false;
  }

  void Scene::serialize(SceneAsset& to, sts::Target targets)
  {
    FLECTRON_LOG_TRACE("Serializing scene");
    FLECTRON_LOG_DEBUG("\tOutput source: {}", to.info());
    FLECTRON_ASSERT(static_cast<SceneAsset::Loader*>(to.loader.get())->isWritable(), "Scene loader is not writable");

    std::vector<char> buffer;
    insert(buffer, SCENE_START);

    if (targets & sts::Environment)
    {
      insert(buffer, ENVIRONMENT_START);
      insert(buffer, environment);
      insert(buffer, ENVIRONMENT_END);
    }
    else
      insert(buffer, ENVIRONMENT_SKIP);

    if (targets & sts::DateTime)
    {
      insert(buffer, DATETIME_START);
      insert(buffer, *dateTime);
      insert(buffer, DATETIME_END);
    }
    else
      insert(buffer, DATETIME_SKIP);

    if (targets & sts::Entities)
    {
      insert(buffer, ENTITIES_START);
      auto serializable = registry.view<UUIDComponent>();
      insert(buffer, serializable.size());
      for (auto entityID : serializable)
      {
        Entity entity(entityID, &registry);

        // FLECTRON_LOG_DEBUG("\tSerializing entity {}", entity.get<TagComponent>().tag);

        insert(buffer, ENTITY_START);
        insert(buffer, entity.get<TagComponent>().tag);
        insert(buffer, serializable.get<UUIDComponent>(entityID).uuid);

        // serialize components
        if (validate<PositionComponent>(buffer, entity))
        {
          const auto& pc = entity.get<PositionComponent>();
          insert(buffer, pc.position);
          insert(buffer, pc.rotation);
        }

        if (validate<PolygonComponent>(buffer, entity))
        {
          const auto& pc = entity.get<PolygonComponent>();
          insert(buffer, pc.vertices);
        }

        if (validate<BoxComponent>(buffer, entity))
        {
          const auto& bc = entity.get<BoxComponent>();
          insert(buffer, bc.height);
          insert(buffer, bc.width);
        }

        if (validate<CircleComponent>(buffer, entity))
        {
          const auto& cc = entity.get<CircleComponent>();
          insert(buffer, cc.fade);
          insert(buffer, cc.thickness);
          insert(buffer, cc.radius);
        }

        if (validate<PhysicsComponent>(buffer, entity))
        {
          const auto& pc = entity.get<PhysicsComponent>();
          insert(buffer, pc.linearVelocity);
          insert(buffer, pc.rotationalVelocity);

          insert(buffer, pc.force);
          insert(buffer, pc.torque);
          insert(buffer, pc.isStatic);
          
          insert(buffer, pc.density);
          insert(buffer, pc.area);

          insert(buffer, pc.mass);
          insert(buffer, pc.invMass);

          insert(buffer, pc.inertia);
          insert(buffer, pc.invInertia);

          insert(buffer, pc.resitution);

          insert(buffer, pc.staticFriction);
          insert(buffer, pc.dynamicFriction);
        }

        if (validate<FillComponent>(buffer, entity))
          insert(buffer, entity.get<FillComponent>().fillColor);

        if (validate<StrokeComponent>(buffer, entity))
        {
          const auto& sc = entity.get<StrokeComponent>();
          insert(buffer, sc.strokeWidth);
          insert(buffer, sc.strokeColor);
        }

        if (validate<TextureComponent>(buffer, entity))
        {
          const auto& tc = entity.get<TextureComponent>();
          insert(buffer, tc.texturePositions);
        }

        if (validate<TextureVertexComponent>(buffer, entity))
        {
          const auto& tvc = entity.get<TextureVertexComponent>();
          insert(buffer, tvc.textureOffset);
          insert(buffer, tvc.textureHalfSize);
          insert(buffer, tvc.textureVertices);
          insert(buffer, tvc.isTextureUpdateRequired);
        }

        if (validate<AnimationComponent>(buffer, entity))
        {
          const auto& ac = entity.get<AnimationComponent>();
          insert(buffer, ac.animationState.currentName);
          insert(buffer, ac.animationState.currentRange);
          insert(buffer, ac.animationState.currentFrame);
          insert(buffer, ac.animationState.elapsedTime);
        }

        if (validate<LightComponent>(buffer, entity))
        {
          const auto& lc = entity.get<LightComponent>();
          insert(buffer, lc.lightColor);
          insert(buffer, lc.lightRadius);
        }

        validate<TemporaryComponent>(buffer, entity);

        if (validate<ScriptComponent>(buffer, entity))
        {
          const auto& sc = entity.get<ScriptComponent>();
          insert(buffer, sc.order);
        }

        if (entity.has<SerializationComponent>())
          entity.get<SerializationComponent>().serialize(buffer);
        
        insert(buffer, ENTITY_END);
      }
      insert(buffer, ENTITIES_END);
    }
    else
      insert(buffer, ENTITIES_SKIP);

    insert(buffer, SCENE_END);

    static_cast<SceneAsset::Loader*>(to.loader.get())->save(buffer.data(), buffer.size());
  }

  template<typename T>
  T get(char* data, ptrdiff_t& offset)
  {
    T value = *reinterpret_cast<T*>(data + offset);
    offset += sizeof(T);
    return value;
  }

  template<>
  std::string get<std::string>(char* data, ptrdiff_t& offset)
  {
    auto length = get<size_t>(data, offset);
    std::string value(data + offset, length);
    offset += length;
    return value;
  }

  // template<typename T, typename std::enable_if<std::is_class<std::vector<T>>{}, bool>::type = true>
  template<typename T>
  std::vector<T> getVector(char* data, ptrdiff_t& offset)
  {
    auto length = get<size_t>(data, offset);
    std::vector<T> value;
    value.insert(value.begin(), reinterpret_cast<T*>(data + offset), reinterpret_cast<T*>(data + offset) + length);
    offset += length * sizeof(T);
    return value;
  }

  bool isValid(char* data, ptrdiff_t& offset)
  {
    return get<uint8_t>(data, offset) == COMPONENT_VALID;
  }

  template<typename T, typename ... Args>
  T& addOrUpdate(Entity entity, Args&& ... args)
  {
    if (entity.has<T>())
      return entity.get<T>();
    else
      return entity.add<T>(std::forward<Args>(args)...);
  }

  void Scene::deserialize(const SceneAsset& from, std::function<void(Entity)> onEntityCreation)
  {
    FLECTRON_LOG_TRACE("Deserializing scene");
    FLECTRON_LOG_DEBUG("\tInput source: {}", from.info());
    FLECTRON_ASSERT(from.isLoaded(), "Scene asset is not loaded");

    ptrdiff_t offset = 0;
    Validation stage;

    stage = get<Validation>(from.data, offset);
    FLECTRON_ASSERT(stage == SCENE_START, "Invalid scene data (SCENE_START)");

    stage = get<Validation>(from.data, offset);
    if (stage == ENVIRONMENT_START)
    {
      environment = get<Environment>(from.data, offset);

      stage = get<Validation>(from.data, offset);
      FLECTRON_ASSERT(stage == ENVIRONMENT_END, "Invalid scene data (ENVIRONMENT_END)");
    }
    else
      FLECTRON_ASSERT(stage == ENVIRONMENT_SKIP, "Invalid scene data (ENVIRONMENT_SKIP)");

    stage = get<Validation>(from.data, offset);
    if (stage == DATETIME_START)
    {
      dateTime = createScope<DateTime>(get<DateTime>(from.data, offset));

      stage = get<Validation>(from.data, offset);
      FLECTRON_ASSERT(stage == DATETIME_END, "Invalid scene data (DATETIME_END)");
    }
    else
      FLECTRON_ASSERT(stage == DATETIME_SKIP, "Invalid scene data (DATETIME_SKIP)");

    // deserialize entities
    stage = get<Validation>(from.data, offset);
    if (stage == ENTITIES_START)
    {
      auto entityCount = get<size_t>(from.data, offset);
      for (size_t i = 0; i < entityCount; ++i)
      {
        stage = get<Validation>(from.data, offset);
        FLECTRON_ASSERT(stage == ENTITY_START, "Invalid scene data (ENTITY_START)");
        auto entity = createEntity();

        entity.get<TagComponent>().tag = get<std::string>(from.data, offset);
        entity.add<UUIDComponent>().uuid = get<uint64_t>(from.data, offset);

        // FLECTRON_LOG_DEBUG("\tDeserializing entity: {}", entity.get<TagComponent>().tag);
        onEntityCreation(entity);

        if (isValid(from.data, offset))
        {
          auto& pc = entity.get<PositionComponent>();
          pc.position = get<Vector>(from.data, offset);
          pc.rotation = get<float>(from.data, offset);
        }

        if (isValid(from.data, offset))
          entity.add<PolygonComponent>(getVector<Vector>(from.data, offset));

        if (isValid(from.data, offset))
          entity.add<BoxComponent>(get<float>(from.data, offset), get<float>(from.data, offset));

        if (isValid(from.data, offset))
          entity.add<CircleComponent>(get<float>(from.data, offset), get<float>(from.data, offset), get<float>(from.data, offset));

        if (isValid(from.data, offset))
        {
          auto& pc = entity.add<PhysicsComponent>();
          pc.linearVelocity = get<Vector>(from.data, offset);
          pc.rotationalVelocity = get<float>(from.data, offset);

          pc.force = get<Vector>(from.data, offset);
          pc.torque = get<float>(from.data, offset);
          pc.isStatic = get<bool>(from.data, offset);

          pc.density = get<float>(from.data, offset);
          pc.area = get<float>(from.data, offset);

          pc.mass = get<float>(from.data, offset);
          pc.invMass = get<float>(from.data, offset);

          pc.inertia = get<float>(from.data, offset);
          pc.invInertia = get<float>(from.data, offset);

          pc.resitution = get<float>(from.data, offset);

          pc.staticFriction = get<float>(from.data, offset);
          pc.dynamicFriction = get<float>(from.data, offset);
        }

        if (isValid(from.data, offset))
          entity.add<FillComponent>(get<Color>(from.data, offset));

        if (isValid(from.data, offset))
          entity.add<StrokeComponent>(get<Color>(from.data, offset), get<float>(from.data, offset));

        if (isValid(from.data, offset))
        {
          FLECTRON_ASSERT(entity.has<TextureComponent>(), "Texture component must be defined before deserializing texture data");
          entity.get<TextureComponent>().texturePositions = get<glm::vec4>(from.data, offset);
        }

        if (isValid(from.data, offset))
        {
          auto& tvc = entity.add<TextureVertexComponent>(Vector(0.0f, 0.0f), Vector(0.0f, 0.0f));
          tvc.textureOffset = get<Vector>(from.data, offset);
          tvc.textureHalfSize = get<Vector>(from.data, offset);
          tvc.textureVertices = get<std::array<Vector, 4>>(from.data, offset);
          tvc.isTextureUpdateRequired = get<bool>(from.data, offset);
        }

        if (isValid(from.data, offset))
        {
          FLECTRON_ASSERT(entity.has<AnimationComponent>(), "Animation component must be defined before deserializing animation data");
          auto& ac = entity.get<AnimationComponent>();
          ac.animationState.currentName = get<std::string>(from.data, offset);
          ac.animationState.currentRange = get<size_t>(from.data, offset);
          ac.animationState.currentFrame = get<size_t>(from.data, offset);
          ac.animationState.elapsedTime = get<float>(from.data, offset);
        }

        if (isValid(from.data, offset))
          entity.add<LightComponent>(get<float>(from.data, offset), get<Color>(from.data, offset));

        if (isValid(from.data, offset))
          entity.add<TemporaryComponent>();

        if (isValid(from.data, offset))
        {
          FLECTRON_ASSERT(entity.has<ScriptComponent>(), "Script component must be defined before deserializing script data");
          entity.get<ScriptComponent>().order = get<int>(from.data, offset);
        }

        if (entity.has<DeserializationComponent>())
          entity.get<DeserializationComponent>().deserialize();

        stage = get<Validation>(from.data, offset);
        FLECTRON_ASSERT(stage == ENTITY_END, "Invalid scene data (ENTITY_END)");
      }
      stage = get<Validation>(from.data, offset);
      FLECTRON_ASSERT(stage == ENTITIES_END, "Invalid scene data (ENTITIES_END)");
    }
    else
      FLECTRON_ASSERT(stage == ENTITIES_SKIP, "Invalid scene data (ENTITIES_SKIP)");

    stage = get<Validation>(from.data, offset);
    FLECTRON_ASSERT(stage == SCENE_END, "Invalid scene data (SCENE_END)");
  }

}