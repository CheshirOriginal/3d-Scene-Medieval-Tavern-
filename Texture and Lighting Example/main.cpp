#include <thread>
#include <chrono>
#include "func.h"
#include "SuperModel.h"

#define PI 3.14159265358979323846f  


const glm::mat4 WorldMatrix = glm::mat4(1.0f);

const int TARGET_FPS = 60; // ������� FPS
const double FRAME_TIME = 1.0 / TARGET_FPS; // ����� ������ ����� � ��������


int WinWidth;
int WinHeight;

bool useMipmaps = true;
//GLuint texture = -1;

////��� ��� ������� � �������� ���� � ����������� �������
glm::vec3 CamPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 CamTarget = glm::vec3(0);
//
bool CamMoved = false;
//
GLfloat CamSpeed = 0.05;
// ���� ��� ���������� �������
float yaw = -90.0f;
float pitch = 0.0f;
// ��������� ���������������� ����
float sensitivity = 0.1f;
// �������� ���������� ������� ����
float lastX = 512; // ����� ���� (�� X)
float lastY = 384; // ����� ���� (�� Y)
// ���� ��� ��������� ������� �����
bool firstMouse = true; 

float angle = 0;

glm::vec3 baseDiffuse = glm::vec3(1.0, 0.8, 0.6);



int main() {
    srand(time(nullptr));

    GLFWwindow* window = InitAll(1920, 1080, false);
    if (window == nullptr)
    {
        EndAll();
        return -1;
    }
    glfwSwapInterval(0);

    //������� ��� �������� ��������� ������
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_textures;
    std::vector< glm::vec3 > temp_normals;
    std::vector< GLuint > temp_indices;
    float rotation = 0;
    float rotation_speed = -6;


    SuperModel model("taverna.obj", "taverna.mtl");

    SuperModel   rat("rat.obj", "");

    //��������� ��������� �����
    LightsInfo Lights;

    Lights.numPLights = 10;
    Lights.numSLights = 0;

    glm::vec3 la = glm::vec3(0);
    glm::vec3 ld = glm::vec3(0);
    glm::vec3 ls = { 0,0,0 };


    DirLight lSource;
    lSource.ambient = la;
    lSource.diffuse = ld;
    lSource.specular = ls;
    lSource.direction = glm::vec3(0, -1, 0);

    Lights.dirLight = lSource;

    //������
    PointLight P;
    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(0.562, 0.92, -0.3);
    P.constant = 1;
    P.linear = 3;
    P.quadratic = 10;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(-1.35, 0.92, 0.27);
    P.constant = 1;
    P.linear = 3;
    P.quadratic = 10;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(-1.35, 0.92, -1.08);
    P.constant = 1;
    P.linear = 3;
    P.quadratic = 10;
    Lights.pointLights.push_back(P);

    //����� 0.7 1.8
    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(-0.14, 0.95, 0.5);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(-0.11, 0.95, 0.38);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(0.11, 0.95, 0.39);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(0.06, 0.95, 0.6);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(-0.07, 0.95, 0.59);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(0.14, 0.95, 0.51);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);

    P.ambient = glm::vec3(0.22, 0.17, 0.12);
    P.diffuse = glm::vec3(1.0, 0.8, 0.6);
    P.specular = glm::vec3(1);
    P.position = glm::vec3(0, 0.95, 0.33);
    P.constant = 1;
    P.linear = 5;
    P.quadratic = 20;
    Lights.pointLights.push_back(P);




    //������ �������� ������
    glm::mat4 MMatr = WorldMatrix;
    glm::mat4 VMatr = WorldMatrix;
    glm::mat4 PMatr = WorldMatrix;

    //������� ������������� ����� ������ ����� ��, ������ � �� ���� �����
    PMatr = glm::perspective(
        glm::radians(67.0f), // ���� ������
        (float)WinWidth / WinHeight,       // ����������� ������
        0.01f,              // ������� ��������� ���������
        10.0f             // ������� ��������� ���������
    );

    //������ ��������� �������� ������� ����
    VMatr = glm::lookAt(CamPosition, CamPosition + cameraFront, glm::vec3(0, 1, 0));

    //���� ����
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //� ��� ��� ���� � 3d ����� �� ������ ��� ����� ������� (���������� ������ ���)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    //������ ���������
    while (!glfwWindowShouldClose(window)) {

        double startTime = glfwGetTime(); // �������� ������ �����
        rotation += rotation_speed;
        glViewport(0, 0, WinWidth, WinHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //���� ������ ������� - ���� ����������� ������� ����
        if (CamMoved)
        {
            VMatr = WorldMatrix;
            VMatr = glm::lookAt(CamPosition, CamPosition + cameraFront, glm::vec3(0, 1, 0));
            CamMoved = false;
        }

        for (auto& pointLight : Lights.pointLights) {
            if (rand() % 6 == 0) {
                float flicker = 0.9f + 0.1f * sin(glfwGetTime() * 10.0f + (rand() % 100 / 100.0f));
                pointLight.diffuse = baseDiffuse * flicker;
            }
        }

        MMatr = WorldMatrix;
        MMatr = glm::scale(MMatr, glm::vec3(0.1, 0.1, 0.1));
        model.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);


        MMatr = WorldMatrix;
        MMatr = glm::rotate(MMatr, glm::radians(rotation), glm::vec3(0.0, 1.0, 0.0));
        MMatr = glm::translate(MMatr, glm::vec3(0.1, 0.03, 0.0));
        rat.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);

        glfwPollEvents();
        glfwSwapBuffers(window);
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }

        double endTime = glfwGetTime();
        double elapsedTime = endTime - startTime;

        // ���� ���� ���������� ������� �������� �������, ����������� ���������
        if (elapsedTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - elapsedTime));
        }
    }

    return 0;
}