/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "main.h"
#include "lodepng.h"

#define _USE_MATH_DEFINES
#include <algorithm>
#include <chrono>
#include <kodi/General.h>
#include <math.h>

const std::vector<Preset> CScreensaverShadertoy::g_presets =
{
  { "Ball",                       "ball.frag.glsl",               { "tex16.png", "tex16.png",    "",          "" } },
  { "Ball Of Fire",               "balloffire.frag.glsl",         { "",          "",             "",          "" } },
  { "Bleepy Blocks",              "bleepyblocks.frag.glsl",       { "tex15.png", "",             "",          "" } },
  { "Bob Scanline",               "blobandscsanline.glsl",        { "",          "",             "",          "" } },
  { "Blobs",                      "blobs.frag.glsl",              { "",          "",             "",          "" } },
  { "Blue Fire",                  "bluefire.frag.glsl",           { "tex12.png", "",             "",          "" } },
  { "Cellular",                   "cellular.frag.glsl",           { "",          "",             "",          "" } },
  { "Classic RGB Plasma",         "rgbplasma.frag.glsl",          { "",          "",             "",          "" } },
  { "Coloured Circles",           "colorcircles.frag.glsl",       { "",          "",             "",          "" } },
  { "Dance Floor",                "dancefloor.frag.glsl",         { "",          "",             "",          "" } },
  { "Dot Dot Dot",                "dotdotdot.frag.glsl",          { "",          "",             "",          "" } },
  { "Effect Ringblobs",           "ringblobs.frag.glsl",          { "",          "",             "",          "" } },
  { "Ether",                      "ether.frag.glsl",              { "",          "",             "",          "" } },
  { "Fire",                       "fire.frag.glsl",               { "tex12.png", "",             "",          "" } },
  { "Flaring 1",                  "flaringtype1.frag.glsl",       { "tex12.png", "",             "",          "" } },
  { "Flaring 2",                  "flaringtype2.frag.glsl",       { "tex12.png", "",             "",          "" } },
  { "Flaring 3",                  "flaringtype3.frag.glsl",       { "tex12.png", "",             "",          "" } },
  { "Flaring 4",                  "flaringtype4.frag.glsl",       { "tex12.png", "",             "",          "" } },
  { "Flaring 5",                  "flaringtype5.frag.glsl",       { "tex12.png", "",             "",          "" } },
  { "Fractal Tiling",             "fractaltiling.frag.glsl",      { "",          "",             "",          "" } },
  { "Infinite Fall",              "infinitefall.frag.glsl",       { "tex16.png" ,"",             "",          "" } },
  { "Input Time",                 "inputtime.frag.glsl",          { "",          "",             "",          "" } },
  { "Interstellar",               "stellar.frag.glsl",            { "tex12.png", "",             "",          "" } },
  { "Interweaving Sine bands",    "sinebands.frag.glsl",          { "",          "",             "",          "" } },
  { "Julia Trap",                 "juliatrap.frag.glsl",          { "",          "",             "",          "" } },
  { "Layers Of Plasma",           "layersplasma.frag.glsl",       { "",          "",             "",          "" } },
  { "Liquid Spectrum",            "liquidspectrum.frag.glsl",     { "",          "",             "",          "" } },
  { "Noise Animation - Electric", "noiseanimelectric.frag.glsl",  { "tex12.png", "",             "",          "" } },
  { "Noise Animation - Flow",     "noiseanimflow.frag.glsl",      { "tex12.png", "",             "",          "" } },
  { "Noise Animation - Lava",     "noiseanimlava.frag.glsl",      { "tex12.png", "",             "",          "" } },
  { "Noise Animation - Watery",   "noiseanimwatery.frag.glsl",    { "tex12.png", "",             "",          "" } },
  { "Overly Satisfying",          "overlysatisfying.frag.glsl",   { "",          "",             "",          "" } },
  { "Plasma Circles",             "plasmacircles.frag.glsl",      { "",          "",             "",          "" } },
  { "Plasma Triangles",           "plasmatriangle.frag.glsl",     { "",          "",             "",          "" } },
  { "Plasma Matrix Spin",         "plasmatrixspin.frag.glsl",     { "",          "",             "",          "" } },
  { "Pixellated Plasma",          "pixellated.frag.glsl",         { "",          "",             "",          "" } },
  { "Silexarst Creation",         "silexarst.frag.glsl",          { "",          "",             "",          "" } },
  { "Sky at Night",               "skyatnight.frag.glsl",         { "",          "",             "",          "" } },
  { "Sphere",                     "sphere.frag.glsl",             { "",          "",             "",          "" } },
  { "Star Tunnel",                "startunnel.frag.glsl",         { "",          "",             "",          "" } },
  { "Vector Field",               "vectorfield.frag.glsl",        { "tex16.png", "keyboard.png", "tex09.png", "" } },
  { "Venus",                      "venus.frag.glsl",              { "tex16.png", "",             "",          "" } },
  { "Watch It Burn",              "watchitburn.frag.glsl",        { "tex16.png", "",             "",          "" } },
  { "Water Caustic",              "watercaustic.frag.glsl",       { "",          "",             "",          "" } },
  { "Wiggly (try mouse)",         "wiggly.frag.glsl",             { "",          "",             "",          "" } },
  { "Worley Noise Waters",        "worleynoisewaters.frag.glsl",  { "",          "",             "",          "" } },
};

