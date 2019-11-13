/**
* @file FrameBufferObject.h
*/
#ifndef FRAMEBUFFEROBJECT_H_INCLUDED
#define FRAMEBUFFEROBJECT_H_INCLUDED
#include <GL/glew.h>
#include "Texture.h"
#include <memory>

class FrameBufferObject;
typedef std::shared_ptr<FrameBufferObject> FrameBufferObjectPtr;

/**
* �t���[���o�b�t�@�̎��.
*/
enum class FrameBufferType {
  colorAndDepth,
  colorOnly,
  depthOnly,
};

/**
* �t���[���o�b�t�@�I�u�W�F�N�g.
*/
class FrameBufferObject
{
public:
  static FrameBufferObjectPtr Create(int w, int h, GLenum internalFormat = GL_RGBA8, FrameBufferType type = FrameBufferType::colorAndDepth);
  FrameBufferObject() = default;
  ~FrameBufferObject();

  GLuint GetFramebuffer() const;
  const Texture::Image2DPtr& GetColorTexture() const;
  const Texture::Image2DPtr& GetDepthTexture() const;

private:
  Texture::Image2DPtr texColor; ///< �t���[���o�b�t�@�p�e�N�X�`��.
  Texture::Image2DPtr texDepth; ///< �t���[���o�b�t�@�p�e�N�X�`��.
  GLuint id= 0; ///< �t���[���o�b�t�@�I�u�W�F�N�g.
};

#endif // FRAMEBUFFEROBJECT_H_INCLUDED
