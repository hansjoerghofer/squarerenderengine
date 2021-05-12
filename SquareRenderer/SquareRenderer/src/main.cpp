#include "Common/Logger.h"
#include "Common/Timer.h"

#include "API/GraphicsAPI.h"
#include "Application/GLWindow.h"
#include "Application/InputHandler.h"

#include "Preprocessor/ShaderParser.h"

#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"
#include "Material/Material.h"

#include "Scene/Scene.h"
#include "Scene/SceneElement.h"
#include "Scene/CubeGeometry.h"
#include "Scene/Mesh.h"

#include "Renderer/RenderEngine.h"
#include "Renderer/PerspectiveCamera.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>

ShaderProgramSPtr loadProgram(GraphicsAPISPtr api, ShaderParser& parser, const std::string& name, std::vector<std::string> shaderPaths)
{
    ShaderProgramSPtr program = std::make_shared<ShaderProgram>(name);
    for (const std::string& path : shaderPaths)
    {
        program->addShaderSource(parser.loadFromFile(path));
    }

    auto result = api->compile(*program);
    if (!result)
    {
        Logger::Error("Error while linking shader program: '%s'\n%s",
            program->name().c_str(), result.message.c_str());

        return nullptr;
    }
    return program;
}

int main()
{
    // register console logger
    auto logcout = Logger::getInstance().registerCallback(
        [](LogSeverity s, const std::string& msg) {
            std::cout << "[" << Logger::severity(s) << "]: " << msg << std::endl;
        }
    );

    GraphicsAPISPtr api = std::make_shared<GraphicsAPI>();

    GLWindowSPtr mainWindow = GLWindowSPtr(
        new GLWindow(1280, 720, "Square Renderer"));
    mainWindow->enableGUI();

    ShaderProgramSPtr blinnPhongProgram;
    ShaderProgramSPtr lineProgram;
    GeometrySPtr cubeGeo;
    GeometrySPtr coordinatePointSet;
    try
    {
        ShaderParser parser;
        ScopedTimerLog t("load shaders");

        blinnPhongProgram = loadProgram(api, parser, "BlinnPhong", { "Resources/Shaders/Lit/BlinnPhong.vert", "Resources/Shaders/Lit/BlinnPhong.frag" });
        lineProgram = loadProgram(api, parser, "Line", { "Resources/Shaders/Line.vert", "Resources/Shaders/Line.frag" });

        coordinatePointSet = GeometrySPtr(new Mesh({ 
            { {0,0,0}, {0,0}, {1,0,0} }, // X start
            { {1,0,0}, {0,0}, {1,0,0} }, // X end
            { {0,0,0}, {0,0}, {0,1,0} }, // Y start
            { {0,1,0}, {0,0}, {0,1,0} }, // Y end
            { {0,0,0}, {0,0}, {0,0,1} }, // Z start
            { {0,0,1}, {0,0}, {0,0,1} }  // Z end
            }, {}, true, true, false));
        if (!api->allocate(*coordinatePointSet))
        {
            Logger::Error("Error while allocating geometry buffers.");
        }

        cubeGeo = GeometrySPtr(new CubeGeometry());
        if (!api->allocate(*cubeGeo))
        {
            Logger::Error("Error while allocating geometry buffers.");
        }
    }
    catch (const std::runtime_error& e)
    {
        Logger::Error(e.what());

        return -1;
    }

    SceneElementSPtr elem = SceneElementSPtr(new SceneElement("Cube"));
    elem->setGeometry(cubeGeo);
    elem->setMaterial(MaterialSPtr(new Material("BlinnPhong", blinnPhongProgram)));

    elem->material()->setUniform("albedoColor", glm::vec4(0.6, 0.6, 0.6, 1));
    elem->material()->setUniform("shininessFactor", 0.8f);

    SceneSPtr scene = SceneSPtr(new Scene());
    scene->addSceneElement(elem);

    RenderEngineUPtr renderEngine = RenderEngineUPtr(new RenderEngine(api));
    renderEngine->setScene(scene);

    CameraSPtr camera = CameraSPtr(new PerspectiveCamera(
        mainWindow->width(), mainWindow->height(), 60, .1f, 10.f));
    camera->lookAt(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));

    Timer frameTimer;
    while (mainWindow->isOpen())
    {
        const double deltaTime = frameTimer.elapsed();
        frameTimer.reset();

        // ------------------ input stuff -----------------------------

        mainWindow->inputHandler().poll();
        if (mainWindow->inputHandler().keyDown(Key::Escape))
        {
            mainWindow->close();
        }
        else if (mainWindow->inputHandler().keyDown(Key::Space))
        {
            Logger::Debug("Space pressed!");
        }

        // ------------------ update stuff -----------------------------

        elem->setTransform(glm::rotate(static_cast<float>(deltaTime), 
            glm::vec3(0, 1, 0)) * elem->transform());

        // TODO update somewhere else
        camera->updateResolution(mainWindow->width(), mainWindow->height());
        elem->material()->setUniform("M", elem->transform());
        elem->material()->setUniform("N", glm::transpose(glm::inverse(elem->transform())));

        // ------------------ render stuff -----------------------------

        renderEngine->render(*camera);

        renderEngine->renderLines(*camera, *coordinatePointSet, *lineProgram);

        mainWindow->renderGUI();

        mainWindow->swapBuffers();
    }

    return 0;
}
