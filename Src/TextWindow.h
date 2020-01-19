/**
* @file TextWindow.h
*/
#ifndef TEXTWINDOW_H_INCLUDED
#define TEXTWINDOW_H_INCLUDED
#include "Sprite.h"
#include "Font.h"
#include <string>

/**
* �e�L�X�g�\���p�E�B���h�E.
*
* �w�肳�ꂽ�e�L�X�g��\������.
*/
class TextWindow
{
public:
  TextWindow() = default;
  ~TextWindow() = default;

  bool Init(const char* imagePath, const glm::vec2& scale, const glm::vec2& textAreaOffset, const glm::vec2& textAreaSize);
  void ProcessInput();
  void Update(float deltaTime);
  void Draw();

  void Open(const wchar_t*);
  void Close();

private:
  glm::vec3 position = glm::vec3(0); // �E�B���h�E����̈ʒu.
  glm::vec2 scale = glm::vec2(1); // �E�B���h�E�̃T�C�Y.
  glm::vec2 textAreaOffset = glm::vec2(-0.1f, -0.2f);
  glm::vec2 textAreaSize = glm::vec2(0.8f);

  std::wstring text; // �\������e�L�X�g.
  int outputCount = 0; // �o�͍ς݂̕�����.
  bool isOpen = false; // �E�B���h�E���J���Ă�����true.
  bool waitForInput = false; // ���͑҂��Ȃ�true. 

  float interval = 0.1f; // �����̕\���Ԋu(�b).
  float outputTimer = 0; // �����\���^�C�}�[(�b).

  SpriteRenderer spriteRenderer;
  Sprite sprBackground;

  FontRenderer fontRenderer;
};

#endif // TEXTWINDOW_H_INCLUDED