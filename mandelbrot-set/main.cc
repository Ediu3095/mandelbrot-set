#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "mandelbrot-set/wrapper/shader.h"

#define WIDTH 800
#define HEIGHT 600

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  // Initialize glfw
  glfwInit();
  glfwWindowHint(GLFW_SAMPLES, 4);  // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a window and opengl context
  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Set glfw callbacks to handle IO events
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  // Load opengl functions
  if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Set the viewport
  glViewport(0, 0, WIDTH, HEIGHT);

  // Enable the Depth Buffer 
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

    // Store time to measure framerate
    double lastTime = glfwGetTime();
    long int frameCount = 0;

    /*********
    * CANVAS *
    *********/
    // 0. Variable declaration and data initialization
    GLuint canvasVertexArrayID;  // VAO
    GLuint canvasVertexBufferID;  // VBO
    GLuint canvasElementBufferID;  // EBO
    glGenVertexArrays(1, &canvasVertexArrayID);
    glGenBuffers(1, &canvasVertexBufferID);
    glGenBuffers(1, &canvasElementBufferID);
    const GLfloat canvasVertexBufferData[] = {
        //  x,     y,    z,    R,    I
        -2.0f, -1.0f, 0.0f, -0.5f, 0.0f,
         2.0f, -1.0f, 0.0f,  1.5f, 0.0f,
         2.0f,  1.0f, 0.0f,  1.5f, 1.0f,
        -2.0f,  1.0f, 0.0f, -0.5f, 1.0f
    };
    const GLuint canvasElementBufferData[] = {
        0, 1, 2,
        0, 2, 3
    };

    // 1. Bind Vertex Array Object
    glBindVertexArray(canvasVertexArrayID);

    // 2. Copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, canvasVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(canvasVertexBufferData), canvasVertexBufferData, GL_STATIC_DRAW);

    // 3. Copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvasElementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(canvasElementBufferData), canvasElementBufferData, GL_STATIC_DRAW);

    // 4. Set the vertex attribute pointers (0 = vertices, 1 = complex coords)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    /**********
    * SHADERS *
    **********/
    // Create and compile our GLSL program from the shaders
    opengl::Shader shader("mandelbrot-set/shaders/mandelbrot.vert", "mandelbrot-set/shaders/mandelbrot.frag");

    /***********
    * TEXTURES *
    ***********/
    // Activate the texture unit before binding a texture
    glActiveTexture(GL_TEXTURE0);
    // Create a texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    // Set the texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load and generate the texture
    int textureWidth = 4, textureNrChannels = 3;
    const GLubyte textureData[] = {
          0, 139, 224,
        215, 215, 215,
        223, 113,   0,
         60,   0,  57
    };
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, textureWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    /*******
    * ZOOM *
    *******/
    // Left-Bottom, Right-Top and Destination coordinates
    glm::dvec2 left_bottom(-2.0, -2.0), right_top(2.0, 2.0), destination(0.36024044343761436323, -0.64131306106480317486);
    glm::dvec4 left_bottom_right_top(left_bottom, right_top);
    // Zoom per second and Total zoom
    double zoom = 1.25;
    double totalZoom = 1;
    // Color period and Max iterations
    float colorPeriod = 100.0f;
    float maxIt = 1.0f;

    while (!glfwWindowShouldClose(window)) {
        // Measure speed
        double currentTime = glfwGetTime();
        double frameLatency = (currentTime - lastTime);
        double frameRate = 1. / frameLatency;

        // Fix framerate
        if (frameRate > 144)
            continue;

        // Update timing variables
        lastTime = currentTime;
        frameCount++;

        // Update window title
        std::stringstream ss;
        ss << "FPS: " << frameRate << " -- Latency: " << frameLatency << " -- Frame count: " << frameCount << " -- Current zoom: " << totalZoom << " -- Max iterations: " << static_cast<int>(maxIt);
        glfwSetWindowTitle(window, ss.str().c_str());

        /******
        * MVP *
        ******/
        // Adjust projection to window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Projection
        float ar = static_cast<float>(width) / static_cast<float>(height);
        float ag = ar < 1.0f ? 90.0f : 90.0f / ar;
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), ar, 0.1f, 100.0f);

        // Camera
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // Model
        glm::mat4 model = glm::mat4(1.0f);
        
        // MVP
        glm::mat4 mvp = projection * view * model;

        /*********
        * RENDER *
        *********/
        // Clear the color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        shader.Use();

        // Draw canvas
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, texture);
        glBindVertexArray(canvasVertexArrayID);
        shader.SetUniform("mvp", mvp);
        shader.SetUniform("lbrt", left_bottom_right_top);
        shader.SetUniform("colorPeriod", colorPeriod);
        shader.SetUniform("maxIt", maxIt);
        shader.SetUniform("colormap", 0);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /***************
        * UPDATE LOGIC *
        ***************/
        // Update the projection matrix
        frameLatency = 1.0f / 144.0f;
        left_bottom = destination + (left_bottom - destination) / ((zoom - 1.0f) * frameLatency + 1.0f);
        right_top = destination + (right_top - destination) / ((zoom - 1.0f) * frameLatency + 1.0f);
        totalZoom *= ((zoom - 1.0f) * frameLatency + 1.0f);
        left_bottom_right_top = glm::dvec4(left_bottom, right_top);
        maxIt = maxIt + 20 * frameLatency;

        /****************
        * UPDATE SCREEN *
        ****************/
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}