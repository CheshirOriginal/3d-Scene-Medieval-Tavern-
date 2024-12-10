#pragma once

#include <glm/glm.hpp>

//Набор глобальных переменных и констант для управления сценой

#define PI 3.14159265358979323846f  

/// <summary>
/// Единичная матрица. Для быстрого сброса значений.
/// </summary>
extern const glm::mat4 WorldMatrix;

/// <summary>
/// Ширина окна.
/// </summary>
extern int WinWidth;
/// <summary>
/// Высота окна.
/// </summary>
extern int WinHeight;

/// <summary>
/// Направление вращения модели.
/// </summary>
extern bool RotationMode;

/// <summary>
/// Позиция камеры
/// </summary>
extern glm::vec3 CamPosition;
extern glm::vec3 cameraFront;
extern float yaw;
extern float pitch;
extern float sensitivity;
extern float lastX;
extern float lastY;
extern bool firstMouse;
/// <summary>
/// Позиция цели камеры
/// </summary>
//extern glm::vec3 CamTarget;
/// <summary>
/// Двигалась ли камера после предыдущего кадра
/// </summary>
extern bool CamMoved;
/// <summary>
/// Скорость камеры
/// </summary>
extern GLfloat CamSpeed;
extern float angle;
extern bool useMipmaps;
//extern GLuint texture;

