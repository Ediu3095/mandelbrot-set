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
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
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

    /*******
    * ZOOM *
    *******/
    // Left-Bottom, Right-Top and Destination coordinates
    glm::dvec2
        lb = glm::dvec2(-2.0, -2.0),
        rt = glm::dvec2( 2.0,  2.0),
        dt = glm::dvec2(
            -1.9997740601362903593126807559602500475710416233856384007148508574291012335984591928248364190215796259575718318799960175396106897245889581254834492701372949636783094955897931317174101259095891469501748126725148714587333938548443819033709904187344921523413310221887295870857771431011674873342592895504186325482220668710775749899926429101099841583206278295793058921625817004481783699245865364627140554117737774937789463895102748671351750212506004241754983473339789940659968568850689353099462034492524909310777724611601104714214019347435268544619054369865904944457792527241696528695821059623303046651934176389789308453627525109367436309636375268231073110318555064708363221007235298404379856922536028913291478442839193381367508575286692330907891402483843152933153748354825108021776358693600801782904774626935265722056455978643513448489091026679036353407968495795003386248005939867069799946547181378474054113117046900560609110812439442002663909295191705374444149326937073460052706389967886211172676612720028299452788285465688867116337489531157494508508315428488520037968118008255840569742557333862639124341116894229885253643651920014148109308402199399127712572209466874971603743536096235390414412927589954662603878558182262865151900604451937214289079939337905846647369517138325441736853526711818853134657265043099539402286244220638999824999819000131999789999857999958,
            -0.0000000032900403214794350534969786759266805967852946505878410088326046927853549452991056352681196631150325234171525664335353457621247922992470898021063583060218954321140472066153878996044171428801408137278072521468882260382336298800961530905692393992277070012433445706657829475924367459793505729004118759963065667029896464160298608486277109065108339157276150465318584383757554775431988245033409975361804443001325241206485033571912765723551757793318752425925728969073157628495924710926832527350298951594826689051400340011140584507852761857568007670527511272585460136585523090533629795012272916453744029579624949223464015705500594059847850617137983380334184205468184810116554041390142120676993959768153409797953194054452153167317775439590270326683890021272963306430827680201998682699627962109145863135950941097962048870017412568065614566213639455841624790306469846132055305041523313740204187090956921716703959797752042569621665723251356946610646735381744551743865516477084313729738832141633286400726001116308041460406558452004662264165125100793429491308397667995852591271957435535504083325331161340230101590756539955554407081416407239097101967362512942992702550533040602039494984081681370518238283847808934080198642728761205332894028474812918370467949299531287492728394399650466260849557177609714181271299409118059191938687461000000000000000000000000000000000000
        );
    glm::dvec4
        lbrt = glm::dvec4(lb, rt);
    // Zoom per second and Total zoom
    double zoom = 1.25;
    double totalZoom = 1;

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
        sprintf_s(titleBuffer, "%s -- FPS: %3.0lf -- Latency: %1.5lf -- Frame count: %ld -- Current zoom: %.2e", windowTitle, frameRate, frameLatency, frameCount, totalZoom);
        glfwSetWindowTitle(window, titleBuffer);

        /*********
        * RENDER *
        *********/
        // Clear the color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use our shader
        shader.use();

        // Draw canvas
        shader.setVector4dv("lbrt", lbrt);
        glBindVertexArray(canvasVertexArrayID);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /***************
        * UPDATE LOGIC *
        ***************/
        // Update the projection matrix
        lb = dt + (lb - dt) / ((zoom - 1.0f) * frameLatency + 1.0f);
        rt = dt + (rt - dt) / ((zoom - 1.0f) * frameLatency + 1.0f);
        totalZoom *= ((zoom - 1.0f) * frameLatency + 1.0f);
        lbrt = glm::dvec4(lb, rt);

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