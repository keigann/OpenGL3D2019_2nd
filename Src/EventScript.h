/**
* @file EventScript.h
*/
#ifndef EVENTSCRIPT_H_INCLUDED
#define EVENTSCRIPT_H_INCLUDED
#include "TextWindow.h"
#include "Camera.h"
#include <string>
#include <vector>
#include <variant>

/**
* �C�x���g�X�N���v�g����N���X.
*/
class EventScriptEngine
{
public:
static EventScriptEngine& Instance();

  bool Init(int maxVariableCount = 100);
  bool RunScript(const char* filename);
  void Update(float deltaTime);
  void Draw();

  bool IsFinished() const;

  void SetVariable(int no, double value);
  double GetVariable(int no) const;

  void SetCamera(Camera* p) { camera = p; }
  
private:
  EventScriptEngine() = default;
  ~EventScriptEngine() = default;
  EventScriptEngine(const EventScriptEngine&) = delete;
  EventScriptEngine& operator=(const EventScriptEngine&) = delete;

  std::string filename;

  // �@�\�R�[�h.
  enum class InstructionType {
    nop,
    print,
    beginIf,
    endIf,
    jump,
    assign,
    expression,
    select2,
    select3,
    setCameraPosition,
    setCameraTarget,
  };

  // ���Z�q.
  enum class Operator {
    equal,
    notEqual,
    less,
    lessEqual,
    greater,
    greaterEqual,

    add,
    sub,
    mul,
    div,
  };

  // �����̎��.
  using VariableId = int;
  using Number= double;
  using Text = std::wstring;
  using Argument = std::variant<VariableId, Number, Text, Operator>;
  void Set(Argument&, const char*);
  void SetOperator(Argument&, const char*);
  Number Get(const Argument&) const;

  struct Instruction {
    InstructionType type = InstructionType::nop;
    size_t jump = 0;
    Argument arguments[4];
  };
  std::vector<Instruction> script;
  std::vector<double> variables;
  size_t programCounter = 0;

  bool isInitialized = false; // �G���W��������������Ă����true.
  bool isFinished = false; // �X�N���v�g�̎��s���I��������true.

  TextWindow textWindow;
  Camera* camera = nullptr;
};

#endif // EVENTSCRIPT_H_INCLUDED

