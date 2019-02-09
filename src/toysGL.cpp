/*
 * Screensavers from Shadertoy for Kodi OpenGL
 * Copyright (c) 2015-2019 Team Kodi
 *
 * Ver 1.0 12 May 2015	Anton Fedchin (afedchin)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <kodi/addon-instance/Screensaver.h>
#include <kodi/gui/OpenGL/Shader.h>
#include <kodi/General.h>
#include <kodi/utils/Time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "toys.h"
#include "lodepng.h"

#define SHADER_SOURCE(...) #__VA_ARGS__
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

struct MYVERTEX {
  float x, y, z;
  float u, v;
};

struct Preset {
  std::string name;
  std::string file;
  std::string channel[4];
};

const std::vector<Preset> g_presets =
{
  {
    "Ball",
    "ball.frag.glsl",
    {
      "tex16.png",
      "tex16.png",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Bleepy Blocks",
    "bleepyblocks.frag.glsl",
    {
      "tex15.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Bob Scanline",
    "blobandscsanline.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Blobs",
    "blobs.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Blue Fire",
    "bluefire.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Classic RGB Plasma",
    "rgbplasma.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Dance Floor",
    "dancefloor.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Effect Ringblobs",
    "ringblobs.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  //{ "Ether",                    "ether.frag.glsl",
//     {
//       "",
//       "",
//       "",
//       ""
//     },
  {
    "Fire",
    "fire.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Flaring 1",
    "flaringtype1.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Flaring 2",
    "flaringtype2.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Flaring 3",
    "flaringtype3.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Flaring 4",
    "flaringtype4.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Flaring 5",
    "flaringtype5.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Interstellar",
    "stellar.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Interweaving Sine bands",
    "sinebands.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Julia Trap",
    "juliatrap.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Layers Of Plasma",
    "layersplasma.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Liquid Spectrum",
    "liquidspectrum.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Noise Animation - Electric",
    "noiseanimelectric.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Noise Animation - Flow",
    "noiseanimflow.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Noise Animation - Lava",
    "noiseanimlava.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Noise Animation - Watery",
    "noiseanimwatery.frag.glsl",
    {
      "tex12.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Overly Satisfying",
    "overlysatisfying.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Plasma Circles",
    "plasmacircles.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Plasma Triangles",
    "plasmatriangle.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Plas.mAtrix. Spin",
    "plasmatrixspin.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Pixellated Plasma",
    "pixellated.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Silexarst Creation",
    "silexarst.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Sky at Night",
    "skyatnight.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Sphere",
    "sphere.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Star Tunnel",
    "startunnel.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Vector Field",
    "vectorfield.frag.glsl",
    {
      "tex16.png",
      "keyboard.png",
      "tex09.png",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Venus",
    "venus.frag.glsl",
    {
      "tex16.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Warp",
    "warp.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Watch It Burn",
    "watchitburn.frag.glsl",
    {
      "tex16.png",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Water Caustic",
    "watercaustic.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  },
  //----------------------------------------------------------------------------
  {
    "Worley Noise Waters",
    "worleynoisewaters.frag.glsl",
    {
      "",
      "",
      "",
      ""
    }
  }
  //----------------------------------------------------------------------------
  //"balloffire.frag.glsl",
  //"noiseanimwatery.frag.glsl",
  //"tweets.frag.glsl",
  //"wiggly.frag.glsl",
};

std::string fsHeader =
  "#version 130\n"
  "uniform vec3    iResolution;\n"
  "uniform float   iGlobalTime;\n"
  "uniform float   iChannelTime[4];\n"
  "uniform vec4    iMouse;\n"
  "uniform vec4    iDate;\n"
  "uniform float   iSampleRate;\n"
  "uniform vec3    iChannelResolution[4];\n"

  "uniform sampler2D iChannel0;\n"
  "uniform sampler2D iChannel1;\n"
  "uniform sampler2D iChannel2;\n"
  "uniform sampler2D iChannel3;\n"

  "in vec2 uv;\n"

  "vec4 color;\n"

  "#define iTime iGlobalTime";

std::string fsFooter = SHADER_SOURCE(
  void main()
  {
//     vec2 fragCoord = uv;
//     fragCoord.x *= (iResolution.x - 0.5);
//     fragCoord.y *= (iResolution.y - 0.5);
    mainImage(color, gl_FragCoord.xy);
    color.w = 1.0;
    gl_FragColor = color;
  }
);

class ATTRIBUTE_HIDDEN CScreensaverToys
  : public kodi::addon::CAddonBase,
    public kodi::addon::CInstanceScreensaver
  , public kodi::gui::gl::CShaderProgram
{
public:
  CScreensaverToys();
  virtual ~CScreensaverToys();

  // kodi::addon::CInstanceScreensaver
  bool Start() override;
  void Stop() override;
  void Render() override;

  // kodi::gui::gl::CShaderProgram
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;

private:
  bool InitPreset(int preset);
  float fCubicInterpolate(float y0, float y1, float y2, float y3, float t);
  unsigned long dwCubicInterpolate(unsigned long y0, unsigned long y1, unsigned long y2, unsigned long y3, float t);
  GLint loadTexture(const std::string& name);
  GLuint createTexture(GLint format, unsigned int w, unsigned int h, const GLvoid * data);
  GLuint createTexture(const GLvoid *data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat);
  GLuint createTexture(const std::string&, GLint internalFormat, GLint scaling, GLint repeat);

  MYVERTEX m_verts[4] =
  {
    { -1.0f,  1.0f, 0.0f, 0.0f, 1.0f },
    {  1.0f,  1.0f, 0.0f, 1.0f, 1.0f },
    {  1.0f, -1.0f, 0.0f, 1.0f, 0.0f },
    { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f },
  };

  int m_numberPresets = g_presets.size();
  int m_currentPreset = 0;
  int m_width;
  int m_height;

  GLint m_iResolutionLoc = -1;
  GLint m_iGlobalTimeLoc = -1;
  GLint m_iChannelTimeLoc = -1;
  GLint m_iMouseLoc = -1;
  GLint m_iDateLoc = -1;
  GLint m_iSampleRateLoc = -1;
  GLint m_iChannelResolutionLoc = -1;
  GLint m_iChannel0Loc = -1;
  GLint m_iChannel1Loc = -1;
  GLint m_iChannel2Loc = -1;
  GLint m_iChannel3Loc = -1;
  GLint m_hVertex = -1;
  GLint m_hCoord = -1;

  GLuint m_vertexVBO = 0;

  glm::vec3 m_iResolution;
  float m_iGlobalTime;
  float m_iChannelTime[4];
  glm::vec4 m_iMouse;
  glm::vec4 m_iDate;
  float m_iSampleRate;
  glm::vec3 m_iChannelResolution[4];

  GLuint m_textureChannels[4] = { 0 };

  std::string m_currentShaderFile;
  std::string m_textureFiles[4];
};

CScreensaverToys::~CScreensaverToys()
{
}

////////////////////////////////////////////////////////////////////////////
// Kodi has loaded us into memory, we should set our core values
// here and load any settings we may have from our config file
//
CScreensaverToys::CScreensaverToys()
{
  m_width = Width();
  m_height = Height();

  bool ownShader = kodi::GetSettingBoolean("ownshader");
  if (ownShader)
  {
    m_currentPreset = -1;
    std::string str = kodi::GetSettingString("shader");
    if (kodi::vfs::FileExists(str))
    {
      m_currentShaderFile = str;
      str = kodi::GetSettingString("texture0");
      if (kodi::vfs::FileExists(str))
        m_textureFiles[0] = str;
      str = kodi::GetSettingString("texture1");
      if (kodi::vfs::FileExists(str))
        m_textureFiles[1] = str;
      str = kodi::GetSettingString("texture2");
      if (kodi::vfs::FileExists(str))
        m_textureFiles[2] = str;
      str = kodi::GetSettingString("texture3");
      if (kodi::vfs::FileExists(str))
        m_textureFiles[3] = str;
    }
    else
      kodi::QueueNotification(QUEUE_ERROR, "", kodi::GetLocalizedString(30104));
  }

  if (m_currentShaderFile.empty())
  {
    int c = ownShader ? 0 : kodi::GetSettingInt("preset");
    if (c < 0)
      c = 0;
    else if (c > g_presets.size())
        c = g_presets.size();

    if (c == 0)
      m_currentPreset = rand() % m_numberPresets;
    else
      m_currentPreset = c - 1;

    m_currentShaderFile = kodi::GetAddonPath("resources/" + g_presets[m_currentPreset].file);
    m_textureFiles[0] = g_presets[m_currentPreset].channel[0];
    m_textureFiles[1] = g_presets[m_currentPreset].channel[1];
    m_textureFiles[2] = g_presets[m_currentPreset].channel[2];
    m_textureFiles[3] = g_presets[m_currentPreset].channel[3];
  }

  srand(time(nullptr));
}

bool CScreensaverToys::Start()
{
  if (!CreateShader(kodi::GetAddonPath("resources/main.vert.glsl"),
                    m_currentShaderFile) ||
      !CompileAndLink("", "", fsHeader, fsFooter))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to create and compile shader");
    return false;
  }

  if (!InitPreset(m_currentPreset))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to init preset '%i'", m_currentPreset);
    return false;
  }

  glGenBuffers(1, &m_vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_verts), m_verts, GL_STATIC_DRAW);

  return true;
}

void CScreensaverToys::Stop()
{
  glDeleteBuffers(1, &m_vertexVBO);
  m_vertexVBO = 0;

  glDeleteTextures(4, m_textureChannels);
  memset(m_textureChannels, 0, sizeof(m_textureChannels));
  m_currentShaderFile = "";
}

bool CScreensaverToys::InitPreset(int preset)
{
  for (int i = 0; i < 4; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    if (!m_textureFiles[i].empty())
      m_textureChannels[i] = loadTexture(m_textureFiles[i]);
    glBindTexture(GL_TEXTURE_2D, m_textureChannels[i]);
  }
  return true;
}

void CScreensaverToys::Render()
{
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);

  glVertexAttribPointer(m_hVertex, 4, GL_FLOAT, GL_TRUE, sizeof(MYVERTEX), BUFFER_OFFSET(offsetof(MYVERTEX, x)));
  glEnableVertexAttribArray(m_hVertex);

  glVertexAttribPointer(m_hCoord, 2, GL_FLOAT, GL_TRUE, sizeof(MYVERTEX), BUFFER_OFFSET(offsetof(MYVERTEX, u)));
  glEnableVertexAttribArray(m_hCoord);

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glClear(GL_DEPTH_BUFFER_BIT);

  float t = kodi::time::GetTimeSec<float>();
  time_t now = time(nullptr);
  tm *ltm = localtime(&now);
  float year = (float)(1900 + ltm->tm_year);
  float month = (float)ltm->tm_mon;
  float day = (float)ltm->tm_mday;
  float sec = (float)(ltm->tm_hour * 3600 + ltm->tm_min * 60 + ltm->tm_sec);

  m_iResolution = glm::vec3((float)m_width, (float)m_height, (float)m_width/(float)m_height);
  m_iGlobalTime = t;
  m_iSampleRate = 0;
  m_iChannelTime[0] = m_iChannelTime[1] = m_iChannelTime[2] = m_iChannelTime[3] = t;
  m_iDate = glm::vec4(year, month, day, sec);

  Enable();
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  Disable();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glDisableVertexAttribArray(m_hVertex);
  glDisableVertexAttribArray(m_hCoord);
}

void CScreensaverToys::OnCompiledAndLinked()
{
  // Variables passed directly to the Vertex shader
  m_iResolutionLoc = glGetUniformLocation(ProgramHandle(), "iResolution");
  m_iGlobalTimeLoc = glGetUniformLocation(ProgramHandle(), "iGlobalTime");
  m_iChannelTimeLoc = glGetUniformLocation(ProgramHandle(), "iChannelTime");
  m_iMouseLoc = glGetUniformLocation(ProgramHandle(), "iMouse");
  m_iDateLoc = glGetUniformLocation(ProgramHandle(), "iDate");
  m_iSampleRateLoc = glGetUniformLocation(ProgramHandle(), "iSampleRate");
  m_iChannelResolutionLoc = glGetUniformLocation(ProgramHandle(), "iChannelResolution");
  m_iChannel0Loc = glGetUniformLocation(ProgramHandle(), "iChannel0");
  m_iChannel1Loc = glGetUniformLocation(ProgramHandle(), "iChannel1");
  m_iChannel2Loc = glGetUniformLocation(ProgramHandle(), "iChannel2");
  m_iChannel3Loc = glGetUniformLocation(ProgramHandle(), "iChannel3");

  m_hVertex = glGetAttribLocation(ProgramHandle(), "a_position");
  m_hCoord = glGetAttribLocation(ProgramHandle(), "a_coord");
}

bool CScreensaverToys::OnEnabled()
{
  glUniform3fv(m_iResolutionLoc, 1, glm::value_ptr(m_iResolution));
  glUniform1f(m_iGlobalTimeLoc, m_iGlobalTime);
  glUniform1fv(m_iChannelTimeLoc, 4, m_iChannelTime);
  glUniform4fv(m_iMouseLoc, 1, glm::value_ptr(m_iMouse));
  glUniform4fv(m_iDateLoc, 1, glm::value_ptr(m_iDate));
  glUniform1f(m_iSampleRateLoc, m_iSampleRate);
  glUniform3fv(m_iChannelResolutionLoc, 4, &m_iChannelResolution[0][0]);
  glUniform1i(m_iChannel0Loc, 0);
  glUniform1i(m_iChannel1Loc, 1);
  glUniform1i(m_iChannel2Loc, 2);
  glUniform1i(m_iChannel3Loc, 3);

  return true;
}

float CScreensaverToys::fCubicInterpolate(float y0, float y1, float y2, float y3, float t)
{
  float a0, a1, a2, a3, t2;

  t2 = t*t;
  a0 = y3 - y2 - y0 + y1;
  a1 = y0 - y1 - a0;
  a2 = y2 - y0;
  a3 = y1;

  return(a0*t*t2 + a1*t2 + a2*t + a3);
}

unsigned long CScreensaverToys::dwCubicInterpolate(unsigned long y0, unsigned long y1, unsigned long y2, unsigned long y3, float t)
{
  // performs cubic interpolation on a D3DCOLOR value.
  unsigned long ret = 0;
  unsigned long shift = 0;
  for (int i = 0; i<4; i++)
  {
    float f = fCubicInterpolate(
      ((y0 >> shift) & 0xFF) / 255.0f,
      ((y1 >> shift) & 0xFF) / 255.0f,
      ((y2 >> shift) & 0xFF) / 255.0f,
      ((y3 >> shift) & 0xFF) / 255.0f,
      t
      );
    if (f<0)
      f = 0;
    if (f>1)
      f = 1;
    ret |= ((unsigned long)(f * 255)) << shift;
    shift += 8;
  }
  return ret;
}

GLint CScreensaverToys::loadTexture(const std::string& name)
{
  if (!name.empty())
  {
    GLint format = GL_RGBA;
    GLint scaling = GL_LINEAR;
    GLint repeat = GL_REPEAT;
    return createTexture(name, format, scaling, repeat);
  }
  return 0;
}

GLuint CScreensaverToys::createTexture(GLint format, unsigned int w, unsigned int h, const GLvoid * data)
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

GLuint CScreensaverToys::createTexture(const GLvoid *data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat)
{
  GLuint texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);

#if defined(HAS_GLES)
  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
#endif
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

GLuint CScreensaverToys::createTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat)
{
  std::string fullPath = kodi::GetAddonPath("/resources/" + file);

  kodi::Log(ADDON_LOG_INFO, "creating texture %s", fullPath.c_str());

  unsigned error;
  unsigned char* image;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, fullPath.c_str());
  if (error)
  {
    kodi::Log(ADDON_LOG_ERROR, "create texture error %u: %s\n", error, lodepng_error_text(error));
    return 0;
  }

  GLuint texture = createTexture(image, GL_RGBA, width, height, internalFormat, scaling, repeat);
  free(image);
  return texture;
}

ADDONCREATOR(CScreensaverToys);
