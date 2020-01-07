/**
* @file Particle.h
*/
#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED
#include <GL/glew.h>
#include "Texture.h"
#include "BufferObject.h"
#include "Shader.h"
#include "Sprite.h"
#include <glm/glm.hpp>
#include <vector>
#include <list>
#include <memory>

class Particle;
class ParticleEmitter;
using ParticleEmitterPtr = std::shared_ptr<ParticleEmitter>;
class ParticleSystem;

/**
* ���q.
*/
class Particle {
public:
  friend ParticleSystem;
  friend ParticleEmitter;

  Particle(const glm::vec3& pos, const glm::vec3& v, float lifetime) : position(pos), velocity(v), lifetime(lifetime) {};
  virtual ~Particle() = default;

  void Update(float deltatime);

  // ���W�̐ݒ�E�擾
  void Position(const glm::vec3& p) { position = p; }
  const glm::vec3& Position() const { return position; }

  void Velocity(const glm::vec3& v) { velocity = v; }
  const glm::vec3& Velocity() const { return velocity; }

  void Acceleration(const glm::vec3& a) { acceleration = a; }
  const glm::vec3& Acceleration() const { return acceleration; }

  // ��]�̐ݒ�E�擾
  void Rotation(float r) { rotation = r; }
  float Rotation() const { return rotation; }

  // �g�嗦�̐ݒ�E�擾
  void Scale(const glm::vec2& s) { scale = s; }
  const glm::vec2& Scale() const { return scale; }

  // �F�̐ݒ�E�擾
  void Color(const glm::vec4& c) { color = c; }
  const glm::vec4& Color() const { return color; }

  // ��`�̐ݒ�E�擾
  void Rectangle(const Rect& r) { rect = r; }
  const Rect& Rectangle() const { return rect; }

  bool IsDead() const { return lifetime <= 0; }

private:
  glm::vec3 position = glm::vec3(0);
  glm::vec3 velocity = glm::vec3(0);
  glm::vec3 acceleration = glm::vec3(0);
  float lifetime = 0;

  glm::vec2 scale = glm::vec2(1);
  float rotation = 0;
  glm::vec4 color = glm::vec4(1);
  Rect rect = { glm::vec2(0), glm::vec2(0) };
};

/**
* �p�[�e�B�N�����o�N���X.
*/
class ParticleEmitter
{
public:
  friend ParticleSystem;

  ParticleEmitter(const char* imagePath, const glm::vec3& pos, float duration, bool loop, float eps);
  ~ParticleEmitter() = default;

  void Update(float deltatime);
  void Draw();

  // ���W�̐ݒ�E�擾
  void Position(const glm::vec3& p) { position = p; }
  const glm::vec3& Position() const { return position; }

  // ��]�̐ݒ�E�擾
  void Rotation(const glm::vec3& r) { rotation = r; }
  const glm::vec3& Rotation() const { return rotation; }

  void Angle(float a) { angle = a; }
  float Angle() const { return angle; }

  void Radius(float r) { radius = r; }
  float Radius() const { return radius; }

  void Gravity(float g) { gravity = g; }
  float Gravity() const { return gravity; }

  void Id(int n) { id = n; }
  int Id() const { return id; }

  bool IsDead() const { return !loop && timer >= duration && particles.empty(); }

private:
  void AddParticle();

  int id = 0;
  glm::vec3 position = glm::vec3(0);
  glm::vec3 rotation = glm::vec3(0);
  float duration = 1.0f; // �p�[�e�B�N������o����b��.
  bool loop = true; // duration�o�ߌ�Ƀ��[�v�Đ�����Ȃ�true�A��~����Ȃ�false.
  float emissionsPerSecond = 3.0f; // �b������̃p�[�e�B�N�����o��.
  float angle = 0.5f; // �~���̊p�x.
  float radius = 0.3f; // �~���̔��a.
  float gravity = 9.8f; // �d��.
  Texture::Image2DPtr texture; // �p�[�e�B�N���p�e�N�X�`��.

  float interval = 0;
  float timer = 0;
  float emissionTimer = 0;

  size_t count = 0;
  size_t baseVertex = 0;

  std::list<Particle>  particles;
};

/**
* �p�[�e�B�N���Ǘ��N���X.
*/
class ParticleSystem
{
public:
  ParticleSystem() = default;
  ~ParticleSystem() = default;

  bool Init(size_t maxParticleCount);
  void Add(const ParticleEmitterPtr& p);
  void Clear();
  void Update(float deltatime);
  void Draw(const glm::mat4& matProj, const glm::mat4& matView);

private:
  BufferObject vbo;
  BufferObject ibo;
  VertexArrayObject vao;
  Shader::ProgramPtr program;

  std::list<ParticleEmitterPtr> emitters;
};

#endif // PARTICLE_H_INCLUDED
