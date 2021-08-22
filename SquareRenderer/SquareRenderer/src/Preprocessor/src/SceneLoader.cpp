#include "Common/Logger.h"
#include "Common/Math3D.h"

#include "Preprocessor/SceneLoader.h"
#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/Mesh.h"
#include "Material/MaterialLibrary.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

inline void Map(const aiMatrix4x4& source, glm::mat4& target)
{
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    target[0][0] = source.a1; target[1][0] = source.a2; target[2][0] = source.a3; target[3][0] = source.a4;
    target[0][1] = source.b1; target[1][1] = source.b2; target[2][1] = source.b3; target[3][1] = source.b4;
    target[0][2] = source.c1; target[1][2] = source.c2; target[2][2] = source.c3; target[3][2] = source.c4;
    target[0][3] = source.d1; target[1][3] = source.d2; target[2][3] = source.d3; target[3][3] = source.d4;
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

inline void Map(const aiQuaternion& source, glm::quat& target)
{
    target.w = source.w;
    target.x = source.x;
    target.y = source.y;
    target.z = source.z;
}

MaterialSPtr SceneLoader::processMaterial(const aiMaterial& mat)
{
    aiString name;
    mat.Get(AI_MATKEY_NAME, name);

    Logger::Info("Process Material '%s'", name.C_Str());

    return m_matLib->instanciate(m_defaultProgramName, std::string(name.C_Str()));
}

void LogMatrix(const std::string& description, const glm::mat4& m)
{
    Logger::Debug("Matrix: %s\n"
        "%.2f,\t\t%.2f,\t\t%.2f,\t\t%.2f\n"
        "%.2f,\t\t%.2f,\t\t%.2f,\t\t%.2f\n"
        "%.2f,\t\t%.2f,\t\t%.2f,\t\t%.2f\n"
        "%.2f,\t\t%.2f,\t\t%.2f,\t\t%.2f\n",
        description.c_str(),
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
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

    unsigned char dataFieldFlags = 0;
    dataFieldFlags |= mesh.HasTextureCoords(0) ? Vertex::DATA_UV : 0;
    dataFieldFlags |= mesh.HasNormals() ? Vertex::DATA_NORMAL : 0;
    dataFieldFlags |= mesh.HasTangentsAndBitangents() ? Vertex::DATA_TANGENT : 0;

    std::vector<Vertex> vertices(mesh.mNumVertices);

    for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
    {
        Vertex& vertex = vertices[i];
        
        Map(mesh.mVertices[i], vertex.position);

        if (dataFieldFlags & Vertex::DATA_UV != 0)
        {
            Map(mesh.mTextureCoords[0][i], vertex.uv);
        }
        if (dataFieldFlags & Vertex::DATA_NORMAL != 0)
        {
            Map(mesh.mNormals[i], vertex.normal);
        }
        if (dataFieldFlags & Vertex::DATA_TANGENT != 0)
        {
            Map(mesh.mTangents[i], vertex.tangent);
        }
    }

    const size_t faceCount = mesh.mNumFaces;
    constexpr size_t vertPerTri = 3;
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

    return std::make_shared<Mesh>(std::move(vertices), std::move(indices), dataFieldFlags);
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

    //LogMatrix(newNode->name(), newNode->localTransform());

    if (currentNode.mNumMeshes > 0)
    {
        const auto& mesh = meshSet[currentNode.mMeshes[0]];

        newNode->setGeometry(mesh.first);
        newNode->setMaterial(materialSet[mesh.second]);

        BoundingBox aabb;
        for (const Vertex& v : mesh.first->vertices())
        {
            aabb.insert(v.position);
        }
        newNode->setBounds(aabb);
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
        //| aiProcess_PreTransformVertices        //! Apply transformations
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
        //rootNode->setLocalTransform(glm::scale(rootNode->localTransform(), glm::vec3(0.1f)));
        //rootNode->setLocalTransform(glm::scale(rootNode->localTransform(), glm::vec3(0.001f)));

        scene = std::make_unique<Scene>(rootNode);
    }
    
	return scene;
}