#if defined(HAS_GL)

std::string fsHeader =
R"shader(#version 150

#extension GL_OES_standard_derivatives : enable

uniform vec3 iResolution;
uniform float iGlobalTime;
uniform float iChannelTime[4];
uniform vec4 iMouse;
uniform vec4 iDate;
uniform float iSampleRate;
uniform vec3 iChannelResolution[4];
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;

out vec4 FragColor;

#define iTime iGlobalTime

#ifndef texture2D
#define texture2D texture
#endif
)shader";

std::string fsFooter =
R"shader(
void main(void)
{
  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
  mainImage(color, gl_FragCoord.xy);
  color.w = 1.0;
  FragColor = color;
}
)shader";

#else

std::string fsHeader =
R"shader(#version 100

#extension GL_OES_standard_derivatives : enable

precision mediump float;
precision mediump int;

uniform vec3 iResolution;
uniform float iGlobalTime;
uniform float iChannelTime[4];
uniform vec4 iMouse;
uniform vec4 iDate;
uniform float iSampleRate;
uniform vec3 iChannelResolution[4];
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;

#define iTime iGlobalTime
#ifndef texture
#define texture texture2D
#endif

#ifndef textureLod
vec4 textureLod(sampler2D sampler, vec2 uv, float lod)
{
  return texture2D(sampler, uv, lod);
}
#endif
)shader";

std::string fsFooter =
R"shader(
void main(void)
{
  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
  mainImage(color, gl_FragCoord.xy);
  color.w = 1.0;
  gl_FragColor = color;
}
)shader";

#endif

CScreensaverShadertoy::CScreensaverShadertoy()
{
  m_settingsUseOwnshader = kodi::GetSettingBoolean("ownshader");
  if (m_settingsUseOwnshader)
  {
    m_currentPreset = -1;
    std::string str = kodi::GetSettingString("shader");
    if (!kodi::vfs::FileExists(str))
    {
      kodi::QueueNotification(QUEUE_ERROR, "", kodi::GetLocalizedString(30104));
      m_settingsUseOwnshader = false;
    }
  }

  if (!m_settingsUseOwnshader)
  {
    int c = kodi::GetSettingInt("preset");
    if (c < 0)
      c = 0;
    else if (c > g_presets.size())
        c = g_presets.size();

    if (c == 0)
      m_currentPreset = rand() % m_numberPresets;
    else
      m_currentPreset = c - 1;
  }
}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
void CScreensaverShadertoy::Render()
{
  if (m_initialized)
  {
    if (m_state.fbwidth && m_state.fbheight)
    {
      RenderTo(m_shadertoyShader.ProgramHandle(), m_state.effect_fb);
      RenderTo(m_displayShader.ProgramHandle(), 0);
    }
    else
    {
      RenderTo(m_shadertoyShader.ProgramHandle(), 0);
    }
  }
}

