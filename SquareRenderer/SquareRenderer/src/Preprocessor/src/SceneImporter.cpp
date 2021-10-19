#include "Preprocessor/SceneImporter.h"
#include "Preprocessor/ModelLoader.h"
#include "Preprocessor/TextureImporter.h"
#include "Common/Logger.h"
#include "API/GraphicsAPI.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/IGeometry.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"
#include "Scene/DirectionalLight.h"
#include "Scene/PointLight.h"
#include "Texture/Cubemap.h"
#include "Texture/Texture2D.h"
#include "Renderer/RenderEngine.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>

#undef ASYNC_TEXTURE_IMPORT

namespace YAML 
{
	template<>
	struct convert<glm::vec3> 
	{
		static Node encode(const glm::vec3& rhs) 
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3) 
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<TextureWrap>
	{
		static Node encode(const TextureWrap& rhs)
		{
			Node node;
			switch (rhs)
			{
			case TextureWrap::Repeat: 
				node.push_back("Repeat"); 
				break;
			case TextureWrap::Mirror: 
				node.push_back("Mirror");
				break;
			case TextureWrap::ClampToBorder: 
				node.push_back("ClampToBorder");
				break;
			case TextureWrap::ClampToEdge: 
				node.push_back("ClampToEdge");
				break;
			}

			return node;
		}

		static bool decode(const Node& node, TextureWrap& rhs)
		{
			if (!node.IsScalar())
			{
				return false;
			}

			const std::string valueStr = node.as<std::string>("");
			if (valueStr.empty())
			{
				return false;
			}

			if (valueStr == "Mirror")
			{
				rhs = TextureWrap::Mirror;
			}
			else if (valueStr == "ClampToEdge")
			{
				rhs = TextureWrap::ClampToEdge;
			}
			else if (valueStr == "ClampToBorder")
			{
				rhs = TextureWrap::ClampToBorder;
			}
			else
			{
				rhs = TextureWrap::Repeat;
			}
			
			return true;
		}
	};

	template<>
	struct convert<TextureFilter>
	{
		static Node encode(const TextureFilter& rhs)
		{
			Node node;
			switch (rhs)
			{
			case TextureFilter::Linear:
				node.push_back("Linear");
				break;
			case TextureFilter::Nearest:
				node.push_back("Nearest");
				break;
			}

			return node;
		}

		static bool decode(const Node& node, TextureFilter& rhs)
		{
			if (!node.IsScalar())
			{
				return false;
			}

			const std::string valueStr = node.as<std::string>("");
			if (valueStr.empty())
			{
				return false;
			}

			if (valueStr == "Nearest")
			{
				rhs = TextureFilter::Nearest;
			}
			else
			{
				rhs = TextureFilter::Linear;
			}

			return true;
		}
	};

	template<>
	struct convert<TextureImporter::ImportFormat>
	{
		static Node encode(const TextureImporter::ImportFormat& rhs)
		{
			Node node;
			switch (rhs)
			{
			case TextureImporter::ImportFormat::Auto:
				node.push_back("Auto");
				break;
			case TextureImporter::ImportFormat::R:
				node.push_back("R");
				break;
			case TextureImporter::ImportFormat::RG:
				node.push_back("RG");
				break;
			case TextureImporter::ImportFormat::RGB:
				node.push_back("RGB");
				break;
			case TextureImporter::ImportFormat::RGBA:
				node.push_back("RGBA");
				break;
			case TextureImporter::ImportFormat::sRGB:
				node.push_back("sRGB");
				break;
			case TextureImporter::ImportFormat::sRGBA:
				node.push_back("sRGBA");
				break;
			}

			return node;
		}

		static bool decode(const Node& node, TextureImporter::ImportFormat& rhs)
		{
			if (!node.IsScalar())
			{
				return false;
			}

			const std::string valueStr = node.as<std::string>("");
			if (valueStr.empty())
			{
				return false;
			}

			if (valueStr == "R")
			{
				rhs = TextureImporter::ImportFormat::R;
			}
			else if (valueStr == "RG")
			{
				rhs = TextureImporter::ImportFormat::RG;
			}
			else if (valueStr == "RGB")
			{
				rhs = TextureImporter::ImportFormat::RGB;
			}
			else if (valueStr == "RGBA")
			{
				rhs = TextureImporter::ImportFormat::RGBA;
			}
			else if (valueStr == "sRGB")
			{
				rhs = TextureImporter::ImportFormat::sRGB;
			}
			else if (valueStr == "sRGBA")
			{
				rhs = TextureImporter::ImportFormat::sRGBA;
			}
			else
			{
				rhs = TextureImporter::ImportFormat::Auto;
			}

			return true;
		}
	};
}

