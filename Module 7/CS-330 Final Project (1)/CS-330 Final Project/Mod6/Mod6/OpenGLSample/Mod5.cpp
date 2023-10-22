#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions
#include "SceneBuilder.h"
#include "ShapeBuilder.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

const char* const WINDOW_TITLE = "Alex Casanova Final Project"; 

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

GLightMesh lightMesh;
ShapeBuilder builder;

//main window
GLFWwindow* gWindow = nullptr;

//shader program
GLuint gShaderProgram;
GLuint gLightProgramId;

// scene vector for drawing shapes
vector<GLMesh> scene;



// Texture
GLuint gTextureId;
glm::vec2 gUVScale(1.0f, 1.0f);
GLint gTexWrapMode = GL_REPEAT;

// Shader programs
GLuint gCubeProgramId;
GLuint gLampProgramId;

// camera
Camera gCamera(glm::vec3(0.0f, -0.2f, 7.0f));
float gLastX = WINDOW_WIDTH / 2.0f;
float gLastY = WINDOW_HEIGHT / 2.0f;
bool gFirstMouse = true;
float controllerMoveSpeed = 0.25;
// timing
float gDeltaTime = 0.0f; // time between current frame and last frame
float gLastFrame = 0.0f;


// Light color, position and scale
glm::vec3 glightColor(1.0f, 1.0f, 1.0f);
glm::vec3 glightPosition(4.5f, 1.0f, -1.5f);
glm::vec3 glightScale(0.001f);

// Light animation (Toggled with L Key)
bool gIsLampOrbiting = true;

// Perspective Change (Toggled with P Key)
bool perspective = false;

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void URender(vector<GLMesh> scene);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void UCreateLightMesh(GLightMesh& lightMesh);

const GLchar* vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
	layout(location = 1) in vec3 normal; // VAP position 1 for normals
	layout(location = 2) in vec2 textureCoordinate;

	out vec3 vertexNormal; // For outgoing normals to fragment shader
	out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
	out vec2 vertexTextureCoordinate;

	//Uniform / Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);
		vertexFragmentPos = vec3(model * vec4(position, 1.0f)); 
		vertexNormal = mat3(transpose(inverse(model))) * normal;
		vertexTextureCoordinate = textureCoordinate;
	}
);

// Shape Fragment Shader Source Code
const GLchar* fragmentShaderSource = GLSL(440,

	in vec3 vertexFragmentPos;
	in vec3 vertexNormal;	
	in vec2 vertexTextureCoordinate; // for texture coordinates, not color

	out vec4 fragmentColor;
	
	uniform vec3 objectColor;
	uniform vec3 lightColor;
	uniform vec3 lightPos;
	uniform vec3 viewPosition;
	
	uniform sampler2D uTexture;
	uniform vec2 uvScale;

    //Phong Lighting Calculations
	void main()
	{
		//Calculate Ambient lighting*/
		float lightStrength = 0.2f; // Set ambient or global lighting strength
		vec3 light = lightStrength * lightColor; // Generate ambient light color

		//Calculate Diffuse lighting*/
		vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
		vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube

		float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light

		vec3 diffuse = impact * lightColor; // Generate diffuse light color

		//Calculate Specular lighting*/
		float specularIntensity = 0.8f; // Set specular light strength
		float highlightSize = 16.0f; // Set specular highlight size
		vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
		vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
		//Calculate specular component
		float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
		vec3 specular = specularIntensity * specularComponent * lightColor;

		// Texture holds the color to be used for all three components
		vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

		// Calculate phong result
		vec3 phong = (light + diffuse + specular) * textureColor.xyz;

		fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU

	}
);


