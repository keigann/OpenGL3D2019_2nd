/**
* @file Shader.cpp
*/
#include "Shader.h"
#include "Geometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdint.h>

/**
* �V�F�[�_�[�Ɋւ���@�\���i�[���閼�O���
*/
namespace Shader {
/**
* �V�F�[�_�[�E�v���O�������R���p�C������
*
* @param type �V�F�[�_�[�̎��
* @param string �V�F�[�_�[�E�v���O�����ւ̃|�C���^
*
* @retval 0 ���傫���@�쐬�����V�F�[�_�[�E�I�u�W�F�N�g
* @retval 0				�V�F�[�_�[�E�I�u�W�F�N�g�̍쐬�Ɏ��s
*/
GLuint Compile(GLenum type, const GLchar* string)
{
  if (!string) {
    return 0;
  }

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &string, nullptr);
  glCompileShader(shader);
  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  // �R���p�C���Ɏ��s�����ꍇ�A�������R���\�[���ɏo�͂��ĂO��Ԃ�
  if (!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      std::vector<char> buf;
      buf.resize(infoLen);
      if (static_cast<int>(buf.size()) >= infoLen) {
        glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
        std::cerr << "ERROR: �V�F�[�_�[�̃R���p�C���Ɏ��s.\n" << buf.data() << std::endl;
      }
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

/**
* �v���O�����E�I�u�W�F�N�g���쐬����
*
* @param vsCode ���_�V�F�[�_�[�E�v���O�����ւ̃|�C���^
* @param fsCode �t���O�����g�V�F�[�_�[�E�v���O�����ւ̃|�C���^
*
* @retval 0���傫�� �쐬�����v���O�����E�I�u�W�F�N�g
* @retval 0				�v���O�����E�I�u�W�F�N�g�̍쐬�Ɏ��s
*/
GLuint Build(const GLchar* vsCode, const GLchar* fsCode)
{
  GLuint vs = Compile(GL_VERTEX_SHADER, vsCode);
  GLuint fs = Compile(GL_FRAGMENT_SHADER, fsCode);
  if (!vs || !fs) {
    return 0;
  }
  GLuint program = glCreateProgram();
  glAttachShader(program, fs);
  glDeleteShader(fs);
  glAttachShader(program, vs);
  glDeleteShader(vs);
  glLinkProgram(program);
  GLint linkStatus = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus != GL_TRUE) {
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      std::vector<char> buf;
      buf.resize(infoLen);
      if (static_cast<int>(buf.size()) >= infoLen) {
        glGetProgramInfoLog(program, infoLen, NULL, buf.data());
        std::cerr << "ERROR: �V�F�[�_�[�����N�Ɏ��s.\n" << buf.data() << std::endl;
      }
    }
    glDeleteProgram(program);
    return 0;
  }
  return program;
}

/**
* �t�@�C����ǂݍ���
*
* @param path �ǂݍ��ރt�@�C����
*
* @return �ǂݍ��񂾃f�[�^
*/
std::vector<GLchar> ReadFile(const char* path)
{
  std::basic_ifstream<GLchar> ifs;
  ifs.open(path, std::ios_base::binary);
  if (!ifs.is_open()) {
    std::cerr << "ERROR: " << path << " ���J���܂���.\n";
    return{};
  }
  ifs.seekg(0, std::ios_base::end);
  const size_t length = (size_t)ifs.tellg();
  ifs.seekg(0, std::ios_base::beg);
  std::vector<GLchar> buf(length);
  ifs.read(buf.data(), length);
  buf.push_back('\0');
  return buf;
}
/**
* �t�@�C������v���O�����E�I�u�W�F�N�g���쐬����
*
* @param vsPath ���_�V�F�[�_�[�E�t�@�C����
* @param fsPath �t���O�����g�V�F�[�_�[�E�t�@�C����
*
* @return �쐬�����v���O�����E�I�u�W�F�N�g
*/
GLuint BuildFromFile(const char* vsPath, const char* fsPath)
{
  const std::vector<GLchar> vsCode = ReadFile(vsPath);
  const std::vector<GLchar> fsCode = ReadFile(fsPath);
  return Build(vsCode.data(), fsCode.data());
}

/**
* �R���X�g���N�^
*/
Program::Program()
{
}

/**
* �R���X�g���N�^
*/
Program::Program(GLuint programID)
{
  Reset(programID);
}


/**
* �f�X�g���N�^
*
* �v���O�����E�I�u�W�F�N�g���폜����
*/
Program::~Program()
{
  //if (id) {
  glDeleteProgram(id);
  //}
}

/**
* �v���O�����E�I�u�W�F�N�g��ݒ肷��.
*
* @param id �v���O�����E�I�u�W�F�N�g��ID.
*/
void Program::Reset(GLuint programId)
{
  glDeleteProgram(id);
  id = programId;
  if (id == 0) {
    locMatMVP = -1;
    locMatModel = -1;
    locPointLightCount = -1;
    locPointLightIndex = -1;
    locSpotLightCount = -1;
    locSpotLightIndex = -1;
    return;
  }

  locMatMVP = glGetUniformLocation(id, "matMVP");
  locMatModel = glGetUniformLocation(id, "matModel");
  locPointLightCount = glGetUniformLocation(id, "pointLightCount");
  locPointLightIndex = glGetUniformLocation(id, "pointLightIndex");
  locSpotLightCount = glGetUniformLocation(id, "spotLightCount");
  locSpotLightIndex = glGetUniformLocation(id, "spotLightIndex");

  const GLint texColorLoc = glGetUniformLocation(id, "texColor");
  if (texColorLoc >= 0) {
    glUseProgram(id);
    glUniform1i(texColorLoc, 0);
    glUseProgram(0);
  }
}

