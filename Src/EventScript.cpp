/**
* @file EventScript.cpp
*/
#define _CRT_SECURE_NO_WARNINGS
#include "EventScript.h"
#include "GLFWEW.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

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
* �X�N���v�g���߂̈����ɒl��ݒ肷��.
*
* @param arg �X�N���v�g���߂̈���.
* @param str �ݒ肷��l���܂ޕ�����.
*/
void EventScriptEngine::Set(Argument& arg, const char* s)
{
  if (s[0] == '[') {
    VariableId id;
    if (sscanf(s, "[%d]", &id) >= 1) {
      arg = id;
    }
  } else {
    Number n;
    if (sscanf(s, "%lf", &n) >= 1) {
      arg = n;
    }
  }
}

/**
* �X�N���v�g���߂̈�������l���擾����.
*
* @param arg �X�N���v�g���߂̈���.
*
* @return �������璼�ځE�Ԑڂɓ���ꂽ�l.
*/
EventScriptEngine::Number EventScriptEngine::Get(const Argument& arg) const
{
  if (const auto p = std::get_if<VariableId>(&arg)) {
    return variables[*p];
  } else if (const auto p = std::get_if<Number>(&arg)) {
    return *p;
  }
  return 0;
}

/**
* �X�N���v�g���߂̈����Ɋ֌W���Z�q��ݒ肷��.
*
* @param arg �X�N���v�g���߂̈���.
* @param str �ݒ肷��l���܂ޕ�����.
*/
void EventScriptEngine::SetOperator(Argument& arg, const char* str)
{
  if (str[1] == '\0') {
    switch (str[0]) {
    case '<': arg = Operator::less; break;
    case '>': arg = Operator::greater; break;
    case '+': arg = Operator::add; break;
    case '-': arg = Operator::sub; break;
    case '*': arg = Operator::mul; break;
    case '/': arg = Operator::div; break;
    default: /* �������Ȃ� */ break;
    }
  } else if (str[1] == '=' && str[2] == '\0') {
    switch (str[0]) {
    case '=': arg = Operator::equal; break;
    case '!': arg = Operator::notEqual; break;
    case '<': arg = Operator::lessEqual; break;
    case '>': arg = Operator::greaterEqual; break;
    default: /* �������Ȃ� */ break;
    }
  }
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

  script.clear();

  size_t lineCount = 0;
  std::string line;
  char buf[1000];
  char a[20], b[20], c[20], op[20];
  std::vector<size_t> stackIfElse;
  stackIfElse.reserve(100);
  while (std::getline(ifs, line)) {
    line.erase(0, line.find_first_not_of(" \t\n"));
    ++lineCount;

    Instruction inst;
    int n = sscanf(line.c_str(), "p %999[^\n]", buf);
    if (n >= 1) {
      const std::string s = std::regex_replace(buf, std::regex(R"(\\n)"), "\n");
      setlocale(LC_CTYPE, "ja-JP");
      const size_t size = mbstowcs(nullptr, s.c_str(), 0);
      std::wstring text(size, L'\0');
      mbstowcs(&text[0], s.c_str(), size);
      inst.type = InstructionType::print;
      inst.arguments[0] = text;
      script.push_back(inst);
      continue;
    }
    n = sscanf(line.c_str(), "set_camera_position %19s %19s %19s", a, b, c);
    if (n >= 3) {
      inst.type = InstructionType::setCameraPosition;
      Set(inst.arguments[0], a);
      Set(inst.arguments[1], b);
      Set(inst.arguments[2], c);
      script.push_back(inst);
      continue;
    }
    n = sscanf(line.c_str(), "set_camera_target %19s %19s %19s", a, b, c);
    if (n >= 3) {
      inst.type = InstructionType::setCameraTarget;
      Set(inst.arguments[0], a);
      Set(inst.arguments[1], b);
      Set(inst.arguments[2], c);
      script.push_back(inst);
      continue;
    }
    n = sscanf(line.c_str(), "[%19[^]]] = %19[^=!<>+-*/] %19[=!<>+-*/] %19[^=!<>+-*/]", a, b, op, c);
    if (n >= 4) {
      inst.type = InstructionType::expression;
      inst.arguments[0] = static_cast<VariableId>(atoi(a));
      Set(inst.arguments[1], b);
      SetOperator(inst.arguments[2], op);
      Set(inst.arguments[3], c);
      script.push_back(inst);
      continue;
    }
    n = sscanf(line.c_str(), "[%19[^]]] = %19s", a, b);
    if (n >= 2) {
      inst.type = InstructionType::assign;
      inst.arguments[0] = static_cast<VariableId>(atoi(a));
      Set(inst.arguments[1], b);
      script.push_back(inst);
      continue;
    }
    n = sscanf(line.c_str(), "if %19s %19s %19s", a, b, c);
    if (n >= 3) {
      inst.type = InstructionType::beginIf;
      Set(inst.arguments[0], a);
      SetOperator(inst.arguments[1], b);
      Set(inst.arguments[2], c);
      stackIfElse.push_back(script.size());
      script.push_back(inst);
      continue;
    }
    if (strncmp(line.c_str(), "endif", 5) == 0) {
      const size_t p = stackIfElse.back();
      stackIfElse.pop_back();
      script[p].jump = script.size();
      continue;
    }
    if (strncmp(line.c_str(), "else", 4) == 0) {
      const size_t posIf = stackIfElse.back();
      stackIfElse.back() = script.size();
      inst.type = InstructionType::jump;
      script.push_back(inst);
      script[posIf].jump = script.size();
      continue;
    }
  }

  this->filename = filename;
  programCounter = 0;
  isFinished = false;
  std::cout << "[INFO]" << __func__ << ":�X�N���v�g�t�@�C��" << filename << "�����s.\n";
  return true;
}

