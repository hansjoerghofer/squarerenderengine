#include "Application/GL.h"
#include "Application/GLWindow.h"
#include "Application/InputHandler.h"
#include "ShaderParser/ShaderParser.h"
#include "Material/ShaderSource.h"

#include <iostream>
#include <fstream>

int main()
{
    GLApplicationScope gl;

    std::shared_ptr<GLWindow> mainWindow = std::make_shared<GLWindow>(1280, 720, "Square Renderer");
    mainWindow->enableGUI();

    InputHandler io(mainWindow);

    ShaderParser parser;

    //try
    //{
    //    auto test = parser.loadFromFile("Resources/Shaders/Test.frag");

    //    std::ofstream myfile;
    //    myfile.open("Resources/Shaders/Cache/Test.parsed.frag");
    //    myfile << test->source();
    //    myfile.close();
    //}
    //catch (const std::runtime_error& e)
    //{
    //    std::cout << "Error: " << e.what() << std::endl;
    //}

    while (mainWindow->isOpen())
    {
        io.poll();

        if (io.keyDown(Key::Escape))
        {
            mainWindow->close();
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO scene rendering

        mainWindow->renderGUI();

        mainWindow->swapBuffers();
    }

    return 0;
}