/**
* �v���O�����E�I�u�W�F�N�g���ݒ肳��Ă��邩���ׂ�.
*
* @retval true	�ݒ肳��Ă���.
* @retval false	�ݒ肳��Ă��Ȃ�
*/
bool Program::IsNull() const
{
  return id == 0;
}

/**
* �v���O�����E�I�u�W�F�N�g���O���t�B�b�N�X�E�p�C�v���C���Ɋ��蓖�Ă�
*/
void Program::Use()
{
  if (id) {
    glUseProgram(id);
  }
}

/**
* �`��Ɏg�p����e�N�X�`����ݒ肷��
*
* @param unitno �ݒ肷��e�N�X�`���E�C���[�W�E���j�b�g�̔ԍ��i�O�`�j
* @param texid  �ݒ肷��e�N�X�`���̂h�c
*/
void Program::BindTexture(GLuint unitNo, GLuint texId)
{
  glActiveTexture(GL_TEXTURE0 + unitNo);
  glBindTexture(GL_TEXTURE_2D, texId);
}

/**
* �`��Ɏg����r���[�E�v���W�F�N�V�����s���ݒ肷��.
*
* @param matVP �ݒ肷��r���[�E�v���W�F�N�V�����s��.
*/
void Program::SetViewProjectionMatrix(const glm::mat4& matVP)
{
  this->matVP = matVP;
  if (locMatMVP >= 0) {
    glUniformMatrix4fv(locMatMVP, 1, GL_FALSE, &matVP[0][0]);
  }
}

/**
* �`��Ɏg���郂�f���s���ݒ肷��.
*
* @param m �ݒ肷�郂�f���s��.
*/
void Program::SetModelMatrix(const glm::mat4& m)
{
  if (locMatModel >= 0) {
    glUniformMatrix4fv(locMatModel, 1, GL_FALSE, &m[0][0]);
  }
}

/**
*
* @param count      �`��Ɏg�p����|�C���g���C�g�̐�(0�`8).
* @param indexList  �`��Ɏg�p����|�C���g���C�g�ԍ��̔z��.
*/
void Program::SetPointLightIndex(int count, const int* indexList)
{
  const int maxCount = 8;
  if (count > maxCount) {
    count = maxCount;
    std::cerr << "[�x��]" << __func__ << ": ���C�g�̐���" << count <<
      "���w�肳��܂����B�ő�l��" << maxCount << "�ł��B\n";
  }

  const int maxIndex = 100;
  for (int i = 0; i < count; ++i) {
    if (indexList[i] < 0) {
      std::cerr << "[�x��]" << __func__ << ": " << i << "�Ԗڂ̃��C�g�ԍ���" <<
        indexList[i] << "���w�肳��܂����B�ŏ��l��0�ł��B\n";
    } else if (indexList[i] > maxIndex) {
      std::cerr << "[�x��]" << __func__ << ": " << i << "�Ԗڂ̃��C�g�ԍ���" <<
        indexList[i] << "���w�肳��܂����B�ő�l��" << maxIndex << "�ł��B\n";
    }
  }

  if (locPointLightCount >= 0) {
    glUniform1i(locPointLightCount, count);
    {
      GLenum err = glGetError();
      if (err != GL_NO_ERROR) {
        std::cerr << "����[\n";
      }
    }
  }
  if (locPointLightIndex >= 0 && count > 0) {
    glUniform1iv(locPointLightIndex, count, indexList);
  }
  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      std::cerr << "����[\n";
    }
  }
}

/**
* �`��Ɏg���郉�C�g��ݒ肷��
*
* @param count      �`��Ɏg�p����X�|�b�g���C�g�̐�(0�`8).
* @param indexList  �`��Ɏg�p����X�|�b�g���C�g�ԍ��̔z��.
*/
void Program::SetSpotLightIndex(int count, const int* indexList)
{
  const int maxCount = 8;
  if (count > maxCount) {
    count = maxCount;
    std::cerr << "[�x��]" << __func__ << ": ���C�g�̐���" << count <<
      "���w�肳��܂����B�ő�l��" << maxCount << "�ł��B\n";
  }

  const int maxIndex = 100;
  for (int i = 0; i < count; ++i) {
    if (indexList[i] < 0) {
      std::cerr << "[�x��]" << __func__ << ": " << i << "�Ԗڂ̃��C�g�ԍ���" <<
        indexList[i] << "���w�肳��܂����B�ŏ��l��0�ł��B\n";
    } else if (indexList[i] > maxIndex) {
      std::cerr << "[�x��]" << __func__ << ": " << i << "�Ԗڂ̃��C�g�ԍ���" <<
        indexList[i] << "���w�肳��܂����B�ő�l��" << maxIndex << "�ł��B\n";
    }
  }

  if (locSpotLightCount >= 0) {
    glUniform1i(locSpotLightCount, count);
  }
  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      std::cerr << "����[\n";
    }
  }
  if (locSpotLightIndex >= 0 && count > 0) {
    glUniform1iv(locSpotLightIndex, count, indexList);
  }
  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      std::cerr << "����[\n";
    }
  }
}

/**
* �v���O�����I�u�W�F�N�g���쐬����.
*
* @param vsPath  ���_�V�F�[�_�[�t�@�C����.
* @param fsPath  �t���O�����g�V�F�[�_�[�t�@�C����.
*
* @return �쐬�����v���O�����I�u�W�F�N�g.
*/
ProgramPtr Program::Create(const char* vsPath, const char* fsPath)
{
  return std::make_shared<Program>(BuildFromFile(vsPath, fsPath));
}

} // namespace Shader