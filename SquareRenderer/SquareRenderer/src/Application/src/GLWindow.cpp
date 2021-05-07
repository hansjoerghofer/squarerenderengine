#include "Application/GLWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/LogWidget.h"

#include <stdexcept>

GLWindow::GLWindow(int width, int height, const std::string& title)
    : m_width(width)
    , m_height(height)
    , m_title(title)
{
    if (m_width <= 0 || m_height <= 0)
    {
        throw std::invalid_argument("Window dimensions must be greater than 0.");
    }

    // If it's a high DPI monitor, try to scale everything
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor)
    {
        float xscale, yscale;
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);
        if (xscale > 1)
        {
            m_dpiScaling = xscale;
            glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
        }
    }

    // Create window
    m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_handle)
    {
        throw std::runtime_error("Failed to create GLFW window.");
    }

    if (!makeContextCurrent() || gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        // cleanup window
        glfwDestroyWindow(m_handle);

        throw std::runtime_error("Failed to switch to window context.");
    }

    glViewport(0, 0, m_width, m_height);
    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, onResizeCallback);
}

GLWindow::~GLWindow()
{
    if (m_handle)
    {
        cleanupGUI();

        glfwDestroyWindow(m_handle);
    }
}

void GLWindow::close()
{
    if (m_handle)
    {
        glfwSetWindowShouldClose(m_handle, true);
    }
}

void GLWindow::renderGUI()
{
    if (!m_guiInitialized)
    {
        return;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    onGUI();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GLWindow::swapBuffers()
{
    if (m_handle)
    {
        glfwSwapBuffers(m_handle);
    }
}

bool GLWindow::makeContextCurrent()
{
    if (m_handle)
    {
        glfwMakeContextCurrent(m_handle);

        return glfwGetError(nullptr) == 0;
    }

    return false;
}

bool GLWindow::isOpen() const
{
    return m_handle && !glfwWindowShouldClose(m_handle);
}

int GLWindow::width() const
{
    return m_width;
}

int GLWindow::height() const
{
    return m_height;
}

float GLWindow::dpiScaling() const
{
    return m_dpiScaling;
}

bool GLWindow::enableGUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(m_dpiScaling);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/verdana.ttf", 18.0f * m_dpiScaling, nullptr, nullptr);

    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(m_handle, true))
    {
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 130"))
    {
        return false;
    }

    m_logUiWidget = std::make_shared<LogWidget>();

    m_logCallback = Logger::getInstance().registerCallback(
        [=](LogSeverity s, const std::string& message)
        {
            m_logUiWidget->AddLog("[%s] %s\n", Logger::severity(s).c_str(), message.c_str());
        }
    );

    m_guiInitialized = true;
    return m_guiInitialized;
}

void GLWindow::onResize(int /*width*/, int /*height*/)
{
}

void GLWindow::onGUI()
{
    //ImGui::ShowDemoWindow(&m_showUiDemo);

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    m_logUiWidget->Draw("Log", &m_showUiDemo);
}

void GLWindow::handleResize(int width, int height)
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);

    onResize(width, height);
}

void GLWindow::cleanupGUI()
{
    if (!m_guiInitialized)
    {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GLWindow* GLWindow::getGLWindowFromHandle(GLFWwindow* handle)
{
    if (handle)
    {
        return static_cast<GLWindow*>(glfwGetWindowUserPointer(handle));
    }

    return nullptr;
}

void GLWindow::onResizeCallback(GLFWwindow* handle, int width, int height)
{
    GLWindow* window = getGLWindowFromHandle(handle);
    if (window)
    {
        window->handleResize(width, height);
    }
}
