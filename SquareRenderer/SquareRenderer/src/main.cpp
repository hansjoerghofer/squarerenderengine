#include "Application/GL.h"
#include "Application/GLWindow.h"
#include "Application/InputHandler.h"

#include <iostream>

int main()
{
    GLApplicationScope gl;

    std::shared_ptr<GLWindow> mainWindow = std::make_shared<GLWindow>(800, 600, "Square Renderer");
    mainWindow->enableGUI();

    InputHandler io(mainWindow);

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
