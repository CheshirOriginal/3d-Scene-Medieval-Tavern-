#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

/// <summary>
/// ��������� � ��������� ���������� �������� ���������
/// </summary>
struct DirLight
{
    /// <summary>
    /// �����������
    /// </summary>
    glm::vec3 direction;
    //��������� �����
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
/// <summary>
/// ��������� � ��������� ��������� ��������� �����
/// </summary>
struct PointLight
{
    /// <summary>
    /// ����������
    /// </summary>
    glm::vec3 position;
    //���������
    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
/// <summary>
/// ��������� � ��������� ������������� ��������� �����
/// </summary>
struct SpotLight
{
    //�������������� � �����������
    glm::vec3 position;
    glm::vec3 direction;
    //������ ����
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

/// <summary>
/// ��������� � ��������� ���������� ����� �� �����
/// </summary>
struct LightsInfo {
    /// <summary>
    /// ��������� �������� ���������
    /// </summary>
    DirLight dirLight;
    /// <summary>
    /// ������ � ��������� ����������� �����
    /// </summary>
    std::vector<PointLight> pointLights;
    /// <summary>
    /// ������ � ������������� ����������� �����
    /// </summary>
    std::vector<SpotLight> spotLights;

    //���������� �������� � ������������ ���������� �����
    int numPLights;
    int numSLights;

};

struct Mesh {
    /*std::vector<glm::vec3> vertices_coords;
    std::vector<glm::vec3> vertices_normals;
    std::vector<glm::vec2> textures_coords;*/
    long vertices_count;
    GLuint textureID = 0;
    GLuint VAO = 0;
};

class SuperModel
{
public:
    SuperModel(const std::string& objPath, const std::string& mtlPath);
    ~SuperModel() {};
    void render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix, LightsInfo lights, GLuint mode);
private:
    GLuint LoadTexture(const std::string& filename);
    void load_coords(glm::vec3* verteces, size_t count, Mesh& mesh);
    void load_normals(glm::vec3* normals, size_t count, Mesh& mesh);
    void load_texcoord(glm::vec2* tex, size_t count, Mesh& mesh);
    void load_shaders(const char* vect, const char* frag);
private:
    std::vector<Mesh> meshes; // ������ ��� �������� ���� ������ ������ (�����)
    GLuint shader_programme = -1;
};

