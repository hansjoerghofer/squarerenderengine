#include "Preprocessor/SceneLoader.h"
#include "Common/Logger.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/Mesh.h"
#include "Material/MaterialLibrary.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

inline void Map(const aiMatrix4x4& source, glm::mat4& target)
{
    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            target[r][c] = source[r][c];
        }
    }
}

inline void Map(const aiVector3D& source, glm::vec2& target)
{
    target.x = source.x;
    target.y = source.y;
}

inline void Map(const aiVector3D& source, glm::vec3& target)
{
    target.x = source.x;
    target.y = source.y;
    target.z = source.z;
}

inline void Map(const aiColor3D& source, glm::vec3& target)
{
    target.r = source.r;
    target.g = source.g;
    target.b = source.b;
}

MaterialSPtr SceneLoader::processMaterial(const aiMaterial& mat)
{
    aiString name;
    mat.Get(AI_MATKEY_NAME, name);

    Logger::Info("Process Material '%s'", name.C_Str());

    return m_matLib->instanciate(m_defaultProgramName, std::string(name.C_Str()));
}

MeshSPtr SceneLoader::processMesh(const aiMesh& mesh)
{
    if ((mesh.mPrimitiveTypes & aiPrimitiveType_TRIANGLE) == 0
        || mesh.mNumFaces == 0
        || mesh.mNumVertices == 0
        || !mesh.HasNormals()
        || !mesh.HasPositions()
        || !mesh.HasTextureCoords(0)
        || !mesh.HasTangentsAndBitangents())
    {
        Logger::Warning("Mesh is incomplete: %s", mesh.mName.C_Str());

        return MeshSPtr();
    }

    // TODO use reserve when emplacing
    std::vector<Vertex> vertices(mesh.mNumVertices);

    for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
    {
        // TODO use emplace
        Vertex vertex = Vertex();
        
        Map(mesh.mVertices[i], vertex.position);
        Map(mesh.mNormals[i], vertex.normal);
        Map(mesh.mTangents[i], vertex.tangent);
        Map(mesh.mTextureCoords[0][i], vertex.uv);

        vertices[i] = std::move(vertex);
    }

    const unsigned int faceCount = mesh.mNumFaces;
    const unsigned int vertPerTri = 3;
    std::vector<uint32_t> indices(faceCount * vertPerTri);

    for (unsigned int i = 0; i < faceCount; ++i)
    {
        const aiFace& face = mesh.mFaces[i];

        // TODO Maybe only needed in debug mode
        if (face.mNumIndices != vertPerTri)
        {
            Logger::Warning("Triangle with more than 3 vertices!");
        }

        for (unsigned int k = 0; k < vertPerTri; k++)
        {
            // TODO use emplace
            indices[vertPerTri * i + k] = face.mIndices[k];
        }
    }

    return MeshSPtr(new Mesh(std::move(vertices), std::move(indices)));
}

SceneNodeSPtr SceneLoader::processNode(
    const aiNode& currentNode,
    const std::vector<MaterialSPtr>& materialSet,
    const std::vector<std::pair<MeshSPtr, unsigned int>>& meshSet)
{
    if (currentNode.mNumMeshes == 0 
        && currentNode.mChildren == 0)
    {
        return nullptr;
    }

    SceneNodeSPtr newNode = std::make_shared<SceneNode>(currentNode.mName.C_Str());
    
    Logger::Debug("Import node: %s", newNode->name().c_str());

    glm::mat4 localTransform(1);
    Map(currentNode.mTransformation, localTransform);
    newNode->setLocalTransform(localTransform);

    if (currentNode.mNumMeshes > 0)
    {
        const auto& mesh = meshSet[currentNode.mMeshes[0]];

        newNode->setGeometry(mesh.first);
        newNode->setMaterial(materialSet[mesh.second]);
    }

    for (unsigned int i = 0; i < currentNode.mNumChildren; ++i)
    {
        SceneNodeSPtr childNode = processNode(*currentNode.mChildren[i],
            materialSet, meshSet);

        if (childNode)
        {
            newNode->addChild(childNode);
            childNode->setParent(newNode);
        }
    }

    return newNode;
}

SceneLoader::SceneLoader(MaterialLibrarySPtr matLib)
    : m_matLib(matLib)
{
}

SceneLoader::~SceneLoader()
{
}

void SceneLoader::setDefaultProgramName(const std::string& defaultProgramName)
{
    m_defaultProgramName = defaultProgramName;
}

SceneUPtr SceneLoader::loadFromFile(const std::string& filepath)
{
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, 0
        | aiComponent_TEXTURES
        | aiComponent_COLORS
        | aiComponent_TANGENTS_AND_BITANGENTS
        | aiComponent_BONEWEIGHTS
        | aiComponent_ANIMATIONS
        | aiComponent_CAMERAS 
        | aiComponent_LIGHTS);

    const aiScene* aiScene = importer.ReadFile(filepath, 0u
        | aiProcess_CalcTangentSpace
        | aiProcess_JoinIdenticalVertices
        | aiProcess_GenSmoothNormals
        | aiProcess_GenBoundingBoxes
        | aiProcess_PreTransformVertices        //! Flatten hierachy
        | aiProcess_SortByPType                 //! Split up heterogeneous mesh types
        | aiProcess_RemoveComponent
        //| aiProcess_RemoveRedundantMaterials
        | aiProcess_Triangulate
        //| aiProcess_ImproveCacheLocality
        //| aiProcess_OptimizeMeshes
        //| aiProcess_FixInfacingNormals
        //| aiProcess_FlipWindingOrder
    );

    // process materials
    std::vector<MaterialSPtr> materialSet;
    materialSet.reserve(aiScene->mNumMaterials);
    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        materialSet.push_back(processMaterial(*aiScene->mMaterials[i]));
    }

    // process meshes
    std::vector<std::pair<MeshSPtr, unsigned int>> meshSet;
    meshSet.reserve(aiScene->mNumMeshes);
    for (unsigned int i = 0; i < aiScene->mNumMeshes; ++i)
    {
        aiMesh& mesh = *aiScene->mMeshes[i];
        meshSet.emplace_back(processMesh(mesh), mesh.mMaterialIndex);
    }

    const aiNode* importNode = aiScene->mRootNode;

    SceneNodeSPtr rootNode = processNode(*importNode,
        materialSet, meshSet);

    SceneUPtr scene;
    if (rootNode)
    {
        // scale root
        rootNode->setLocalTransform(glm::scale(rootNode->localTransform(), glm::vec3(0.001f)));

        scene = std::make_unique<Scene>(rootNode);
    }
    
	return scene;
}