bool CScreensaverShadertoy::Start()
{
#ifdef DEBUG_PRINT
  printf("Start\n");
#endif

  static const GLfloat vertex_data[] =
  {
    -1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0,
     1.0,-1.0, 1.0, 1.0,
    -1.0,-1.0, 1.0, 1.0,
  };

  // Upload vertex data to a buffer
  glGenBuffers(1, &m_state.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_state.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

  Launch(m_currentPreset);
  m_initialized = true;

  return true;
}

void CScreensaverShadertoy::Stop()
{
  m_initialized = false;
#ifdef DEBUG_PRINT
  printf("Stop\n");
#endif

  UnloadPreset();
  UnloadTextures();

  glDeleteBuffers(1, &m_state.vertex_buffer);
}

void CScreensaverShadertoy::RenderTo(GLuint shader, GLuint effect_fb)
{
  glUseProgram(shader);

  if (shader == m_shadertoyShader.ProgramHandle())
  {
    GLuint w = Width();
    GLuint h = Height();
    if (m_state.fbwidth && m_state.fbheight)
      w = m_state.fbwidth, h = m_state.fbheight;
    int64_t intt = static_cast<int64_t>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0) - m_initialTime;
    if (m_bitsPrecision)
      intt &= (1<<m_bitsPrecision)-1;

    float t = intt / 1000.0f;
    GLfloat tv[] = { t, t, t, t };

    glUniform3f(m_attrResolutionLoc, w, h, 0.0f);
    glUniform1f(m_attrGlobalTimeLoc, t);
    glUniform1f(m_attrSampleRateLoc, 44100);
    glUniform1fv(m_attrChannelTimeLoc, 4, tv);
    glUniform2f(m_state.uScale, static_cast<GLfloat>(Width()) / m_state.fbwidth, static_cast<GLfloat>(Height()) /m_state.fbheight);

    time_t now = time(NULL);
    tm *ltm = localtime(&now);

    float year = 1900 + ltm->tm_year;
    float month = ltm->tm_mon;
    float day = ltm->tm_mday;
    float sec = (ltm->tm_hour * 60 * 60) + (ltm->tm_min * 60) + ltm->tm_sec;

    glUniform4f(m_attrDateLoc, year, month, day, sec);

    for (int i = 0; i < 4; i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glUniform1i(m_attrChannelLoc[i], i);
      glBindTexture(GL_TEXTURE_2D, m_channelTextures[i]);
    }
  }
  else
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_state.framebuffer_texture);
    glUniform1i(m_state.uTexture, 0); // first currently bound texture "GL_TEXTURE0"
  }

  // Draw the effect to a texture or direct to framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, effect_fb);

  GLuint attr_vertex = shader == m_shadertoyShader.ProgramHandle() ? m_state.attr_vertex_e : m_state.attr_vertex_r;
  glBindBuffer(GL_ARRAY_BUFFER, m_state.vertex_buffer);
  glVertexAttribPointer(attr_vertex, 4, GL_FLOAT, 0, 16, 0);
  glEnableVertexAttribArray(attr_vertex);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableVertexAttribArray(attr_vertex);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0; i < 4; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glUseProgram(0);
}

