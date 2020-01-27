/**
* @file Camera.h
*/
#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include <GL/glew.h>
#include <glm/glm.hpp>
#undef far
#undef near

/**
* �J����.
*/
struct Camera
{
  glm::vec3 target = glm::vec3(100, 0, 100);
  glm::vec3 position = glm::vec3(100, 50, 150);
  glm::vec3 up = glm::vec3(0, 1, 0);
  glm::vec3 velocity = glm::vec3(0);

  // ��ʃp�����[�^.
  float width = 0;
  float height = 0;
  float near = 1.0f;
  float far = 500.0f;

  // �ύX�\�ȃJ�����p�����[�^.
  float fNumber = 1.4f;
  float fov = glm::radians(60.0f);
  float sensorSize = 36.0f;

  // Update�֐��Ōv�Z����J�����p�����[�^.
  float focalLength = 50.0f;
  float aperture = 20.0f;
  float focalPlane = 10000.0f;

  void Update(const glm::mat4& matView);
};

#endif // CAMERA_H_INCLUDED
