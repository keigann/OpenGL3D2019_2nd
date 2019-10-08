/**
* @file MainGameScene.h
*/
#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED
#include "Scene.h"
#include "Sprite.h"
#include "Font.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Actor.h"
#include "PlayerActor.h"
#include "JizoActor.h"
#include "Light.h"
#include <random>
#include <vector>

/**
* ���C���Q�[�����.
*/
class MainGameScene : public Scene
{
public:
  MainGameScene() : Scene("MainGameScene") {}
  virtual ~MainGameScene() = default;

  virtual bool Initialize() override;
  virtual void ProcessInput() override;
  virtual void Update(float) override;
  virtual void Render() override;
  virtual void Finalize() override {}

  bool HandleJizoEffects(int id, const glm::vec3& pos);

private:
  std::mt19937 rand;
  int jizoId = -1; ///< ���ݐ퓬���̂��n���l��ID.
  bool achivements[4] = { false, false, false, false }; ///< �G�������.
  bool isClear = false;
  float clearTimer = 0;

  std::vector<Sprite> sprites;
  SpriteRenderer spriteRenderer;
  FontRenderer fontRenderer;
  Mesh::Buffer meshBuffer;
  Terrain::HeightMap heightMap;

  struct Camera {
    glm::vec3 target = glm::vec3(100, 0, 100);
    glm::vec3 position = glm::vec3(100, 50, 150);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 velocity = glm::vec3(0);
  };
  Camera camera;

  PlayerActorPtr player;
  ActorList enemies;
  ActorList trees;
  ActorList objects;

  LightBuffer lightBuffer;
  ActorList lights;
};

#endif // MAINGAMESCENE_H_INCLUDED