void CScreensaverShadertoy::Launch(int preset)
{
  m_bitsPrecision = DetermineBitsPrecision();
  // mali-400 has only 10 bits which means milliseond timer wraps after ~1 second.
  // we'll fudge that up a bit as having a larger range is more important than ms accuracy
  m_bitsPrecision = std::max(m_bitsPrecision, 13);
#ifdef DEBUG_PRINT
  printf("bits=%d\n", m_bitsPrecision);
#endif

  UnloadTextures();

  if (preset < 0)
  {
    m_usedShaderFile = kodi::GetSettingString("shader");
    m_shaderTextures[0] = kodi::GetSettingString("texture0");
    m_shaderTextures[1] = kodi::GetSettingString("texture1");
    m_shaderTextures[2] = kodi::GetSettingString("texture2");
    m_shaderTextures[3] = kodi::GetSettingString("texture3");
  }
  else
  {
    m_usedShaderFile = kodi::GetAddonPath("resources/shaders/" + g_presets[preset].file);

    m_usedShaderFile = kodi::GetAddonPath("resources/shaders/" + g_presets[m_currentPreset].file);
    m_shaderTextures[0] = !g_presets[m_currentPreset].channel[0].empty() ? kodi::GetAddonPath("resources/" + g_presets[m_currentPreset].channel[0]) : "";
    m_shaderTextures[1] = !g_presets[m_currentPreset].channel[1].empty() ? kodi::GetAddonPath("resources/" + g_presets[m_currentPreset].channel[1]) : "";
    m_shaderTextures[2] = !g_presets[m_currentPreset].channel[2].empty() ? kodi::GetAddonPath("resources/" + g_presets[m_currentPreset].channel[2]) : "";
    m_shaderTextures[3] = !g_presets[m_currentPreset].channel[3].empty() ? kodi::GetAddonPath("resources/" + g_presets[m_currentPreset].channel[3]) : "";
  }

  for (int i = 0; i < 4; i++)
  {
    if (!m_shaderTextures[i].empty())
    {
      GLint format = GL_RGBA;
      GLint scaling = GL_LINEAR;
      GLint repeat = GL_REPEAT;
      m_channelTextures[i] = CreateTexture(m_shaderTextures[i], format, scaling, repeat);
    }
  }

  const int size1 = 256, size2=512;
  double t1 = MeasurePerformance(m_usedShaderFile, size1);
  double t2 = MeasurePerformance(m_usedShaderFile, size2);

  double expected_fps = 40.0;
  // time per pixel for rendering fragment shader
  double B = (t2-t1)/(size2*size2-size1*size1);
  // time to render to screen
  double A = t2 - size2*size2 * B;
  // how many pixels get the desired fps
  double pixels = (1000.0/expected_fps - A) / B;
  m_state.fbwidth = sqrtf(pixels * Width() / Height());
  if (m_state.fbwidth >= Width())
    m_state.fbwidth = 0;
  else if (m_state.fbwidth < 320)
    m_state.fbwidth = 320;
  m_state.fbheight = m_state.fbwidth * Height() / Width();

#ifdef DEBUG_PRINT
  printf("expected fps=%f, pixels=%f %dx%d (A:%f B:%f t1:%.1f t2:%.1f)\n", expected_fps, pixels, m_state.fbwidth, m_state.fbheight, A, B, t1, t2);
#endif

  LoadPreset(m_usedShaderFile);
}

void CScreensaverShadertoy::UnloadTextures()
{
  for (int i = 0; i < 4; i++)
  {
    if (m_channelTextures[i])
    {
      glDeleteTextures(1, &m_channelTextures[i]);
      m_channelTextures[i] = 0;
    }
  }
}

void CScreensaverShadertoy::LoadPreset(const std::string& shaderPath)
{
  UnloadPreset();
  std::string vertShadertoyShader = kodi::GetAddonPath("resources/shaders/main_shadertoy_" GL_TYPE_STRING ".vert.glsl");
  if (!m_shadertoyShader.LoadShaderFiles(vertShadertoyShader, shaderPath) ||
      !m_shadertoyShader.CompileAndLink("", "", fsHeader, fsFooter))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to compile shadertoy shaders (current shadertoy file '%s')", shaderPath.c_str());
    return;
  }

  GLuint shadertoyShader = m_shadertoyShader.ProgramHandle();

  m_attrResolutionLoc = glGetUniformLocation(shadertoyShader, "iResolution");
  m_attrGlobalTimeLoc = glGetUniformLocation(shadertoyShader, "iGlobalTime");
  m_attrChannelTimeLoc = glGetUniformLocation(shadertoyShader, "iChannelTime");
  m_attrMouseLoc = glGetUniformLocation(shadertoyShader, "iMouse");
  m_attrDateLoc = glGetUniformLocation(shadertoyShader, "iDate");
  m_attrSampleRateLoc  = glGetUniformLocation(shadertoyShader, "iSampleRate");
  m_attrChannelResolutionLoc = glGetUniformLocation(shadertoyShader, "iChannelResolution");
  m_attrChannelLoc[0] = glGetUniformLocation(shadertoyShader, "iChannel0");
  m_attrChannelLoc[1] = glGetUniformLocation(shadertoyShader, "iChannel1");
  m_attrChannelLoc[2] = glGetUniformLocation(shadertoyShader, "iChannel2");
  m_attrChannelLoc[3] = glGetUniformLocation(shadertoyShader, "iChannel3");

  m_state.uScale = glGetUniformLocation(shadertoyShader, "uScale");
  m_state.attr_vertex_e = glGetAttribLocation(shadertoyShader,  "vertex");

  std::string vertShader = kodi::GetAddonPath("resources/shaders/main_display_" GL_TYPE_STRING ".vert.glsl");
  std::string fraqShader = kodi::GetAddonPath("resources/shaders/main_display_" GL_TYPE_STRING ".frag.glsl");
  if (!m_displayShader.LoadShaderFiles(vertShader, fraqShader) ||
      !m_displayShader.CompileAndLink())
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to compile main shaders");
    return;
  }

  m_state.uTexture = glGetUniformLocation(m_displayShader.ProgramHandle(), "uTexture");
  m_state.attr_vertex_r = glGetAttribLocation(m_displayShader.ProgramHandle(), "vertex");

  // Prepare a texture to render to
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &m_state.framebuffer_texture);
  glBindTexture(GL_TEXTURE_2D, m_state.framebuffer_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_state.fbwidth, m_state.fbheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Prepare a framebuffer for rendering
  glGenFramebuffers(1, &m_state.effect_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, m_state.effect_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_state.framebuffer_texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_initialTime = static_cast<int64_t>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0);
}

