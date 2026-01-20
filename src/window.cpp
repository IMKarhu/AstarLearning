#include "window.h"
#include "camera.h"
#include "grid.h"
#include <glad.h>

#include <iostream>

Window::Window(uint32_t width, uint32_t height, std::string title)
    : m_title(title)
    , m_width(width)
    , m_height(height)
{
    if (!initWindow())
    {
        std::cout << "something went wrong while initializing..." << std::endl;
        return;
    }
}

Window::~Window() {}

bool Window::initWindow()
{
    if (!glfwInit())
    {
        std::cout << "could not initialize window" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize OpenGL context\n" << std::endl;
        glfwTerminate();
        glfwDestroyWindow(m_window);
        return -1;
    }

    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, m_width, m_height);

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetCursorPosCallback(m_window, cursor_position_callback);
    glfwSetScrollCallback(m_window, mouse_scroll_callback);
    return true;
}

void Window::pollEvents()
{
    glfwPollEvents();
}

bool Window::isWindowClosed()
{
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

void Window::setCamera(Camera* camera)
{
    m_camera = camera;
}

void Window::setGrid(Grid* grid)
{
    m_grid = grid;
}

void Window::setTileCallback(tileClickCallback tcb)
{
    m_tileClick = tcb;
}

void Window::setWindowShouldClose(bool value)
{
    glfwSetWindowShouldClose(m_window, value);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    if (win->m_camera)
    {
        win->m_camera->resize(width, height);
    }
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));


    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            win->mouseDown = true;
            glfwGetCursorPos(window, &win->lastx, &win->lasty);
        }
        else if (action == GLFW_RELEASE)
        {
            win->mouseDown = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (win->m_grid->state() == Grid::GameState::MENU)
            return;

        printf(" left click\n");
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        win->processLeftClick(mouseX, mouseY);
    }
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win->mouseDown || !win->m_camera)
        return;

    float sensitivity = 0.3f;
    float xoffset = xpos - win->lastx;
    float yoffset = ypos - win->lasty;

    win->lastx = xpos;
    win->lasty = ypos;

    win->m_camera->processMouse(xoffset, yoffset, sensitivity);

}

void Window::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!win->m_camera)
        return;

    win->m_camera->processScroll(yoffset);
}

void Window::processLeftClick(double x, double y)
{
    if (!m_camera || !m_grid)
    {
        return;
        printf("camera or grid doesnt exist");
    }

    m_camera->update();
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    float ndcX = (2.0f * x) / width - 1.0f;
    float ndcY = 1.0f - (2.0f * y) / height;

    glm::vec4 rayStartNDC = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 rayEndNDC = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    glm::mat4 invVP = glm::inverse(m_camera->proj() * m_camera->view());

    glm::vec4 rayStartW = invVP * rayStartNDC;
    rayStartW /= rayStartW.w;
    glm::vec4 rayEndW = invVP * rayEndNDC;
    rayEndW /= rayEndW.w;

    glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndW - rayStartW));
    glm::vec3 cameraPos = m_camera->position();
    float t = -cameraPos.y / rayDir.y;
    if (t < 0)
    {
        printf("t is 0");
        return;
    }

    glm::vec3 hit = cameraPos + t * rayDir;
    glm::vec2 tile = m_grid->getTileIndex(hit);
    if (m_tileClick)
    {
        m_tileClick(tile.x, tile.y);
    }
}
