#include "Scene/MeshBuilder.h"
#include "Scene/Mesh.h"
#include "Scene/PrimitiveSet.h"

IGeometryUPtr MeshBuilder::cube()
{
    std::vector<Vertex> vertices = {
        // front
        { glm::vec3(-.5,-.5, .5), glm::vec2(0,1),  glm::vec3(0, 0, 1) },
        { glm::vec3(.5,-.5, .5), glm::vec2(1,1),  glm::vec3(0, 0, 1) },
        { glm::vec3(.5, .5, .5), glm::vec2(1,0),  glm::vec3(0, 0, 1) },
        { glm::vec3(-.5, .5, .5), glm::vec2(0,0),  glm::vec3(0, 0, 1) },
        // back
        { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -glm::vec3(0, 0, 1) },
        { glm::vec3(.5,-.5,-.5), glm::vec2(1,1), -glm::vec3(0, 0, 1) },
        { glm::vec3(.5, .5,-.5), glm::vec2(1,0), -glm::vec3(0, 0, 1) },
        { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), -glm::vec3(0, 0, 1) },
        // right
        { glm::vec3(.5,-.5, .5), glm::vec2(0,1),  glm::vec3(1, 0, 0) },
        { glm::vec3(.5,-.5,-.5), glm::vec2(1,1),  glm::vec3(1, 0, 0) },
        { glm::vec3(.5, .5,-.5), glm::vec2(1,0),  glm::vec3(1, 0, 0) },
        { glm::vec3(.5, .5, .5), glm::vec2(0,0),  glm::vec3(1, 0, 0) },
        // left
        { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -glm::vec3(1, 0, 0) },
        { glm::vec3(-.5,-.5, .5), glm::vec2(1,1), -glm::vec3(1, 0, 0) },
        { glm::vec3(-.5, .5, .5), glm::vec2(1,0), -glm::vec3(1, 0, 0) },
        { glm::vec3(-.5, .5,-.5), glm::vec2(0,0), -glm::vec3(1, 0, 0) },
        // bottom
        { glm::vec3(-.5,-.5,-.5), glm::vec2(0,1), -glm::vec3(0, 1, 0) },
        { glm::vec3(.5,-.5,-.5), glm::vec2(1,1), -glm::vec3(0, 1, 0) },
        { glm::vec3(.5,-.5, .5), glm::vec2(1,0), -glm::vec3(0, 1, 0) },
        { glm::vec3(-.5,-.5, .5), glm::vec2(0,0), -glm::vec3(0, 1, 0) },
        // top
        { glm::vec3(-.5, .5, .5), glm::vec2(0,1),  glm::vec3(0, 1, 0) },
        { glm::vec3(.5, .5, .5), glm::vec2(1,1),  glm::vec3(0, 1, 0) },
        { glm::vec3(.5, .5,-.5), glm::vec2(1,0),  glm::vec3(0, 1, 0) },
        { glm::vec3(-.5, .5,-.5), glm::vec2(0,0),  glm::vec3(0, 1, 0) }
    };

    std::vector<uint32_t> indices = {
        // front
        0, 1, 2,
        2, 3, 0,
        // glm::vec3(1, 0, 0)
        8, 9, 10,
        10, 11, 8,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        12, 13, 14,
        14, 15, 12,
        // bottom
        16, 17, 18,
        18, 19, 16,
        // top
        20, 21, 22,
        22, 23, 20
    };

	return std::make_unique<Mesh>(std::move(vertices), std::move(indices), Vertex::DATA_UV | Vertex::DATA_NORMAL);
}

IGeometryUPtr MeshBuilder::screenTriangle()
{
    std::vector<Vertex> vertices = {
            {{-1.f,-1.f, 0.f}},
            {{ 3.f,-1.f, 0.f}},
            {{-1.f, 3.f, 0.f}}
    };

    return std::make_unique<PrimitiveSet>(PrimitiveType::Triangles, std::move(vertices));
}

IGeometryUPtr MeshBuilder::skybox()
{
    std::vector<Vertex> vertices = {
        { glm::vec3(-1.0f,  1.0f, -1.0f) },
        { glm::vec3(-1.0f, -1.0f, -1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f) },
        { glm::vec3(1.0f,  1.0f, -1.0f) },
        { glm::vec3(-1.0f,  1.0f, -1.0f) },

        { glm::vec3(-1.0f, -1.0f,  1.0f) },
        { glm::vec3(-1.0f, -1.0f, -1.0f) },
        { glm::vec3(-1.0f,  1.0f, -1.0f) },
        { glm::vec3(-1.0f,  1.0f, -1.0f) },
        { glm::vec3(-1.0f,  1.0f,  1.0f) },
        { glm::vec3(-1.0f, -1.0f,  1.0f) },

        { glm::vec3(1.0f, -1.0f, -1.0f) },
        { glm::vec3(1.0f, -1.0f,  1.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f) },
        { glm::vec3(1.0f,  1.0f, -1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f) },

        { glm::vec3(-1.0f, -1.0f,  1.0f) },
        { glm::vec3(-1.0f,  1.0f,  1.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f) },
        { glm::vec3(1.0f, -1.0f,  1.0f) },
        { glm::vec3(-1.0f, -1.0f,  1.0f) },

        { glm::vec3(-1.0f,  1.0f, -1.0f) },
        { glm::vec3(1.0f,  1.0f, -1.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f) },
        { glm::vec3(-1.0f,  1.0f,  1.0f) },
        { glm::vec3(-1.0f,  1.0f, -1.0f) },

        { glm::vec3(-1.0f, -1.0f, -1.0f) },
        { glm::vec3(-1.0f, -1.0f,  1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f) },
        { glm::vec3(-1.0f, -1.0f,  1.0f) },
        { glm::vec3(1.0f, -1.0f,  1.0f) }
    };

    return std::make_unique<PrimitiveSet>(PrimitiveType::Triangles, std::move(vertices));
}