void CScreensaverShadertoy::UnloadPreset()
{
  if (m_state.framebuffer_texture)
  {
    glDeleteTextures(1, &m_state.framebuffer_texture);
    m_state.framebuffer_texture = 0;
  }
  if (m_state.effect_fb)
  {
    glDeleteFramebuffers(1, &m_state.effect_fb);
    m_state.effect_fb = 0;
  }
}

GLuint CScreensaverShadertoy::CreateTexture(GLint format, unsigned int w, unsigned int h, const GLvoid* data)
{
  GLuint texture = 0;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  return texture;
}

GLuint CScreensaverShadertoy::CreateTexture(const GLvoid* data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat)
{
  GLuint texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

GLuint CScreensaverShadertoy::CreateTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat)
{
#ifdef DEBUG_PRINT
  printf("creating texture %s\n", file.c_str());
#endif

  unsigned error;
  unsigned char* image;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, file.c_str());
  if (error)
  {
    kodi::Log(ADDON_LOG_ERROR, "lodepng_decode32_file error %u: %s", error, lodepng_error_text(error));
    return 0;
  }

  GLuint texture = CreateTexture(image, GL_RGBA, width, height, internalFormat, scaling, repeat);
  free(image);
  return texture;
}

int CScreensaverShadertoy::DetermineBitsPrecision()
{
  m_state.fbwidth = 32, m_state.fbheight = 26*10;
  LoadPreset(kodi::GetAddonPath("resources/shaders/main_test.frag.glsl"));
  RenderTo(m_shadertoyShader.ProgramHandle(), m_state.effect_fb);
  glFinish();

  unsigned char* buffer = new unsigned char[m_state.fbwidth * m_state.fbheight * 4];
  if (buffer)
    glReadPixels(0, 0, m_state.fbwidth, m_state.fbheight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

  int bits = 0;
  unsigned char b = 0;
  for (int j=0; j<m_state.fbheight; j++)
  {
    unsigned char c = buffer[4*(j*m_state.fbwidth+(m_state.fbwidth>>1))];
    if (c && !b)
      bits++;
    b = c;
  }
  delete buffer;
  UnloadPreset();
  return bits;
}

double CScreensaverShadertoy::MeasurePerformance(const std::string& shaderPath, int size)
{
  int iterations = -1;
  m_state.fbwidth = m_state.fbheight = size;
  LoadPreset(shaderPath);

  int64_t end, start;
  do
  {
    RenderTo(m_shadertoyShader.ProgramHandle(), m_state.effect_fb);
    RenderTo(m_displayShader.ProgramHandle(), m_state.effect_fb);
    glFinish();
    if (++iterations == 0)
      start = static_cast<int64_t>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0);
    end = static_cast<int64_t>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0);
  } while (end - start < 50);
  double t = (double)(end - start)/iterations;
#ifdef DEBUG_PRINT
  printf("%s %dx%d %.1fms = %.2f fps\n", __func__, size, size, t, 1000.0/t);
#endif
  UnloadPreset();
  return t;
}

ADDONCREATOR(CScreensaverShadertoy) // Don't touch this!