/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
    }
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,
    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU
    void main()
    {
        fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
    }
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{

    //check if initialized correctly
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    //build scene
    SceneBuilder::UBuildScene(scene);

    //build shader 
    for (auto& m : scene)
    {
        if (!UCreateTexture(m.texFilename, m.textureId))
        {
            cout << "Failed to load texture " << m.texFilename << endl;
            return EXIT_FAILURE;
        }

        if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gShaderProgram)) {
            return EXIT_FAILURE;
        }
    }

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLightProgramId))
        return EXIT_FAILURE;

    // Create Light Object
    UCreateLightMesh(lightMesh);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gShaderProgram);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gShaderProgram, "uTexture"), 0);

    //rendering loop
    //keep checking if window has closed
    while (!glfwWindowShouldClose(gWindow))
    {
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        //process user input
        UProcessInput(gWindow);

        //render frame
        URender(scene);

        glfwPollEvents();
    }

    //clean up
    for (auto& m : scene)
    {
        UDestroyMesh(m);
    }

    scene.clear();

    UDestroyShaderProgram(gShaderProgram);
    UDestroyShaderProgram(gLightProgramId);

    exit(EXIT_SUCCESS);
}


bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    //initialize GLFW, GLEW, create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create window
    //use constants for vars
    *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    //window fails to be created
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    return true;
}