SceneImporter::SceneImporter(GraphicsAPISPtr api, MaterialLibrarySPtr matLib, RenderEngineSPtr renderEngine)
	: m_api(api)
	, m_matLib(matLib)
	, m_renderEngine(renderEngine)
{
}

SceneImporter::~SceneImporter()
{
}

SceneUPtr SceneImporter::importFromFile(const std::string& filepath)
{
	const std::filesystem::path path(filepath);

	if (!std::filesystem::exists(path) || !path.has_filename())
	{
		throw std::runtime_error("Invalid path to scene file.");
	}

	SceneNodeSPtr sceneRoot = std::make_shared<SceneNode>("SceneRoot");
	SceneUPtr scene = std::make_unique<Scene>(sceneRoot);

	ModelLoader loader(m_matLib);
	loader.setDefaultProgramName("Lit.PBR");

	YAML::Node config = YAML::LoadFile(filepath);

	for (const auto& model : config["models"])
	{
		const std::string modelPath = model["filepath"].as<std::string>();
		const glm::vec3 position = model["position"].as<glm::vec3>(glm::vec3(0,0,0));
		const glm::vec3 rotation = model["rotation"].as<glm::vec3>(glm::vec3(0, 0, 0));
		const float scale = model["scale"].as<float>(1.f);

		SceneNodeSPtr modelRootNode = loader.loadFromFile(modelPath);
		if (modelRootNode)
		{
			glm::mat4 transform = modelRootNode->localTransform();
			
			glm::mat4 R = glm::eulerAngleYXZ(
				glm::radians(rotation.y), 
				glm::radians(rotation.x), 
				glm::radians(rotation.z));
			glm::mat4 S = glm::translate(glm::mat4(1), position);
			glm::mat4 T = glm::scale(glm::mat4(1), glm::vec3(scale));

			modelRootNode->setLocalTransform(S * T * R * transform);

			sceneRoot->addChild(modelRootNode);
		}
	}

	for (const auto& light : config["lights"])
	{
		const std::string type = light["type"].as<std::string>();
		const glm::vec3 color = light["color"].as<glm::vec3>(glm::vec3(1,1,1));
		const float intensity = light["intensity"].as<float>(1.f);
		const bool isShadowCaster = light["isShadowCaster"].as<bool>(false);

		ILightsourceSPtr lightsource;

		if (type == "Directional" && light["direction"])
		{
			const glm::vec3 direction = light["direction"].as<glm::vec3>();

			lightsource = std::make_shared<DirectionalLight>(
				direction, color, intensity, isShadowCaster);
		}
		else if (type == "Point" && light["position"])
		{
			const glm::vec3 position = light["position"].as<glm::vec3>();

			lightsource = std::make_shared<PointLight>(
				position, color, intensity);
		}

		if (lightsource)
		{
			scene->addLight(lightsource);
		}
	}

	TextureImporter ti(m_api);

	Texture2DSPtr skyHDRI;
	if (config["sky"])
	{
		const auto& sky = config["sky"];
		const std::string hdriPath = sky["filepath"].as<std::string>();
		//const float rotation = sky["rotation"].as<float>(0);

		skyHDRI = ti.importFromFileAsync(hdriPath);
	}

	if (config["overrides"])
	{
		for (const auto& materialOverride : config["overrides"]["material"])
		{
			const std::string name = materialOverride["name"].as<std::string>();
			MaterialSPtr material = m_matLib->findMaterial(name);
			if (!material)
			{
				continue;
			}

			if (materialOverride["program"])
			{
				const std::string programName = materialOverride["program"].as<std::string>();
				ShaderProgramSPtr program = m_matLib->findProgram(programName);
				if (program)
				{
					material->setProgram(program);
				}
			}

			for (const auto& parameter : materialOverride["parameters"])
			{
				const std::string uniform = parameter["uniform"].as<std::string>();
				const std::string type = parameter["type"].as<std::string>();

				if (type == "Texture2D")
				{
					typedef TextureImporter::ImportFormat ImportFormat;

					const auto& value					= parameter["value"];
					const std::string textureParamPath	= value["path"].as<std::string>();
					const ImportFormat format			= value["format"].as<ImportFormat>(ImportFormat::Auto);

					TextureSampler sampler;
					sampler.filter		= value["filter"].as<TextureFilter>(sampler.filter);
					sampler.wrap		= value["wrap"].as<TextureWrap>(sampler.wrap);
					sampler.mipmapping  = value["mipmapping"].as<bool>(sampler.mipmapping);
					sampler.borderColor = value["bordercolor"].as<glm::vec4>(sampler.borderColor);

					Texture2DSPtr texture = ti.importFromFileAsync(textureParamPath, format, sampler);
					if (texture)
					{
						material->setUniform(uniform, texture);
					}
				}
				else if (type == "PackedTexture2D")
				{
					typedef TextureImporter::ImportFormat ImportFormat;

					const auto& value = parameter["value"];

					std::vector<std::string> channelTexturePaths{
						value["r"].as<std::string>(),
						value["g"].as<std::string>(),
						value["b"].as<std::string>(),
						value["a"].as<std::string>()
					};

					TextureSampler sourceSampler;
					sourceSampler.mipmapping = false;

					bool waitForImport = false;
					std::vector< Texture2DSPtr> channelTextures(channelTexturePaths.size());
					for (size_t i = 0; i < channelTexturePaths.size(); ++i)
					{
						const std::string& channelTexturePath = channelTexturePaths[i];
						ITextureSPtr texture = m_matLib->findDefaultTexture(channelTexturePath);
						if (texture && texture->layout() == TextureLayout::Texture2D)
						{
							channelTextures[i] = std::dynamic_pointer_cast<Texture2D>(texture);
						}
						else
						{
							channelTextures[i] = ti.importFromFileAsync(channelTexturePath, ImportFormat::R, sourceSampler);
							waitForImport = true;
						}
					}

					if (waitForImport)
					{
						ti.waitForCompletion();
					}

					TextureSampler targetSampler;
					targetSampler.filter = value["filter"].as<TextureFilter>(targetSampler.filter);
					targetSampler.wrap = value["wrap"].as<TextureWrap>(targetSampler.wrap);
					targetSampler.mipmapping = value["mipmapping"].as<bool>(targetSampler.mipmapping);
					targetSampler.borderColor = value["bordercolor"].as<glm::vec4>(targetSampler.borderColor);

					const TextureFormat targetFormat = channelTextures[0]->format() == TextureFormat::R ? TextureFormat::RGBA : TextureFormat::RGBAFloat;
					Texture2DSPtr texture = std::make_shared<Texture2D>(
						channelTextures[0]->width(), 
						channelTextures[0]->height(), 
						targetFormat, 
						targetSampler);

					m_renderEngine->packTextures(texture, channelTextures[0], channelTextures[1], channelTextures[2], channelTextures[3]);
					if (texture)
					{
						material->setUniform(uniform, texture);
					}
				}
				else if (type == "Float")
				{
					const float value = parameter["value"].as<float>();
					material->setUniform(uniform, value);
				}
				else if (type == "Vector4D")
				{
					const glm::vec4 value = parameter["value"].as<glm::vec4>();
					material->setUniform(uniform, value);
				}
			}
		}
	}

	auto t = scene->traverser();
	while (t.hasNext())
	{
		SceneNodeSPtr node = t.next();
		IGeometrySPtr geo = node->geometry() ? 
			std::dynamic_pointer_cast<IGeometry>(node->geometry()) : nullptr;

		if (geo && !m_api->allocate(geo))
		{
			Logger::Warning("Could not allocate geometry buffers.");
		}
	}

	ti.waitForCompletion();

	if (skyHDRI)
	{
		// convert equirectangular HDRI to cubemap 
		TextureSampler skySampler = { TextureFilter::Linear, TextureWrap::ClampToEdge, true, glm::vec4(0,0,0,0) };
		CubemapSPtr skybox = std::make_shared<Cubemap>(skyHDRI->height() / 2, TextureFormat::RGBHalf, skySampler);
		m_renderEngine->projectEquirectangularToCubemap(skyHDRI, skybox);
		scene->setSky(skybox);
	}
	
	return scene;
}
