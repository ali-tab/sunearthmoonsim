#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <strstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>
using namespace std;

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte* pixels = new GLubyte[totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id++;
    delete[] pixels;
    fout.flush();
    fout.close();
}

//key board control
void processInput(GLFWwindow* window)
{
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("Assignment1-ss", buffer_width, buffer_height);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 vertexColor;\n"
"out vec3 fragmentColor;\n"
"uniform mat4 MVP;\n"
"void main()\n"
"{\n"
"   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	fragmentColor = vertexColor;\n"
"}\0";


const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 fragmentColor;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"	color = fragmentColor;\n"
"}\n\0";


//sun
float get_sun_rotate_angle_around_itself(float day) {

    float angle;

    angle = (360.0f / 27) * day;
    return angle;
}

//earth
float get_earth_rotate_angle_around_sun(float day) {

    float angle;

    angle = (360.0f / 365) * day;
    return angle;
}

float get_earth_rotate_angle_around_itself(float day) {

    float angle;

    angle = 360.0f * day;
    return angle;
}

//moon
float get_moon_rotate_angle_around_earth(float day) {

    float angle;

    angle = (360.0f / 28) * day;
    return angle;
}

float get_moon_rotate_angle_around_itself(float day) {

    float angle;

    angle = (360.0f / 28) * day;
    return angle;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    int windowWidth = 1024;
    int windowHeight = 768;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Assignment1", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "GLFW Window Failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return -1;
    }

    //shaders
    int success;
    char error_msg[512];
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vs, 512, NULL, error_msg);
        std::cout << "Vertex Shader Failed: " << error_msg << std::endl;
    }
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fs, 512, NULL, error_msg);
        std::cout << "Fragment Shader Failed: " << error_msg << std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_msg);
        std::cout << "Program Link Error: " << error_msg << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    //earth vertices
    GLfloat sun[] = {
        -4.0f,-4.0f,-4.0f,
        -4.0f,-4.0f, 4.0f,
        -4.0f, 4.0f, 4.0f,
         4.0f, 4.0f,-4.0f,
        -4.0f,-4.0f,-4.0f,
        -4.0f, 4.0f,-4.0f,
         4.0f,-4.0f, 4.0f,
        -4.0f,-4.0f,-4.0f,
         4.0f,-4.0f,-4.0f,
         4.0f, 4.0f,-4.0f,
         4.0f,-4.0f,-4.0f,
        -4.0f,-4.0f,-4.0f,
        -4.0f,-4.0f,-4.0f,
        -4.0f, 4.0f, 4.0f,
        -4.0f, 4.0f,-4.0f,
         4.0f,-4.0f, 4.0f,
        -4.0f,-4.0f, 4.0f,
        -4.0f,-4.0f,-4.0f,
        -4.0f, 4.0f, 4.0f,
        -4.0f,-4.0f, 4.0f,
         4.0f,-4.0f, 4.0f,
         4.0f, 4.0f, 4.0f,
         4.0f,-4.0f,-4.0f,
         4.0f, 4.0f,-4.0f,
         4.0f,-4.0f,-4.0f,
         4.0f, 4.0f, 4.0f,
         4.0f,-4.0f, 4.0f,
         4.0f, 4.0f, 4.0f,
         4.0f, 4.0f,-4.0f,
        -4.0f, 4.0f,-4.0f,
         4.0f, 4.0f, 4.0f,
        -4.0f, 4.0f,-4.0f,
        -4.0f, 4.0f, 4.0f,
         4.0f, 4.0f, 4.0f,
        -4.0f, 4.0f, 4.0f,
         4.0f,-4.0f, 4.0f
    };

    //earth vertices
    GLfloat earth[] = {
        -2.5f,-2.5f,-2.5f,
        -2.5f,-2.5f, 2.5f,
        -2.5f, 2.5f, 2.5f,
         2.5f, 2.5f,-2.5f,
        -2.5f,-2.5f,-2.5f,
        -2.5f, 2.5f,-2.5f,
         2.5f,-2.5f, 2.5f,
        -2.5f,-2.5f,-2.5f,
         2.5f,-2.5f,-2.5f,
         2.5f, 2.5f,-2.5f,
         2.5f,-2.5f,-2.5f,
        -2.5f,-2.5f,-2.5f,
        -2.5f,-2.5f,-2.5f,
        -2.5f, 2.5f, 2.5f,
        -2.5f, 2.5f,-2.5f,
         2.5f,-2.5f, 2.5f,
        -2.5f,-2.5f, 2.5f,
        -2.5f,-2.5f,-2.5f,
        -2.5f, 2.5f, 2.5f,
        -2.5f,-2.5f, 2.5f,
         2.5f,-2.5f, 2.5f,
         2.5f, 2.5f, 2.5f,
         2.5f,-2.5f,-2.5f,
         2.5f, 2.5f,-2.5f,
         2.5f,-2.5f,-2.5f,
         2.5f, 2.5f, 2.5f,
         2.5f,-2.5f, 2.5f,
         2.5f, 2.5f, 2.5f,
         2.5f, 2.5f,-2.5f,
        -2.5f, 2.5f,-2.5f,
         2.5f, 2.5f, 2.5f,
        -2.5f, 2.5f,-2.5f,
        -2.5f, 2.5f, 2.5f,
         2.5f, 2.5f, 2.5f,
        -2.5f, 2.5f, 2.5f,
         2.5f,-2.5f, 2.5f
    };

    //earth vertices
    GLfloat moon[] = {
        -1.5f,-1.5f,-1.5f,
        -1.5f,-1.5f, 1.5f,
        -1.5f, 1.5f, 1.5f,
         1.5f, 1.5f,-1.5f,
        -1.5f,-1.5f,-1.5f,
        -1.5f, 1.5f,-1.5f,
         1.5f,-1.5f, 1.5f,
        -1.5f,-1.5f,-1.5f,
         1.5f,-1.5f,-1.5f,
         1.5f, 1.5f,-1.5f,
         1.5f,-1.5f,-1.5f,
        -1.5f,-1.5f,-1.5f,
        -1.5f,-1.5f,-1.5f,
        -1.5f, 1.5f, 1.5f,
        -1.5f, 1.5f,-1.5f,
         1.5f,-1.5f, 1.5f,
        -1.5f,-1.5f, 1.5f,
        -1.5f,-1.5f,-1.5f,
        -1.5f, 1.5f, 1.5f,
        -1.5f,-1.5f, 1.5f,
         1.5f,-1.5f, 1.5f,
         1.5f, 1.5f, 1.5f,
         1.5f,-1.5f,-1.5f,
         1.5f, 1.5f,-1.5f,
         1.5f,-1.5f,-1.5f,
         1.5f, 1.5f, 1.5f,
         1.5f,-1.5f, 1.5f,
         1.5f, 1.5f, 1.5f,
         1.5f, 1.5f,-1.5f,
        -1.5f, 1.5f,-1.5f,
         1.5f, 1.5f, 1.5f,
        -1.5f, 1.5f,-1.5f,
        -1.5f, 1.5f, 1.5f,
         1.5f, 1.5f, 1.5f,
        -1.5f, 1.5f, 1.5f,
         1.5f,-1.5f, 1.5f
    };

    float clrs[] = {

        0, 1, 0,
        0, 1, 0,
        0, 1, 0,

        1, 0, 0,
        1, 0, 0,
        1, 0, 0,

        0, 0, 1,
        0, 0, 1,
        0, 0, 1,

        1, 0, 0,
        1, 0, 0,
        1, 0, 0,

        0, 1, 0,
        0, 1, 0,
        0, 1, 0,

        0, 0, 1,
        0, 0, 1,
        0, 0, 1,


        1, 0, 1,
        1, 0, 1,
        1, 0, 1,


        1, 1, 0,
        1, 1, 0,
        1, 1, 0,

        1, 1, 0,
        1, 1, 0,
        1, 1, 0,

        0, 1, 1,
        0, 1, 1,
        0, 1, 1,

        0, 1, 1,
        0, 1, 1,
        0, 1, 1,

        1, 0, 1,
        1, 0, 1,
        1, 0, 1,

    };

    GLuint VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sun), sun, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint c1;
    glGenBuffers(1, &c1);
    glBindBuffer(GL_ARRAY_BUFFER, c1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(clrs), clrs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, c1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    GLuint VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(earth), earth, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    GLuint c2;
    glGenBuffers(1, &c2);
    glBindBuffer(GL_ARRAY_BUFFER, c2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(clrs), clrs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, c2);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint VAO3;
    glGenVertexArrays(1, &VAO3);
    glBindVertexArray(VAO3);

    GLuint VBO3;
    glGenBuffers(1, &VBO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(moon), moon, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint c3;
    glGenBuffers(1, &c3);
    glBindBuffer(GL_ARRAY_BUFFER, c3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(clrs), clrs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, c3);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float day = 0, inc = 1.0f / 24;

    while (!glfwWindowShouldClose(window))
    {
        unsigned int ID = glGetUniformLocation(shaderProgram, "MVP");

        processInput(window);

        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float sunSelf = get_sun_rotate_angle_around_itself(day);
        float earthSun = get_earth_rotate_angle_around_sun(day);
        float earthSelf = get_earth_rotate_angle_around_itself(day);

        float moonEarth = get_moon_rotate_angle_around_earth(day);
        float moonSelf = get_moon_rotate_angle_around_itself(day);

        //model 1, sun

        glm::mat4 t1 = glm::mat4(1.0f);

        //rotate about self
        t1 = glm::rotate(t1, glm::radians(sunSelf), glm::vec3(0.0, 1.0, 0.0));

        //model 2, earth
        glm::mat4 t2 = glm::mat4(1.0f);

        float eX = 20.0f * glm::cos(glm::radians(get_earth_rotate_angle_around_sun(day)));
        float eZ = -20.0f * glm::sin(glm::radians(get_earth_rotate_angle_around_sun(day)));

        t2 = glm::translate(t2, glm::vec3(eX, 0.0, eZ));

        t2 = glm::rotate(t2, glm::radians(-23.4f), glm::vec3(0.0, 0.0, 1.0));
        t2 = glm::rotate(t2, glm::radians(earthSelf), glm::vec3(0.0, 1.0, 0.0));

        //model 3, moon
        glm::mat4 t3 = glm::mat4(1.0f);

        float mX = eX + 10.0f * glm::sin(glm::radians(get_moon_rotate_angle_around_earth(day)));
        float mZ = eZ - 10.0f * glm::cos(glm::radians(get_moon_rotate_angle_around_earth(day)));

        //rotate in relation to Sun
        t3 = glm::rotate(t3, glm::radians(earthSun), glm::vec3(0.0, 1.0, 0.0));
        //translate to Earth's position
        t3 = glm::translate(t3, glm::vec3(20, 0, 0));

        //rotate in relation to Earth
        t3 = glm::rotate(t3, glm::radians(moonEarth), glm::vec3(0.0, 1.0, 0.0));
        //translate to Moon's position
        t3 = glm::translate(t3, glm::vec3(10, 0, 0));

        // projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

        glm::vec3 sunPos = glm::vec3(0, 0, 0);
        glm::vec3 earthPos = glm::vec3(eX, 0, eZ);
        glm::vec3 moonPos = glm::vec3(mX, 0, mZ);

        //camera matrix
        glm::mat4 view = glm::lookAt(
            glm::vec3(50, 50, 100), // camera location
            sunPos,      //look at coordinates
            //earthPos,
            //moonPos,
            glm::vec3(0, 1, 0)  // up axis
        );

        glm::mat4 model1 = projection * view * t1;
        glm::mat4 model2 = projection * view * t2;
        glm::mat4 model3 = projection * view * t3;


        //draw model 1
        glBindVertexArray(VAO1);
        glUniformMatrix4fv(ID, 1, GL_FALSE, glm::value_ptr(model1));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);


        //draw model 2
        glBindVertexArray(VAO2);
        glUniformMatrix4fv(ID, 1, GL_FALSE, glm::value_ptr(model2));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        //draw model 3
        glBindVertexArray(VAO3);
        glUniformMatrix4fv(ID, 1, GL_FALSE, glm::value_ptr(model3));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        day += inc;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //release resource

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &c1);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &c2);

    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &VBO3);
    glDeleteBuffers(1, &c3);

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}