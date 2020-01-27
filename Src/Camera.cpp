/**
* @file Camera.cpp
*/
#include "Camera.h"

/**
* �J�����̃p�����[�^���X�V����.
*
* @param matView �X�V�Ɏg�p����r���[�s��.
*/
void Camera::Update(const glm::mat4& matView)
{
  const float scale = -1000.0f;
  const glm::vec4 pos = matView * glm::vec4(target, 1);
  focalPlane = pos.z * scale;
#if 1
  const float imageDistance = sensorSize * 0.5f / glm::tan(fov * 0.5f); // �œ_����.
  //const float fov = 2.0f * glm::atan(sensorSize * 0.5f / imageDistance);
  focalLength = 1.0f / ((1.0f / focalPlane) + (1.0f / imageDistance));
#else
  // ��̎���ό`���ď��Z�����炵���o�[�W����.
  // `https://www.slideshare.net/siliconstudio/cedec-2010`�Œ񎦂���Ă��鎮.
  const float f = (focalPlane * sensorSize * 0.5f) / (glm::tan(fov * 0.5f) * focalPlane + sensorSize * 0.5f);
#endif
  aperture = focalLength / fNumber;
}
