#include "pxpch.h"
#include "Scene.h"

#include "Paradox/Scene/Entity.h"
#include "Paradox/Scene/Components.h"
#include "Paradox/Renderer/Renderer2D.h"
#include "Paradox/Assets/AssetManager.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define SCENE_FILE_VERSION 1

namespace Paradox
{
	Scene::Scene(const std::string& name)
		: m_Name(name), m_Registry(entt::registry())
	{}

	void Scene::Update(const glm::mat4& projection, float deltaTime)
	{
		Renderer2D::Begin(projection);

		auto view = m_Registry.view<TransformComponent, SpriteComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			SpriteComponent& sprite = view.get<SpriteComponent>(entity);

			if (sprite.texture.has_value())
				Renderer2D::DrawQuad(transform.GetTransform(), AssetManager::Get()->GetAsset<Texture2D>(sprite.texture.value()), sprite.color, sprite.tilingFactor, sprite.uv0, sprite.uv1);
			else
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
		}

		Renderer2D::End();
	}

	Entity Scene::CreateEntity(const std::string& name, const UUID& uuid)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>(name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::Serialize(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);

		std::string fileName = m_Name + ".pscn";
		std::filesystem::path filePath = path / fileName;

		std::ofstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_CORE_ERROR("Failed to open Scene file: {0}", fileName);
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(SCENE_FILE_VERSION);
		writer.Key("Entities");
		writer.StartObject();

		m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, this };
			if (!entity)
				return;

			writer.Key(entity.GetComponent<IDComponent>().id.ToString());
			writer.StartObject();

			writer.Key("Name");
			writer.String(entity.GetComponent<NameComponent>().name);

			if (entity.HasComponents<TransformComponent>())
			{
				const TransformComponent& transform = entity.GetComponent<TransformComponent>();
				writer.Key("Transform");
				writer.StartObject();
				writer.Key("Position");
				writer.StartArray();
				writer.Double(transform.position.x);
				writer.Double(transform.position.y);
				writer.Double(transform.position.z);
				writer.EndArray();

				writer.Key("Rotation");
				writer.StartArray();
				writer.Double(transform.rotation.x);
				writer.Double(transform.rotation.y);
				writer.Double(transform.rotation.z);
				writer.EndArray();

				writer.Key("Scale");
				writer.StartArray();
				writer.Double(transform.scale.x);
				writer.Double(transform.scale.y);
				writer.Double(transform.scale.z);
				writer.EndArray();
				writer.EndObject();
			}

			if (entity.HasComponents<SpriteComponent>())
			{
				const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
				writer.Key("Sprite");
				writer.StartObject();
				writer.Key("Texture");
				writer.String(sprite.texture.has_value() ? sprite.texture.value().ToString() : "None");
				writer.Key("Color");
				writer.StartArray();
				writer.Double(sprite.color.r);
				writer.Double(sprite.color.g);
				writer.Double(sprite.color.b);
				writer.Double(sprite.color.a);
				writer.EndArray();
				writer.Key("TilingFactor");
				writer.Double(sprite.tilingFactor);
				writer.Key("UV0");
				writer.StartArray();
				writer.Double(sprite.uv0.x);
				writer.Double(sprite.uv0.y);
				writer.EndArray();
				writer.Key("UV1");
				writer.StartArray();
				writer.Double(sprite.uv1.x);
				writer.Double(sprite.uv1.y);
				writer.EndArray();
				writer.EndObject();
			}

			writer.EndObject();
		});

		writer.EndObject();
		writer.EndObject();

		file << buffer.GetString();
		file.close();

		PX_CORE_INFO("Saved Scene file: {0}", filePath.string());
	}

	void Scene::Deserialize(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			PX_CORE_ERROR("Scene file does not exist: {0}", path.string());
			return;
		}

		std::ifstream file(path.string().c_str());
		if (!file.is_open())
		{
			PX_CORE_ERROR("Failed to open Scene file: {0}", path.string());
			return;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_CORE_ERROR("Failed to parse Scene file: {0}", path.string());
			return;
		}

		if (!document.HasMember("FileVersion") || !document["FileVersion"].IsInt())
		{
			PX_CORE_ERROR("Scene file is missing FileVersion: {0}", path.string());
			return;
		}

		uint8_t version = document["FileVersion"].GetInt();
		if (version != SCENE_FILE_VERSION)
		{
			PX_CORE_ERROR("Scene file version mismatch: {0} (expected {1})", version, SCENE_FILE_VERSION);
			return;
		}

		const rapidjson::Value& entities = document["Entities"];
		for (auto it = entities.MemberBegin(); it != entities.MemberEnd(); it++)
		{
			Entity entity = CreateEntity(it->value["Name"].GetString(), UUID(it->name.GetString()));

			if (it->value.HasMember("Transform"))
			{
				const rapidjson::Value& transformVal = it->value["Transform"];
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				transform.position = { transformVal["Position"][0].GetFloat(), transformVal["Position"][1].GetFloat() , transformVal["Position"][2].GetFloat() };
				transform.rotation = { transformVal["Rotation"][0].GetFloat(), transformVal["Rotation"][1].GetFloat() , transformVal["Rotation"][2].GetFloat() };
				transform.scale = { transformVal["Scale"][0].GetFloat(), transformVal["Scale"][1].GetFloat() , transformVal["Scale"][2].GetFloat() };
			}

			if (it->value.HasMember("Sprite"))
			{
				const rapidjson::Value& spriteVal = it->value["Sprite"];
				SpriteComponent& sprite = entity.AddComponent<SpriteComponent>();
				if (std::string(spriteVal["Texture"].GetString()).compare("None"))
					sprite.texture = UUID(spriteVal["Texture"].GetString());

				sprite.color = { spriteVal["Color"][0].GetFloat(), spriteVal["Color"][1].GetFloat(), spriteVal["Color"][2].GetFloat(), spriteVal["Color"][3].GetFloat() };
				sprite.tilingFactor = spriteVal["TilingFactor"].GetFloat();
				sprite.uv0 = { spriteVal["UV0"][0].GetFloat(), spriteVal["UV0"][1].GetFloat() };
				sprite.uv1 = { spriteVal["UV1"][0].GetFloat(), spriteVal["UV1"][1].GetFloat() };
			}
		}

		m_Name = path.stem().string();
		PX_CORE_INFO("Loaded Scene: {0}", m_Name);
	}
}