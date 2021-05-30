#include "Common/Logger.h"
#include "Common/Timer.h"

#include "API/GraphicsAPI.h"

#include "Application/GLWindow.h"
#include "Application/InputHandler.h"
#include "GUI/MaterialLibraryWidget.h"

#include "Preprocessor/SceneImporter.h"
#include "Preprocessor/MaterialImporter.h"
#include "Preprocessor/TextureImporter.h"

#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"
#include "Material/Material.h"
#include "Material/MaterialLibrary.h"

#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Scene/CubeGeometry.h"

#include "Texture/Texture2D.h"

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
            if (static_cast<int>(s) > static_cast<int>(LogSeverity::Info))
            {
                std::cout << "[" << Logger::severity(s) << "]: " << msg << std::endl;
            }
        }
    );

    GraphicsAPISPtr api = GraphicsAPI::create();

    GLWindowSPtr mainWindow = GLWindowSPtr(new GLWindow(1280, 720, "Square Renderer"));
    mainWindow->enableGUI();

    MaterialLibrarySPtr matLib;
    {
        ScopedTimerLog t("Material Library import");
        MaterialImporter mi(api);
        matLib = mi.importFromFile("Resources/Shaders/library.tsv");
    }

    Texture2DSPtr uvTexture;
    {
        ScopedTimerLog t("Texture import");
        TextureImporter ti(api);
        uvTexture = ti.importFromFile("Resources/Textures/uvgrid.png");
    }

    SceneSPtr scene;
    {
        ScopedTimerLog t("Scene import");
        SceneImporter si(api, matLib);
        scene = si.importFromFile("Resources/Scenes/primitives.fbx");
    }

    CameraSPtr camera = CameraSPtr(new PerspectiveCamera(
        mainWindow->width(), mainWindow->height(), 60, .1f, 10.f));
    camera->lookAt(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));

    mainWindow->inputHandler()->enableCameraNavigation(camera, 1000, 10);

    mainWindow->addWidget(std::make_shared<MaterialLibraryWidget>("Materials", matLib));

    RenderEngineUPtr renderEngine = std::make_unique<RenderEngine>(api, matLib);
    renderEngine->setupGizmos("Debug.Line");
    renderEngine->setScene(scene);
    renderEngine->setMainCamera(camera);
    renderEngine->setRenderTarget(mainWindow);

    auto matfind = matLib->materials().find("Ground");
    if (matfind != matLib->materials().end())
    {
        matfind->second->setUniform("albedo", uvTexture);
    }

    Timer frameTimer;
    while (mainWindow->isOpen())
    {
        const double deltaTime = frameTimer.elapsed();
        frameTimer.reset();

        // ------------------ input stuff -----------------------------

        mainWindow->update(deltaTime);
        
        // ------------------ update stuff -----------------------------

        // TODO update somewhere else
        camera->updateResolution(mainWindow->width(), mainWindow->height());

        renderEngine->update(deltaTime);

        // ------------------ render stuff -----------------------------

        renderEngine->render();

        mainWindow->renderGUI();

        mainWindow->swapBuffers();
    }

    return 0;
}
