/**
* @file EventScript.cpp
*/
#define _CRT_SECURE_NO_WARNINGS
#include "EventScript.h"
#include "GLFWEW.h"
#include <fstream>
#include <sstream>
#include <iostream>

/**
* �X�N���v�g�G���W���̃V���O���g���E�C���X�^���X���擾����.
*
* @return �X�N���v�g�G���W���̃V���O���g���E�C���X�^���X.
*/
EventScriptEngine& EventScriptEngine::Instance()
{
  static EventScriptEngine instance;
  return instance;
}

/**
* �X�N���v�g�G���W��������������.
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool EventScriptEngine::Init(int maxVariableCount)
{
  if (isInitialized) {
    std::cerr << "[�G���[] EventScriptEngine�͊��ɏ���������Ă��܂�.\n";
    return false;
  }

  variables.resize(maxVariableCount, 0);
  filename.reserve(256);
  script.reserve(2048);
  if (!textWindow.Init("Res/TextWindow.tga", glm::vec2(0, -248), glm::vec2(48, 32), glm::vec2(0))) {
    std::cerr << "[�G���[]" << __func__ << ":�X�N���v�g�G���W���̏������Ɏ��s.\n";
    return false;
  }
  isInitialized = true;
  return true;
}

/**
* �C�x���g�E�X�N���v�g�����s����.
*
* @param filename �X�N���v�g�E�t�@�C����.
*
* @retval true  ���s�ɐ���.
* @retval false ���s�Ɏ��s.
*/
bool EventScriptEngine::RunScript(const char* filename)
{
  if (!isInitialized) {
    return false;
  }

  std::ifstream ifs(filename);
  if (!ifs) {
    std::cerr << "[�G���[]" << __func__ << ":�X�N���v�g�t�@�C��" << filename << "��ǂݍ��߂܂���.\n";
    return false;
  }
  std::stringstream ss;
  ss << ifs.rdbuf();
  std::string tmp = ss.str();
  setlocale(LC_CTYPE, "ja-JP");
  const size_t size = mbstowcs(nullptr, tmp.c_str(), 0);
  script.resize(size + 1);
  mbstowcs(&script[0], tmp.c_str(), size);

  this->filename = filename;
  isFinished = false;
  textWindow.Open(script.c_str());

  std::cout << "[INFO]" << __func__ << ":�X�N���v�g�t�@�C��" << filename << "�����s.\n";
  return true;
}

/**
* �X�N���v�g�G���W���̏�Ԃ��X�V����.
*
* @param deltaTime  �O��̍X�V����̌o�ߎ���(�b).
*/
void EventScriptEngine::Update(float deltaTime)
{
  if (!isInitialized) {
    return;
  }

  if (textWindow.IsFinished()) {
    const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
    if (gamepad.buttonDown & (GamePad::A | GamePad::B | GamePad::START)) {
      textWindow.Close();
      isFinished = true;
    }
  }
  textWindow.Update(deltaTime);
}

/**
* �X�N���v�g�G���W����`�悷��.
*/
void EventScriptEngine::Draw()
{
  if (!isInitialized) {
    return;
  }

  textWindow.Draw();
}

/**
* �X�N���v�g�ϐ��ɒl��ݒ肷��.
*
* @param no    �ϐ��ԍ�(0�`���������ɐݒ肵���ő吔).
* @param value �ݒ肷��l.
*/
void EventScriptEngine::SetVariable(int no, double value)
{
  if (no < 0 || no >= static_cast<int>(variables.size())) {
    return;
  }
  variables[no] = value;
}

/**
* �X�N���v�g�ϐ��̒l���擾����.
*
* @param no    �ϐ��ԍ�(0�`���������ɐݒ肵���ő吔).
*
* @return no�Ԃ̕ϐ��ɐݒ肳��Ă���l.
*/
double EventScriptEngine::GetVariable(int no) const
{
  if (no < 0 || no >= static_cast<int>(variables.size())) {
    return 0;
  }
  return variables[no];
}

/**
* �X�N���v�g�̎��s���������������ׂ�.
*
* @retval true  ���s����.
* @retval false ���s���A�܂��̓X�N���v�g���ǂݍ��܂�Ă��Ȃ�.
*/
bool EventScriptEngine::IsFinished() const
{
  return isFinished;
}

