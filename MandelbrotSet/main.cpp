#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <vector>

#include "shader.h"

int main(void)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    const char* windowTitle = "Madlebrot set";
    int width = 800, height = 800;
    window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(glewStatus));
        return -1;
    }

    // Get the OpenGL version
    std::cout << glGetString(GL_VERSION) << std::endl;

    // Enable the Depth Buffer 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set the viewport
    glViewport(0, 0, width, height);

    // Update the viewport on window resize
    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    );

    // Store time to measure framerate
    double lastTime = glfwGetTime();
    long int frameCount = 0;

    /*********
    * CANVAS *
    *********/
    // 0. Variable declaration and data initialization
    GLuint canvasVertexArrayID; // VAO
    GLuint canvasVertexBufferID; // VBO
    GLuint canvasElementBufferID; // EBO
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
    Shader shader("vertex.glsl", "fragment.glsl");

    /***********
    * TEXTURES *
    ***********/
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
    glm::dvec2
        lb = glm::dvec2(-2.0, -2.0),
        rt = glm::dvec2( 2.0,  2.0),
        dt = glm::dvec2(
             0.360240443437614363236125244449545308482607807958585750488375814740195346059218100311752936722773426396233731729724987737320035372683285317664532401218521579554288661726564324134702299962817029213329980895208036363104546639698106204384566555001322985619004717862781192694046362748742863016467354574422779443226982622356594130430232458472420816652623492974891730419252651127672782407292315574480207005828774566475024380960675386215814315654794021855269375824443853463117354448779647099224311848192893972572398662626725254769950976527431277402440752868498588785436705371093442460696090720654908973712759963732914849861213100695402602927267843779747314419332179148608587129105289166676461292845685734536033692577618496925170576714796693411776794742904333484665301628662532967079174729170714156810530598764525260869731233845987202037712637770582084286587072766838497865108477149114659838883818795374195150936369987302574377608649625020864292915913378927790344097552591919409137354459097560040374880346637533711271919419723135538377394364882968994646845930838049998854075817859391340445151448381853615103761584177161812057928,
            -0.6413130610648031748603750151793020665794949522823052595561775430644485741727536902556370230689681162370740565537072149790106973211105273740851993394803287437606238596262287731075999483940467161288840614581091294325709988992269165007394305732683208318834672366947550710920088501655704252385244481168836426277052232593412981472237968353661477793530336607247738951625817755401065045362273039788332245567345061665756708689359294516668271440525273653083717877701237756144214394870245598590883973716531691124286669552803640414068523325276808909040317617092683826521501539932397262012011082098721944643118695001226048977430038509470101715555439047884752058334804891389685530946112621573416582482926221804767466258346014417934356149837352092608891639072745930639364693513216719114523328990690069588676087923656657656023794484324797546024248328156586471662631008741349069961493817600100133439721557969263221185095951241491408756751582471307537382827924073746760884081704887902040036056611401378785952452105099242499241003208013460878442953408648178692353788153787229940221611731034405203519945313911627314900851851072122990492499999999999999999991
        );
    glm::dvec4
        lbrt = glm::dvec4(lb, rt);
    // Zoom per second and Total zoom
    double zoom = 1.25;
    double totalZoom = 1;
    // Color period and Max iterations
    float colorPeriod = 100.0f;
    float maxIt = 1.0f;

    /************
    * MAIN LOOP *
    ************/
    while (!glfwWindowShouldClose(window))
    {
        /********
        * STATS *
        ********/
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
        char titleBuffer[256];
        sprintf_s(titleBuffer, "%s -- FPS: %3.0lf -- Latency: %1.5lf -- Frame count: %ld -- Current zoom: %.2e -- Max iterations: %d", windowTitle, frameRate, frameLatency, frameCount, totalZoom, (int)maxIt);
        glfwSetWindowTitle(window, titleBuffer);

        /******
        * MVP *
        ******/
        // Adjust projection to window size
        glfwGetWindowSize(window, &width, &height);

        // Projection
        float ar = (float)width / (float)height;
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
        shader.use();

        // Draw canvas
        glBindTexture(GL_TEXTURE_1D, texture);
        shader.setMatrix4fv("mvp", mvp);
        shader.setVector4dv("lbrt", lbrt);
        shader.setFloat("colorPeriod", colorPeriod);
        shader.setFloat("maxIt", maxIt);
        glBindVertexArray(canvasVertexArrayID);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /***************
        * UPDATE LOGIC *
        ***************/
        // Update the projection matrix
        frameLatency = 1.0f / 144.0f;
        lb = dt + (lb - dt) / ((zoom - 1.0f) * frameLatency + 1.0f);
        rt = dt + (rt - dt) / ((zoom - 1.0f) * frameLatency + 1.0f);
        totalZoom *= ((zoom - 1.0f) * frameLatency + 1.0f);
        lbrt = glm::dvec4(lb, rt);
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