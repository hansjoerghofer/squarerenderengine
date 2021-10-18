#include "Common/Logger.h"
#include "Common/Timer.h"
#include "Common/Math3D.h"

#include "API/GraphicsAPI.h"

#include "Application/GLWindow.h"
#include "Application/InputHandler.h"

#include "GUI/MaterialLibraryWidget.h"
#include "GUI/StatisticsWidget.h"
#include "GUI/RenderPassListWidget.h"

#include "Preprocessor/SceneImporter.h"
#include "Preprocessor/MaterialImporter.h"
#include "Preprocessor/TextureImporter.h"

#include "Material/MaterialLibrary.h"

#include "Scene/Scene.h"

#include "Renderer/RenderEngine.h"
#include "Renderer/PerspectiveCamera.h"

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
        matLib = mi.importFromFile("materials.yaml");// Resources / Shaders / library.tsv");
    }

    RenderEngineSPtr renderEngine = std::make_shared<RenderEngine>(api, matLib);

    SceneSPtr scene;
    {
        ScopedTimerLog t("Scene import");
        SceneImporter si(api, matLib, renderEngine);
        //scene = si.importFromFile("scene_camera.yaml");
        scene = si.importFromFile("scene_lantern.yaml");
        //scene = si.importFromFile("scene_ukulele.yaml");
    }

    CameraSPtr camera = CameraSPtr(new PerspectiveCamera(
        mainWindow->width(), mainWindow->height(), 60, .1f, 10.f));
    camera->lookAt(glm::vec3(1, 1, -1.5), glm::vec3(0, 0, 0));

    mainWindow->inputHandler()->enableCameraNavigation(camera, 1000, 10);

    //renderEngine->setRenderingScale(1.0 / mainWindow->dpiScaling());

    renderEngine->setScene(scene);
    renderEngine->setMainCamera(camera);
    
    renderEngine->setupGizmos("Debug.Line");
    renderEngine->setRenderTarget(mainWindow);

    mainWindow->addWidget(std::make_shared<MaterialLibraryWidget>("Materials", matLib));
    mainWindow->addWidget(std::make_shared<StatisticsWidget>("Stats", mainWindow, renderEngine));
    mainWindow->addWidget(std::make_shared<RenderPassListWidget>("Render Passes", renderEngine));

    Timer frameTimer;
    while (mainWindow->isOpen())
    {
        const double deltaTime = frameTimer.elapsed();
        frameTimer.reset();
                
        // ------------------ update stuff -----------------------------

        mainWindow->update(deltaTime);

        renderEngine->update(deltaTime);

        // ------------------ render stuff -----------------------------

        renderEngine->render();

        mainWindow->render();

        mainWindow->swapBuffers();
    }

    return 0;
}
