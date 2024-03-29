#include "Application/GLWindow.h"
#include "Application/InputHandler.h"
#include "API/GraphicsAPI.h"

#include "Common/Timer.h"

#include "GUI/imgui.h"
#include "GUI/imgui_impl_glfw.h"
#include "GUI/imgui_impl_opengl3.h"
#include "GUI/IWidget.h"
#include "GUI/LogWidget.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#ifdef _DEBUG
#include <iostream>

void glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei /*length*/,
    const char* message,
    const void* /*userParam*/)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return;
    }

    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other"; break;
    } std::cout << std::endl;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification"; break;
    } 
    std::cout << std::endl;
    std::cout << std::endl;
}
#endif

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

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

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

#ifdef _DEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
            nullptr, GL_TRUE);
    }
#endif

    int actualWidth, actualHeight;
    glfwGetWindowSize(m_handle, &actualWidth, &actualHeight);
    handleResize(actualWidth, actualHeight);

    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, onResizeCallback);

    m_inputHandler = std::make_shared<InputHandler>(m_handle);
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

void GLWindow::render()
{
    renderGUI();
}

void GLWindow::renderGUI()
{
    if (!m_guiInitialized)
    {
        return;
    }

    GraphicsAPIBeginScopedDebugGroup("GUI");

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
        // TODO add timer to check how long we wait (Present&Wait)

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

void GLWindow::update(double deltaTime)
{
    m_inputHandler->update(deltaTime);

    for (IWidgetSPtr widget : m_widgets)
    {
        widget->update(deltaTime);
    }
}

int GLWindow::width() const
{
    return m_width;
}

int GLWindow::height() const
{
    return m_height;
}

glm::vec4 GLWindow::dimensions() const
{
    return glm::vec4(width(), height(), 1.f/width(), 1.f/height());
}

SharedResource::Handle GLWindow::handle() const
{
    return SharedResource::Handle(0);
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


    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
    colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
    colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
    colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

    style.ChildRounding = 4.0f;
    style.FrameBorderSize = 1.0f;
    style.FrameRounding = 2.0f;
    style.GrabMinSize = 7.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 12.0f;
    style.ScrollbarSize = 13.0f;
    style.TabBorderSize = 1.0f;
    style.TabRounding = 0.0f;
    style.WindowRounding = 4.0f;


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

    std::shared_ptr<LogWidget> logWidget = std::make_shared<LogWidget>("Log");

    m_logCallback = Logger::getInstance().registerCallback(
        [=](LogSeverity s, const std::string& message)
        {
            if (static_cast<int>(s) > static_cast<int>(LogSeverity::Debug))
            {
                logWidget->AddLog("[%s] %s\n", Logger::severity(s).c_str(), message.c_str());
            }
        }
    );

    m_widgets.push_back(logWidget);

    m_guiInitialized = true;
    return m_guiInitialized;
}

void GLWindow::addWidget(IWidgetSPtr widget)
{
    m_widgets.push_back(widget);
}

InputHandlerSPtr GLWindow::inputHandler() const
{
    return m_inputHandler;
}

void GLWindow::onResize(int /*width*/, int /*height*/)
{
}

void GLWindow::onGUI()
{
    ImGuiIO& io = ImGui::GetIO();
    m_inputHandler->guiHasMouseFocus(io.WantCaptureMouse);

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    
    // draw main menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Widgets"))
        {
            for (IWidgetSPtr widget : m_widgets)
            {
                if (ImGui::MenuItem(widget->name().c_str(), NULL, widget->visible()))
                {
                    widget->setVisible(!widget->visible());
                }
            }

            //ImGui::Separator();

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // draw widgets
    for (IWidgetSPtr widget : m_widgets)
    {
        widget->draw();
    }

    ImGui::ShowDemoWindow(&m_showUiDemo);
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
