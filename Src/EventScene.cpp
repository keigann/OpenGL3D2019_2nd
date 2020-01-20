/**
* @file EventScene.cpp
*/
#include "EventScene.h"
#include "EventScript.h"
#include <fstream>
#include <sstream>

/**
* �R���X�g���N�^.
*
* @param filename �X�N���v�g�t�@�C����.
*/
EventScene::EventScene(const char* filename) :
  Scene("EventScene"), filename(filename)
{
}

/**
*
*/
bool EventScene::Initialize()
{
  return EventScriptEngine::Instance().Execute(filename.c_str());
}

/**
*
*
* �X�N���v�g�ꗗ:
* - TEXT                ���͂�\�����A�\���I����҂�.
* - WAIT                �L�[���͑҂�.
* - IF a op b �` ENDIF  a��b����r���Z�qop�𖞂����Ȃ�ENDIF�܂ł����s.
* - a = b op c              a��b�������Z�qop�ɏ]���đ��.
*
* 100��double�^�ϐ���p��. �X�N���v�g���ł�'var0'�`'var99'�̖��O�ŎQ�Ƃł���. �����l��0.
* 
*
*/
void EventScene::Update(float deltaTime)
{
  if (EventScriptEngine::Instance().IsFinished()) {
    SceneStack::Instance().Pop();
  }
}

