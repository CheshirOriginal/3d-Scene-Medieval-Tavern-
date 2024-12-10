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
/// Структура с описанием параметров фонового освещения
/// </summary>
struct DirLight
{
    /// <summary>
    /// Направление
    /// </summary>
    glm::vec3 direction;
    //Параметры света
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
/// <summary>
/// Структура с описанием точечного источника света
/// </summary>
struct PointLight
{
    /// <summary>
    /// Координаты
    /// </summary>
    glm::vec3 position;
    //Затухание
    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
/// <summary>
/// Структура с описанием направленного источника света
/// </summary>
struct SpotLight
{
    //Местоположение и направление
    glm::vec3 position;
    glm::vec3 direction;
    //Радиус луча
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
/// Структура с описанием источников света на сцене
/// </summary>
struct LightsInfo {
    /// <summary>
    /// Параметры фонового освещения
    /// </summary>
    DirLight dirLight;
    /// <summary>
    /// Массив с точечными источниками света
    /// </summary>
    std::vector<PointLight> pointLights;
    /// <summary>
    /// Массив с направленными источниками света
    /// </summary>
    std::vector<SpotLight> spotLights;

    //Количество точечных и направленных источников света
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
    std::vector<Mesh> meshes; // Вектор для хранения всех частей модели (мэшей)
    GLuint shader_programme = -1;
};

