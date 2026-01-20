#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <functional>



class Camera;
class Grid;

class Window
{
public:
    using tileClickCallback = std::function<void(int, int)>;
    Window(uint32_t width, uint32_t height, std::string title);
    ~Window();

    bool initWindow();
    void pollEvents();
    bool isWindowClosed();
    void swapBuffers();
    void setCamera(Camera* camera);
    void setGrid(Grid* grid);
    void setTileCallback(tileClickCallback tcb);
    void setWindowShouldClose(bool value);

    uint32_t width() { return m_width; }
    uint32_t height() { return m_height; }
    GLFWwindow* getWindow() { return m_window; }
private:
    std::string m_title = "Super awesome pickup and deliver game";
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    GLFWwindow* m_window = nullptr;
    tileClickCallback m_tileClick;
    
    //Not the biggest fan of having pointer for camera and grid here, but works for now...
    Camera* m_camera = nullptr;
    Grid* m_grid = nullptr;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void processLeftClick(double x, double y);

    /* mouse stuff */
    bool mouseDown = false;
    double lastx;
    double lasty;

    float yaw = 45.0f;
    float pitch = 45.0f;
    float sensitivity = 0.3f;
};
