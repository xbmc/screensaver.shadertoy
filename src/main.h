/*
 *      Copyright (C) 2005-2020 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <kodi/addon-instance/Screensaver.h>
#include <kodi/gui/gl/Shader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Preset
{
  std::string name;
  std::string file;
  std::string channel[4];
};

class ATTRIBUTE_HIDDEN CScreensaverShadertoy
  : public kodi::addon::CAddonBase
  , public kodi::addon::CInstanceScreensaver
{
public:
  CScreensaverShadertoy();
  ~CScreensaverShadertoy() override = default;

  bool Start() override;
  void Stop() override;
  void Render() override;

private:
  void RenderTo(GLuint shader, GLuint effect_fb);
  void Launch(int preset);
  void LoadPreset(const std::string& shaderPath);
  void UnloadPreset();
  GLint LoadTexture(int number);
  void UnloadTextures();
  GLuint CreateTexture(GLint format, unsigned int w, unsigned int h, const GLvoid* data);
  GLuint CreateTexture(const GLvoid* data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat);
  GLuint CreateTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat);
  int DetermineBitsPrecision();
  double MeasurePerformance(const std::string& shaderPath, int size);

  static const std::vector<Preset> g_presets;
  int m_numberPresets = g_presets.size();

  bool m_initialized = false;
  int64_t m_initialTime = 0; // in ms
  int m_bitsPrecision = 0;
  int m_currentPreset = 0;

  GLint m_attrResolutionLoc = 0;
  GLint m_attrGlobalTimeLoc = 0;
  GLint m_attrChannelTimeLoc = 0;
  GLint m_attrMouseLoc = 0;
  GLint m_attrDateLoc = 0;
  GLint m_attrSampleRateLoc = 0;
  GLint m_attrChannelResolutionLoc = 0;
  GLint m_attrChannelLoc[4] = {0};
  GLuint m_channelTextures[4] = {0};

  kodi::gui::gl::CShaderProgram m_shadertoyShader;
  kodi::gui::gl::CShaderProgram m_displayShader;

  struct
  {
    GLuint vertex_buffer;
    GLuint attr_vertex_e;
    GLuint attr_vertex_r, uTexture;
    GLuint effect_fb;
    GLuint framebuffer_texture;
    GLuint uScale;
    int fbwidth, fbheight;
  } m_state;

  bool m_settingsUseOwnshader = false;
  std::string m_usedShaderFile;
  std::string m_shaderTextures[4];
};
