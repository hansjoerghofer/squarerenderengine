#include "Common/Logger.h"
#include "Common/Timer.h"
#include "Common/Math3D.h"

#include "API/GraphicsAPI.h"

#include "Application/GLWindow.h"
#include "Application/InputHandler.h"

#include "GUI/MaterialLibraryWidget.h"
#include "GUI/StatisticsWidget.h"

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
#include "Scene/DirectionalLight.h"
#include "Scene/PointLight.h"

#include "Texture/Texture2D.h"

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
        //scene = si.importFromFile("Resources/Scenes/primitives_smooth.fbx");
        //scene = si.importFromFile("F:/SquaredEngine/Resources/Assets/Camera/Camera.FBX");
        scene = si.importFromFile("Resources/Scenes/primitives_more.fbx");
    }

    DirectionalLightSPtr directionalLight = std::make_shared<DirectionalLight>(glm::vec3(1, -1, -0.5), glm::vec3(0.2, 0.7, 0.1));
    scene->addLight(directionalLight);
    //PointLightSPtr pointLight = std::make_shared<PointLight>(glm::vec3(.5, .5, .25), glm::vec3(0.2, 0.2, 0.7));
    //scene->addLight(pointLight);

    CameraSPtr camera = CameraSPtr(new PerspectiveCamera(
        mainWindow->width(), mainWindow->height(), 60, .1f, 10.f));
    camera->lookAt(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));

    mainWindow->inputHandler()->enableCameraNavigation(camera, 1000, 10);

    RenderEngineSPtr renderEngine = std::make_shared<RenderEngine>(api, matLib);
    
    //renderEngine->setRenderingScale(1.0 / mainWindow->dpiScaling());

    renderEngine->setScene(scene);
    renderEngine->setMainCamera(camera);
    
    renderEngine->setupGizmos("Debug.Line");
    renderEngine->setRenderTarget(mainWindow);

    mainWindow->addWidget(std::make_shared<MaterialLibraryWidget>("Materials", matLib));
    mainWindow->addWidget(std::make_shared<StatisticsWidget>("Stats", mainWindow, renderEngine));

    // TESTSETUP remove
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
