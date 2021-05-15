#include "Common/Logger.h"
#include "Common/Timer.h"

#include "API/GraphicsAPI.h"
#include "Application/GLWindow.h"
#include "Application/InputHandler.h"

#include "Preprocessor/SceneImporter.h"
#include "Preprocessor/MaterialImporter.h"

#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"

#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/CubeGeometry.h"

#include "Renderer/RenderEngine.h"
#include "Renderer/PerspectiveCamera.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

int main()
{
    // register console logger
    auto logcout = Logger::getInstance().registerCallback(
        [](LogSeverity s, const std::string& msg) {
            std::cout << "[" << Logger::severity(s) << "]: " << msg << std::endl;
        }
    );

    GraphicsAPISPtr api = GraphicsAPI::create();

    GLWindowSPtr mainWindow = GLWindowSPtr(new GLWindow(1280, 720, "Square Renderer"));
    mainWindow->enableGUI();

    MaterialLibrarySPtr matLib;
    try
    {
        ScopedTimerLog t("Material Library import");
        MaterialImporter matImporter(api);
        matLib = matImporter.importFromFile("Resources/Shaders/library.config");
    }
    catch (const std::runtime_error& e)
    {
        Logger::Error("Material import: %s", e.what());
    }

    /*GeometrySPtr cubeGeo = GeometrySPtr(new CubeGeometry());
    if (!api->allocate(*cubeGeo))
    {
        Logger::Error("Error while allocating geometry buffers.");
    }

    SceneElementSPtr elem = SceneElementSPtr(new SceneElement("Cube"));
    elem->setGeometry(cubeGeo);
    elem->setMaterial(matLib->instanciate("Lit.BlinnPhong"));

    elem->material()->setUniform("albedoColor", glm::vec4(0.6, 0.6, 0.6, 1));
    elem->material()->setUniform("shininessFactor", 0.3f);

    SceneSPtr scene = SceneSPtr(new Scene());
    scene->addSceneElement(elem);*/

    SceneSPtr scene;
    {
        ScopedTimerLog t("Scene import");
        SceneImporter si(api, matLib);
        scene = si.importFromFile("Resources/Scenes/primitives.fbx");
    }

    CameraSPtr camera = CameraSPtr(new PerspectiveCamera(
        mainWindow->width(), mainWindow->height(), 60, .1f, 10.f));
    camera->lookAt(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));

    RenderEngineUPtr renderEngine = std::make_unique<RenderEngine>(api, matLib);
    renderEngine->setupGizmos("Debug.Line");
    renderEngine->setScene(scene);
    renderEngine->setMainCamera(camera);

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

        // TODO update somewhere else
        camera->updateResolution(mainWindow->width(), mainWindow->height());

        

        // TODO how to update these just before rendering!
        // material instance could be shared!
        auto t = scene->traverser();
        while (t.hasNext())
        {
            SceneNodeSPtr node = t.next();
            if (node->isDrawable())
            {
                node->material()->setUniform("M", node->worldTransform());
                node->material()->setUniform("N", 
                    glm::transpose(glm::inverse(node->worldTransform())));
            }
        }

        // ------------------ render stuff -----------------------------

        renderEngine->render();

        mainWindow->renderGUI();

        mainWindow->swapBuffers();
    }

    return 0;
}
