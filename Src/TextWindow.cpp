/**
* @file TextWindow.cpp
*/
#include "TextWindow.h"
#include "GLFWEW.h"
#include <iostream>

/**
* �e�L�X�g�E�B���h�E������������.
*
* @param imagePath      �E�B���h�E�̉摜�t�@�C����.
* @param position       �E�B���h�E�̕\���ʒu(�h�b�g).
* @param textAreaMargin �E�B���h�E�T�C�Y�ƃe�L�X�g�\���̈�̊Ԋu(�h�b�g).
* @param textAreaOffset �e�L�X�g�\���̈�̈ʒu�����炷�l(�h�b�g).
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool TextWindow::Init(const char* imagePath, const glm::vec2& position,
  const glm::vec2& textAreaMargin, const glm::vec2& textAreaOffset)
{
  this->position = position;
  this->textAreaOffset = textAreaOffset;
  this->textAreaMargin = textAreaMargin;
  sprBackground = Sprite(Texture::Image2D::Create(imagePath));
  if (!spriteRenderer.Init(100, "Res/Sprite.vert", "Res/Sprite.frag")) {
    std::cerr << "[�G���[]" << __func__ << ": �e�L�X�g�E�B���h�E�̏������Ɏ��s.\n";
    return false;
  }
  if (!fontRenderer.Init(1000)) {
    std::cerr << "[�G���[]" << __func__ << ": �e�L�X�g�E�B���h�E�̏������Ɏ��s.\n";
    return false;
  }
  if (!fontRenderer.LoadFromFile("Res/font.fnt")) {
    std::cerr << "[�G���[]" << __func__ << ": �e�L�X�g�E�B���h�E�̏������Ɏ��s.\n";
    return false;
  }
  return true;
}

/**
* �e�L�X�g�E�B���h�E�̏�Ԃ��X�V����.
*
* @param deltaTime  �O��̍X�V����̌o�ߎ���(�b).
*/
void TextWindow::Update(float deltaTime)
{
  if (!isOpen) {
    return;
  }

  // �w�i�摜�̍X�V.
  sprBackground.Position(glm::vec3(position, 0));
  spriteRenderer.BeginUpdate();
  spriteRenderer.AddVertices(sprBackground);
  spriteRenderer.EndUpdate();

  // ���ׂĂ̕������\���ς�(outputCount���e�L�X�g�������ȏ�)�Ȃ�
  // �t�H���g���X�V����K�v�͂Ȃ�.
  if (outputCount >= static_cast<int>(text.size())) {
    return;
  }

  // �����\���Ԋu��0���傫���Ȃ�A1�������\������.
  // 0�ȉ��Ȃ��C�ɑS����\������.
  if (interval > 0) {
    outputTimer += deltaTime;
    const int c = static_cast<int>(outputTimer / interval);
    if (c == 0) {
      return;
    }
    outputCount += c;
    outputTimer -= static_cast<float>(c) * interval;
  } else {
    outputCount = text.size();
  }

  // ���͕\���̈�̃T�C�Y���v�Z.
  const Texture::Image2DPtr tex = sprBackground.Texture();
  const glm::vec2 windowSize = glm::vec2(tex->Width(), tex->Height());
  const glm::vec2 textAreaSize = windowSize - textAreaMargin * 2.0f;

  // ���݂܂łɏo�͂���ׂ��������s�P�ʂŎ擾.
  std::vector<std::wstring> rowList;
  float lineWidth = 0; // �s�̉���.
  int outputOffset = 0; // �����ς݂̕�����.
  for (int i = 0; i < outputCount; ++i) {
    lineWidth += fontRenderer.XAdvance(text[i]);
    // ���s�������A�s�̕����e�L�X�g�\���̈�̕��𒴂�����1�s�\�����ĉ��s.
    if (text[i] == L'\n' || lineWidth > textAreaSize.x) {
      rowList.push_back(text.substr(outputOffset, i - outputOffset));
      outputOffset = i;
      lineWidth = 0;
    }
  }
  // �Ō�̍s�ɕ������c���Ă�����A�����\��.
  if (outputOffset < outputCount) {
    rowList.push_back(text.substr(outputOffset, outputCount - outputOffset));
  }

  // �e�L�X�g�̈�̃p�����[�^�[���l�����āA�ŏ��̕����̈ʒu(offset)���v�Z����.
  glm::vec2 offset = textAreaSize * glm::vec2(-0.5f, 0.5f);
  offset += textAreaOffset;
  offset.y -= fontRenderer.LineHeight();
  offset += position;

  // �\���J�n�s�����߂�.
  const int maxLines = static_cast<int>(textAreaSize.y / fontRenderer.LineHeight());
  int startLine = static_cast<int>(rowList.size()) - maxLines;
  if (startLine < 0) {
    startLine = 0;
  }

  // ���͂�\��.
  fontRenderer.BeginUpdate();
  fontRenderer.Color(glm::vec4(0, 0, 0, 1));
  for (int i = startLine; i < static_cast<int>(rowList.size()); ++i) {
    fontRenderer.AddString(glm::vec3(offset, 0), rowList[i].c_str());
    offset.y -= fontRenderer.LineHeight();
  }
  fontRenderer.EndUpdate();
}

/**
* �e�L�X�g�E�B���h�E��`�悷��.
*/
void TextWindow::Draw()
{
  if (isOpen) {
    const GLFWEW::Window& window = GLFWEW::Window::Instance();
    const glm::vec2 screenSize(window.Width(), window.Height());
    spriteRenderer.Draw(screenSize);
    fontRenderer.Draw(screenSize);
  }
}

/**
* �e�L�X�g�E�B���h�E���J��.
*
* @param str �E�B���h�E�ɕ\�����镶��.
*/
void TextWindow::Open(const wchar_t* str)
{
  SetText(str);
  isOpen = true;
}

/**
* �e�L�X�g��ύX����.
*
* @param str �E�B���h�E�ɕ\�����镶��.
*/
void TextWindow::SetText(const wchar_t* str)
{
  text = str;
  outputCount = 0;
  outputTimer = 0;
}

/**
* �e�L�X�g�E�B���h�E�����.
*/
void TextWindow::Close()
{
  text.clear();
  outputCount = 0;
  outputTimer = 0;
  isOpen = false;
}

/**
* �e�L�X�g�̕\�����������������ׂ�.
*
* @retval true  ���ׂĕ�����\������.
* @retval false �܂��\�����Ă��Ȃ�����������.
*/
bool TextWindow::IsFinished() const
{
  return isOpen && (outputCount >= static_cast<int>(text.size()));
}