// process all input: query GLFW whether relevant keys are pressed/released
void UProcessInput(GLFWwindow* window)
{
    // exit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    //Toggle Light Orbit
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        if (gIsLampOrbiting) { gIsLampOrbiting = false; }
        else { gIsLampOrbiting = true; };

    //Toggle Perspective
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        if (perspective) { perspective = false; }
        else { perspective = true; };

    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    //std::cout << present << std::endl;

    if (1 == present)
    {
        int axesCount;       
        int buttonsCount;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonsCount);
        
        //L-Analog Directional Movement
        if (axes[1] < -0.05)
            gCamera.ProcessKeyboard(FORWARD, (-gDeltaTime * axes[1]));
        if (axes[1] > 0.05)
            gCamera.ProcessKeyboard(BACKWARD, (gDeltaTime * axes[1]));

        if (axes[0] < -0.05)
            gCamera.ProcessKeyboard(LEFT, (-gDeltaTime * axes[0]));
        if (axes[0] > 0.05)
            gCamera.ProcessKeyboard(RIGHT, (gDeltaTime * axes[0]));

        //D-Pad Directional Movement
        if (buttons[10] == GLFW_PRESS)
            gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
        if (buttons[11] == GLFW_PRESS)
            gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
            
        if (buttons[12] == GLFW_PRESS)
            gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
        if (buttons[13] == GLFW_PRESS)
            gCamera.ProcessKeyboard(LEFT, gDeltaTime);
       
        //R-Analog Camera Movement
        if (axes[2] < -0.05)
            //Left
            gCamera.ProcessMouseMovement((controllerMoveSpeed * axes[2]), 0);

        if (axes[2] > 0.05)
            //Right
            gCamera.ProcessMouseMovement((controllerMoveSpeed * axes[2]), 0);
        
        if (axes[3] < -0.05)
            //Up
            gCamera.ProcessMouseMovement(0, (-controllerMoveSpeed * axes[3]));

        if (axes[3] > 0.05)
            //Down
            gCamera.ProcessMouseMovement(0, (-controllerMoveSpeed * axes[3]));

        //UP / DOWN movement
        if (axes[4] > -0.95)
            gCamera.ProcessKeyboard(DOWN, (gDeltaTime * (axes[4]+1)));
        if (axes[5] > -0.95)
            gCamera.ProcessKeyboard(UP, (gDeltaTime * (axes[5] + 1)));

        
        if (buttons[1] == GLFW_PRESS)
            cout << "B ";
        if (buttons[2] == GLFW_PRESS)
            cout << "X ";
        if (buttons[3] == GLFW_PRESS)
            cout << "Y ";
        if (buttons[4] == GLFW_PRESS) {
            cout << "LB ";
            if (controllerMoveSpeed > 0.1) {
                controllerMoveSpeed -= 0.001;
            }
        }
        if (buttons[5] == GLFW_PRESS) {
            cout << "RB ";            
            if (controllerMoveSpeed < .75) {
                controllerMoveSpeed += 0.001;
            }
        }
        if (buttons[6] == GLFW_PRESS)
            cout << "Select ";
        if (buttons[7] == GLFW_PRESS)
            cout << "Start ";
        if (buttons[8] == GLFW_PRESS)
            cout << "8 ";
        if (buttons[9] == GLFW_PRESS)
            cout << "9 ";
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

void URender(vector<GLMesh> scene)
{
    // Code from tutorial, allows light to orbit scene
    constexpr float angularVelocity = glm::radians(45.0f);
    if (gIsLampOrbiting)
    {
        glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(glightPosition, 1.0f);
        glightPosition.x = newPosition.x;
        glightPosition.y = newPosition.y;
        glightPosition.z = newPosition.z;
    }

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Background (black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // transform the camera (x, y, z)
    glm::mat4 view = gCamera.GetViewMatrix();

    // create perspective projection (fov, aspect ratio, near plane, far plane)
    glm::mat4 projection;
    if (!perspective)
    {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

    // loop to draw each shape individually
    for (int i = 0; i < scene.size(); ++i)
    {
        GLMesh mesh = scene[i];

        // activate vbo's within mesh's vao
        glBindVertexArray(mesh.vao);

        // set shader
        glUseProgram(gShaderProgram);

        // gets and passes transform matrices to shader prgm for shapes
        GLint modelLocation = glGetUniformLocation(gShaderProgram, "model");
        GLint viewLocation = glGetUniformLocation(gShaderProgram, "view");
        GLint projLocation = glGetUniformLocation(gShaderProgram, "projection");

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mesh.model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));

        // Reference matrix uniforms from the shape shader program for the shape color, light color, light position, and camera position
        GLint objectColorLoc = glGetUniformLocation(gShaderProgram, "objectColor");

        // light
        GLint lightColorLoc = glGetUniformLocation(gShaderProgram, "lightColor");
        GLint lightPositionLoc = glGetUniformLocation(gShaderProgram, "lightPos");

        // Camera view
        GLint viewPositionLoc = glGetUniformLocation(gShaderProgram, "viewPosition");

        // Pass color, light, and camera data to the shape shader 
        glUniform3f(objectColorLoc, mesh.p[0], mesh.p[1], mesh.p[2]);

        // Light
        glUniform3f(lightColorLoc, glightColor.r, glightColor.g, glightColor.b);
        glUniform3f(lightPositionLoc, glightPosition.x, glightPosition.y, glightPosition.z);


        const glm::vec3 cameraPosition = gCamera.Position;
        glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


        GLint UVScaleLoc = glGetUniformLocation(gShaderProgram, "uvScale");
        glUniform2fv(UVScaleLoc, 1, glm::value_ptr(mesh.gUVScale));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.textureId);

        // Draws the shape
        glDrawArrays(GL_TRIANGLES, 0, mesh.nIndices);
    }
    // Vars for lights
    glm::mat4 model;
    GLint modelLoc;
    GLint viewLoc;
    GLint projLoc;

    // --------------------
    // Draw the Light
    glUseProgram(gLightProgramId);
    glBindVertexArray(lightMesh.vao);

    // Light location and Scale
    model = glm::translate(glightPosition) * glm::scale(glightScale);

    // Matrix uniforms from the Light Shader program
    modelLoc = glGetUniformLocation(gLightProgramId, "model");
    viewLoc = glGetUniformLocation(gLightProgramId, "view");
    projLoc = glGetUniformLocation(gLightProgramId, "projection");

    // Matrix data
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the light
    glDrawArrays(GL_TRIANGLES, 0, lightMesh.nVertices);

    // deactivate vao's
    glBindVertexArray(0);
    glUseProgram(0);

    // swap front and back buffers
    glfwSwapBuffers(gWindow);
}


bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}
void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
}

// Template for creating a cube light
void UCreateLightMesh(GLightMesh& lightMesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        //Front Face         //Positive Z Normal
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        //Left Face          //Negative X Normal
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Right Face         //Positive X Normal
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Bottom Face        //Negative Y Normal
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        //Top Face           //Positive Y Normal
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    lightMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &lightMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(lightMesh.vao);

    // Create buffer
    glGenBuffers(1, &lightMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, lightMesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    //error reporting
    int success = 0;
    char infoLog[512];

    //create shader program object
    programId = glCreateProgram();

    //create vertex and frag shader obj
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    //get shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    //compile vertex shader
    glCompileShader(vertexShaderId);

    //check for errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    //compile frag shader
    glCompileShader(fragmentShaderId);

    //check for errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    //shaders compiled, attach to shader program object from above
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    //link the program object
    glLinkProgram(programId);

    //check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);

    return true;
}

void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}