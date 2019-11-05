/**
* @file DepthOfField.frag
*/
#version 430

layout(location=0) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D texColorArray[2];

/**
* ��ʂ̏��.
*
* x: 1.0 / �E�B���h�E�̕�(�s�N�Z���P��)
* y: 1.0 / �E�B���h�E�̍���(�s�N�Z���P��)
* x: near(m�P��)
* y: far(m�P��)
*/
uniform vec4 viewInfo;

/**
* �J�����̏��.
*
* x: �ŕ���(�s���g�̍����ʒu�̃����Y����̋���. mm�P��).
* y: �œ_����(����1�_�ɏW�܂�ʒu�̃����Y����̋���. mm�P��).
* z: �J��(���̎�����̃T�C�Y. mm�P��).
* w: �Z���T�[�T�C�Y(�����󂯂�Z���T�[�̉���. mm�P��).
*/
uniform vec4 cameraInfo;

vec3 ToYUV(vec3 rgb) {
  return mat3(
    0.299, -0.168736, 0.5,
    0.587, -0.331264, -0.418688,
    0.114, 0.5, -0.081312) * rgb;
}

vec3 ToRGB(vec3 yuv) {
  return mat3(
    1.0, 1.0, 1.0,
    0.0, -0.344136, 1.772,
    1.402, -0.714136, 0.0) * yuv;
}

#if 1
// https://github.com/spite/Wagner/blob/master/fragment-shaders/poisson-disc-blur-fs.glsl
const int poissonSampleCount = 12;
const vec2 poissonDisk[poissonSampleCount] = {
  {-0.326,-0.406},
  {-0.840,-0.074},
  {-0.696, 0.457},
  {-0.203, 0.621},
  { 0.962,-0.195},
  { 0.473,-0.480},
  { 0.519, 0.767},
  { 0.185,-0.893},
  { 0.507, 0.064},
  { 0.896, 0.412},
  {-0.322,-0.933},
  {-0.792,-0.598}
};
#else
const int poissonSampleCount = 16;
const vec2 poissonDisk[poissonSampleCount] = vec2[](
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
#endif

/**
* �����~�̔��a���v�Z����.
*
* @param objectDistance �Ώۃs�N�Z���̃J�������W�n�ɂ�����Z���W.
*
* @return �����~�̔��a(�s�N�Z���P��).
*/
vec2 CalcCoC(float objectDistance)
{
  objectDistance *= -1000.0; // mm�P�ʂɕϊ����A�l�𐳂ɂ���.
  float focalPlane = cameraInfo.x;
  float focalLength = cameraInfo.y;
  float aperture = cameraInfo.z;
  float sensorSize = cameraInfo.w;
  float mmToPixel = 1.0 / (sensorSize * viewInfo.x); // mm����s�N�Z�����ւ̕ϊ��W��. �܂��ACoC�͒��a�A�|�A�\���f�B�X�N�̊g�嗦�͔��a�Ȃ̂�0.5�{���Ă���.
  return abs(aperture * (focalLength * (focalPlane - objectDistance)) /
    (objectDistance * (focalPlane - focalLength))) * mmToPixel * viewInfo.xy;
}

/**
* �[�x�o�b�t�@�̐[�x�l���r���[���W�n�̐[�x�l�ɕϊ�����.
*
* @param w �[�x�o�b�t�@�̐[�x�l.
*
* @return w���r���[���W�n�̐[�x�l�ɕϊ������l.
*/
float ToRealZ(float w)
{
  float near = viewInfo.z;
  float far = viewInfo.w;
  float n = 2.0 * w - 1.0;
  return -2.0 * near * far / (far + near - n * (far - near));
}

/**
* DepthOfField fragment shader.
*/
void main()
{
#if 0
  float centerZ = texture(texColorArray[1], inTexCoord).r;
  fragColor.rgb = texture(texColorArray[0], inTexCoord).rgb;
  for (int i = 0; i < 12; ++i) {
    vec2 uv = inTexCoord + poissonDisk[i] * (centerZ - 0.935) * 0.1;
    fragColor.rgb += texture(texColorArray[0], uv).rgb;
  }
  fragColor.rgb = fragColor.rgb / float(poissonSampleCount + 1);
  fragColor.a = 1.0;
#else
  float centerZ = ToRealZ(texture(texColorArray[1], inTexCoord).r);
  vec2 coc = CalcCoC(centerZ);
  fragColor.rgb = texture(texColorArray[0], inTexCoord).rgb;
  float count = 1;
  for (int i = 0; i < poissonSampleCount; ++i) {
    vec2 uv = inTexCoord + coc * poissonDisk[i];
#if 0
    float focalPlane = cameraInfo.x * -0.001;
    float z = ToRealZ(texture(texColorArray[1], uv).r);
    if (coc.x > (2.0 * viewInfo.x) && abs(focalPlane - z) < 2.0) { 
      fragColor.rgb += texture(texColorArray[0], inTexCoord).rgb;
    } else {
      fragColor.rgb += texture(texColorArray[0], uv).rgb;
    }
#elif 0
    float focalPlane = cameraInfo.x * -0.001;
    float z = ToRealZ(texture(texColorArray[1], uv).r);
    if (coc.x > (2.0 * viewInfo.x) && abs(focalPlane - z) < 2.0) { 
    } else {
      fragColor.rgb += texture(texColorArray[0], uv).rgb;
	  count += 1.0;
    }
#else
    fragColor.rgb += texture(texColorArray[0], uv).rgb;
#endif
  }
  //fragColor.rgb = ToRGB(ToYUV(fragColor.rgb) * (1.0 / (float(poissonSampleCount) + 1.0)));
  fragColor.rgb = fragColor.rgb * (1.0 / (float(poissonSampleCount) + 1.0));
  //fragColor.rgb = fragColor.rgb / count;
  //fragColor.rgb = vec3(abs(centerZ - cameraInfo.x * -0.001) * 0.01);
  //fragColor.rgb = vec3(coc.x * 0.5 / viewInfo.x);
  fragColor.a = 1.0;
#endif
}