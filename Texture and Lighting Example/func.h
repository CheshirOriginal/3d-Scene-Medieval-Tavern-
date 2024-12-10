#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>


using namespace std;

void UpdateTextureFiltering();

/// <summary>
/// Загрузка шейдера из файлаю
/// </summary>
/// <param name="filename">Путь к файлу.</param>
/// <returns>Строка с кодом шейдера.</returns>
string LoadShader(const char* filename);

/// <summary>
/// Функция инициализации всего и создания окна.
/// </summary>
/// <param name="w">Ширина окна.</param>
/// <param name="h">Высота окна.</param>
/// <param name="Fullscreen">Режим полного экрана. </param>
/// <returns>Указатель на созданное окно.</returns>
GLFWwindow* InitAll(int w = 640, int h = 480, bool Fullscreen = false);
/// <summary>
/// Функция для корректного завершения программы.
/// </summary>
void EndAll();

//Набор функций для обработки событий

/// <summary>
/// Обработчик изменения размера окна.
/// </summary>
/// <param name="window">Указатель на окно.</param>
/// <param name="width">Новая ширина окна.</param>
/// <param name="height">Новая высота окна.</param>
void window_size_callback(GLFWwindow* window, int width, int height);
/// <summary>
/// Обработчик нажатия клавиш.
/// </summary>
/// <param name="window">Указатель на окно.</param>
/// <param name="key">Код клавиши.</param>
/// <param name="scancode">Код символа.</param>
/// <param name="action">Действие.</param>
/// <param name="mods">Состояние модификаторов нажатия.</param>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
/// <summary>
/// Загрузка OBJ файла
/// </summary>
/// <param name="path">Путь к файлу.</param>
/// <param name="outVertices">Выходной массив координат вершин.</param>
/// <param name="outTextures">Выходной массив текстурных координат.</param>
/// <param name="outNormals">Выходной массив нормалей.</param>
/// <returns>Успешна ли загрузка (true/false)</returns>
bool loadOBJ(
    const char* path,
    std::vector < glm::vec3 >& outVertices,
    std::vector < glm::vec2 >& outTextures,
    std::vector < glm::vec3 >& outNormals
);
/// <summary>
/// Генерация вершин сферы
/// </summary>
/// <param name="outIndexes">Выходной массив индексов.</param>
/// <param name="outVertices">Выходной массив координат вершин.</param>
/// <param name="outTextures">Выходной массив текстурных координат.</param>
/// <param name="outNormals">Выходной массив нормалей.</param>
/// <param name="radius">Радиус сферы.</param>
/// <param name="sectorCount">Число меридианов.</param>
/// <param name="stackCount">Число параллелей.</param>
void genSphere(
    std::vector < GLuint >& outIndexes,
    std::vector < glm::vec3 >& outVertices,
    std::vector < glm::vec2 >& outTextures,
    std::vector < glm::vec3 >& outNormals,
    float radius,
    int sectorCount,
    int stackCount
);
/// <summary>
/// Генерация цилиндра.
/// </summary>
/// <param name="outIndexes">Выходной массив индексов.</param>
/// <param name="outVertices">Выходной массив координат вершин.</param>
/// <param name="outTextures">Выходной массив текстурных координат.</param>
/// <param name="outNormals">Выходной массив нормалей.</param>
/// <param name="baseRadius">Радиус основания.</param>
/// <param name="topRadius">Радиус вершины.</param>
/// <param name="height">Высота цилиндра.</param>
/// <param name="sectorCount">Число граней.</param>
/// <param name="stackCount">Число секций.</param>
void genCylinder(
    std::vector < GLuint >& outIndexes,
    std::vector < glm::vec3 >& outVertices,
    std::vector < glm::vec2 >& outTextures,
    std::vector < glm::vec3 >& outNormals,
    float baseRadius,
    float topRadius,
    float height,
    int sectorCount,
    int stackCount
);
/// <summary>
/// Вспомогательная функция для вычисления относительных координат секции цилиндра.
/// </summary>
/// <param name="sectorCount">Число граней.</param>
/// <returns></returns>
std::vector < glm::vec3 > getUnitCircleVertices(
    int sectorCount
);
/// <summary>
/// Генерация тора.
/// </summary>
/// <param name="outIndexes">Выходной массив индексов.</param>
/// <param name="outVertices">Выходной массив координат вершин.</param>
/// <param name="outTextures">Выходной массив текстурных координат.</param>
/// <param name="outNormals">Выходной массив нормалей.</param>
/// <param name="outerRadius">Внешний радиус.</param>
/// <param name="tubeRadius">Радиус самого тора.</param>
/// <param name="sectorCount">Число граней.</param>
/// <param name="stackCount">Число секций.</param>
void genTorus(
    std::vector < GLuint >& outIndexes,
    std::vector < glm::vec3 >& outVertices,
    std::vector < glm::vec2 >& outTextures,
    std::vector < glm::vec3 >& outNormals,
    float outerRadius,
    float tubeRadius,
    int sectorCount,
    int stackCount
);