/**
* �X�N���v�g�G���W���̏�Ԃ��X�V����.
*
* @param deltaTime  �O��̍X�V����̌o�ߎ���(�b).
*
* 1�s��1����. ���s�ōs�̏I��.
* ���ׂĂ̕ϐ���double�^. �ϐ���[�ϐ��ԍ�]�Ə���.
*
* ���߈ꗗ:
* - p           �e�L�X�g�E�B���h�E�ɕ��͂�\��.
*               ������[�ϐ��ԍ�]�Ə����ƕϐ��̓��e���o�͂����.
* - if a op b   ��������. �������^�̂Ƃ�endif�܂Ŏ��s. op��=,!=,<,>,<=,>=�̂����ꂩ.
* - else        ��������̒���.
* - endif       ��������̏I�[.
* - a = b       �ϐ��̑��. a�͕ϐ��Ab�͕ϐ��܂��͐��l.
* - a = b op c   �l�����Z. a�͕ϐ��Ab, c�͕ϐ��܂��͐��l. op��+,-,*,/�̂����ꂩ.
* - select a b...N                �I�����̕\��. a�͕ϐ��Ab�`N�͕���.
* - spawn_actor name type pos rot �A�N�^�[���o��������.
* - kill_actor name               �A�N�^�[������.
* - move_actor name pos time      �A�N�^�[���ړ�������. 
* - wait time                     �w�肳�ꂽ���ԑҋ@.
* - # comment                     �s�R�����g.
*/
void EventScriptEngine::Update(float deltaTime)
{
  if (!isInitialized) {
    return;
  }

  if (script.empty() || isFinished) {
    return;
  }

  for (bool yield = false; !yield; ) {
    if (programCounter >= script.size()) {
      isFinished = true;
      return;
    }

    const auto& inst = script[programCounter];
    switch (inst.type) {
    case InstructionType::print:
      if (textWindow.IsOpen()) {
        if (textWindow.IsFinished()) {
          const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
          if (gamepad.buttonDown & (GamePad::A | GamePad::B | GamePad::START)) {
            textWindow.Close();
            ++programCounter;
            continue;
          }
        }
        yield = true;
      } else {
        const auto p = std::get_if<Text>(&inst.arguments[0]);
        if (!p) {
          std::cerr << "[�G���[]" << __func__ << "print���߂̈�����Text�^�łȂ��Ă͂Ȃ�܂���.\n";
          ++programCounter;
          break;
        }
        // ���͒��̕ϐ��ԍ��𐔒l�ɒu��������.
        const std::wregex re(LR"*(\[(\d+)\])*");
        std::wsregex_iterator itr(p->begin(), p->end(), re);
        std::wsregex_iterator end;
        if (itr != end) {
          std::wstring tmp;
          tmp.reserve(p->size());
          for (;;) {
            const auto& m = *itr;
            tmp += m.prefix();
            wchar_t* p = nullptr;
            const size_t n = static_cast<size_t>(std::wcstol(m[1].str().c_str(), &p, 10));
            if (n < variables.size()) {
              wchar_t buf[20];
              if (std::floor(variables[n]) == variables[n]) {
                swprintf(buf, 20, L"%d", static_cast<int>(variables[n]));
              } else {
                swprintf(buf, 20, L"%.2lf", variables[n]);
              }
              tmp += buf;
            }
            ++itr;
            if (itr == end) {
              tmp += m.suffix();
              break;
            }
          }
          textWindow.Open(tmp.c_str());
        } else {
          textWindow.Open(p->c_str());
        }
        yield = true;
      }
      break;

    case InstructionType::expression:
      if (const auto a = std::get_if<VariableId>(&inst.arguments[0])) {
        if (const auto op = std::get_if<Operator>(&inst.arguments[2])) {
          const Number b = Get(inst.arguments[1]);
          const Number c = Get(inst.arguments[3]);
          switch (*op) {
          case Operator::add: variables[*a] = b + c; break;
          case Operator::sub: variables[*a] = b - c; break;
          case Operator::mul: variables[*a] = b * c; break;
          case Operator::div: variables[*a] = b / c; break;
          }
        }
      }
      ++programCounter;
      break;

    case InstructionType::assign:
      if (const auto a = std::get_if<VariableId>(&inst.arguments[0])) {
        variables[*a] = Get(inst.arguments[1]);
      }
      ++programCounter;
      break;

    case InstructionType::beginIf:
      if (const auto op = std::get_if<Operator>(&inst.arguments[1])) {
        const Number a = Get(inst.arguments[0]);
        const Number b = Get(inst.arguments[2]);
        bool result = false;
        switch (*op) {
        case Operator::equal: result = a == b; break;
        case Operator::notEqual: result = a != b; break;
        case Operator::less: result = a < b; break;
        case Operator::lessEqual: result = a <= b; break;
        case Operator::greater: result = a > b; break;
        case Operator::greaterEqual: result = a >= b; break;
        }
        if (!result) {
          programCounter = inst.jump;
          break;
        }
      }
      ++programCounter;
      break;

    case InstructionType::jump:
      programCounter = inst.jump;
      break;

    case InstructionType::setCameraPosition:
      if (camera) {
        camera->position.x = static_cast<float>(Get(inst.arguments[0]));
        camera->position.y = static_cast<float>(Get(inst.arguments[1]));
        camera->position.z = static_cast<float>(Get(inst.arguments[2]));
      }
      ++programCounter;
      break;

    case InstructionType::setCameraTarget:
      if (camera) {
        camera->target.x = static_cast<float>(Get(inst.arguments[0]));
        camera->target.y = static_cast<float>(Get(inst.arguments[1]));
        camera->target.z = static_cast<float>(Get(inst.arguments[2]));
      }
      ++programCounter;
      break;

    default:
      ++programCounter;
      break;
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
* �X�N���v�g�̎��s���������������ׂ�.
*
* @retval true  ���s����.
* @retval false ���s���A�܂��̓X�N���v�g���ǂݍ��܂�Ă��Ȃ�.
*/
bool EventScriptEngine::IsFinished() const
{
  return isFinished;
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
