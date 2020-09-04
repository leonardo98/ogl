//
// TODO: ��������, ����� ���������� ���
// ��� ���� ��� ���� � http://www.opengl-tutorial.org/
//

//#include "system/texture_remover.h"
#include "scene/actor.h"

#include <chrono>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int CreateWindow(int width, int height, const char *windowTitle)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    // disabled for 2D - todo: shoud be fixed for using 3D

    // Enable depth test
    //glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    //glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    // 2D case - todo: shoud be fixed for using 3D
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

void game(tst::Actor* root);

std::mutex _mutexGameThread;
bool terminateGameThread = false;
std::mutex _mutexMainThread;
bool terminateMainThread = false;

//void WaitForInput()
//{
//    while (true)
//    {
//        {
//            std::lock_guard<std::mutex> lock(_mutex);
//            if (terminateGameThread)
//            {
//                break;
//            }
//        }
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(10ms);
//    }
//}

void gameHelp(tst::Actor* root)
{
    game(root);

    {
        std::lock_guard<std::mutex> lock(_mutexMainThread);
        terminateMainThread = true;
    }
}

int main(void)
{
    if (CreateWindow(1024, 768, "Threaded OpenGL") != 0)
    {
        fprintf(stderr, "Failed to CreateWindow\n");
        return -1;
    }

    fprintf(stdout, "CreateWindow is Ok!\n");

    glm::mat4x4 rootMatrix;

    // init rootMatrix
    // 2D case!!! todo: shoud be fixed for using 3D

    //glm::mat4x4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    //// Camera matrix
    //glm::vec3 position = glm::vec3(0, 0, 5);
    //glm::vec3 direction = glm::vec3(0.00159254798f, 0.000000000f, -0.999998748f);
    //glm::vec3 up = glm::vec3(0, 1, 0);
    //glm::mat4x4 ViewMatrix = glm::lookAt(
    //    position,           // Camera is here
    //    position + direction, // and looks here : at the same position, plus "direction"
    //    up                  // Head is up (set to 0,-1,0 to look upside-down)
    //);
    //rootMatrix = ProjectionMatrix * ViewMatrix;


    tst::Actor scene;
    std::thread gameThread(gameHelp, &scene);

    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.Render(rootMatrix);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        scene.Update(0.01f);

        //glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0

    } // Check if the ESC key was pressed or the window was closed
    while (!terminateMainThread);

    {
        std::lock_guard<std::mutex> lock(_mutexGameThread);
        terminateGameThread = true;
    }
    gameThread.join();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
