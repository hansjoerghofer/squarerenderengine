#include "Common/Logger.h"
#include "Common/Timer.h"

#include "Application/GL.h"
#include "Application/GLWindow.h"
#include "Application/InputHandler.h"

#include "Preprocessor/ShaderParser.h"

#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"

#include "Scene/Scene.h"

#include "Renderer/RenderEngine.h"
#include "Renderer/PerspectiveCamera.h"

#include "Resources/GLAllocator.h"

#include <iostream>
#include <fstream>

int main()
{
    // register console logger
    auto logcout = Logger::getInstance().registerCallback(
        [](LogSeverity s, const std::string& msg) {
            std::cout << "[" << Logger::severity(s) << "]: " << msg << std::endl;
        }
    );

    GLApplicationScope gl;

    const int width = 1280;
    const int height = 720;

    GLWindowSPtr mainWindow = GLWindowSPtr(
        new GLWindow(width, height, "Square Renderer"));
    mainWindow->enableGUI();

    InputHandler io(mainWindow);

    ShaderParser parser;
    GLAllocator alloc;
    try
    {
        ScopedTimerLog t("load shaders");

        const std::string pathVert("Resources/Shaders/Test.vert");
        auto testVert = parser.loadFromFile(pathVert);

        const std::string pathFrag("Resources/Shaders/Test.frag");
        auto testFrag = parser.loadFromFile(pathFrag);

        auto program = std::make_shared<ShaderProgram>("Test");
        program->addShaderSource(testVert);
        program->addShaderSource(testFrag);

        auto result = alloc.link(*program);
        if (!result)
        {
            Logger::Error("Error while linking shader program: '%s'\n%s", 
                program->name().c_str(), result.message.c_str());
        }

        /*std::ofstream myfile;
        myfile.open("Resources/Shaders/Cache/Test.parsed.frag");
        myfile << test->source();
        myfile.close();*/

        /*auto result = alloc.compile(*test);
        if (!result)
        {
            std::cout << "Error while compiling " << path << std::endl;
            std::cout << result.message << std::endl;
        }*/
    }
    catch (const std::runtime_error& e)
    {
        Logger::Error(e.what());
    }

    SceneSPtr scene = SceneSPtr(new Scene());
    RenderEngineUPtr renderEngine = RenderEngineUPtr(new RenderEngine(scene));

    CameraSPtr camera = CameraSPtr(
        new PerspectiveCamera(width, height, 60, .01f, 100.f));

    while (mainWindow->isOpen())
    {
        io.poll();

        if (io.keyDown(Key::Escape))
        {
            mainWindow->close();
        }
        else if (io.keyDown(Key::Space))
        {
            Logger::Debug("Space pressed!");
        }

        camera->updateResolution(mainWindow->width(), mainWindow->height());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderEngine->render(*camera);

        mainWindow->renderGUI();

        mainWindow->swapBuffers();
    }

    return 0;
}
