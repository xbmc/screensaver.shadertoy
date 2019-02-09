/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "GL.h"

#include <stdio.h>
#include <vector>
#include <string>

#include <kodi/AddonBase.h>
#include <kodi/Filesystem.h>

#define LOG_SIZE 1024
#define GLchar char

namespace kodi
{
namespace gui
{
namespace gl
{
//////////////////////////////////////////////////////////////////////
// CShader - base class
//////////////////////////////////////////////////////////////////////
class ATTRIBUTE_HIDDEN CShader
{
public:
  CShader() = default;
  virtual ~CShader() = default;
  virtual bool Compile(const std::string& extraBegin = "",
                       const std::string& extraEnd = "") = 0;
  virtual void Free() = 0;
  virtual GLuint Handle() = 0;

  bool LoadSource(const std::string& file)
  {
    char buffer[16384];

    kodi::vfs::CFile source;
    if (!source.OpenFile(file))
    {
      kodi::Log(ADDON_LOG_ERROR, "CShader::%s: Failed to open file '%s'", __FUNCTION__, file.c_str());
      return false;
    }
    size_t len = source.Read(buffer, sizeof(buffer));
    m_source.assign(buffer);
    m_source[len] = 0;
    source.Close();
    return true;
  }

  bool OK() { return m_compiled; }

protected:
  std::string m_source;
  std::string m_lastLog;
  std::vector<std::string> m_attr;
  bool m_compiled = false;
};


//////////////////////////////////////////////////////////////////////
// CVertexShader
//////////////////////////////////////////////////////////////////////
class ATTRIBUTE_HIDDEN CVertexShader : public CShader
{
public:
  CVertexShader() = default;
  ~CVertexShader() override { Free(); }

  void Free() override
  {
    if (m_vertexShader)
      glDeleteShader(m_vertexShader);
    m_vertexShader = 0;
  }

  bool Compile(const std::string& vertexExtraBegin = "",
               const std::string& vertexExtraEnd = "") override
  {
    GLint params[4];

    Free();

    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    GLsizei count = 0;
    const char *sources[3];
    if (!vertexExtraBegin.empty())
      sources[count++] = vertexExtraBegin.c_str();
    sources[count++] = m_source.c_str();
    if (!vertexExtraEnd.empty())
      sources[count++] = vertexExtraEnd.c_str();

    glShaderSource(m_vertexShader, count, sources, nullptr);
    glCompileShader(m_vertexShader);
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, params);
    if (params[0] != GL_TRUE)
    {
      GLchar log[LOG_SIZE];
      glGetShaderInfoLog(m_vertexShader, LOG_SIZE, NULL, log);
      kodi::Log(ADDON_LOG_ERROR, "CVertexShader::%s: %s", __FUNCTION__, log);
      fprintf(stderr, "CVertexShader::%s: %s\n", __FUNCTION__, log);
      m_lastLog = log;
      m_compiled = false;
    }
    else
    {
      GLchar log[LOG_SIZE];
      glGetShaderInfoLog(m_vertexShader, LOG_SIZE, NULL, log);
      m_lastLog = log;
      m_compiled = true;
    }
    return m_compiled;
  }

  GLuint Handle() override { return m_vertexShader; }

protected:
  GLuint m_vertexShader = 0;
};

//////////////////////////////////////////////////////////////////////
// CPixelShader
//////////////////////////////////////////////////////////////////////
class ATTRIBUTE_HIDDEN CPixelShader : public CShader
{
public:
  CPixelShader() = default;
  ~CPixelShader() { Free(); }
  void Free() override
  {
    if (m_pixelShader)
      glDeleteShader(m_pixelShader);
    m_pixelShader = 0;
  }

  bool Compile(const std::string& fragmentExtraBegin = "",
               const std::string& fragmentExtraEnd = "") override
  {
    GLint params[4];

    Free();

    if (m_source.length()==0)
      return true;

    m_pixelShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLsizei count = 0;
    const char *sources[3];
    if (!fragmentExtraBegin.empty())
      sources[count++] = fragmentExtraBegin.c_str();
    sources[count++] = m_source.c_str();
    if (!fragmentExtraEnd.empty())
      sources[count++] = fragmentExtraEnd.c_str();

    glShaderSource(m_pixelShader, count, sources, 0);
    glCompileShader(m_pixelShader);
    glGetShaderiv(m_pixelShader, GL_COMPILE_STATUS, params);
    if (params[0] != GL_TRUE)
    {
      GLchar log[LOG_SIZE];
      glGetShaderInfoLog(m_pixelShader, LOG_SIZE, NULL, log);
      kodi::Log(ADDON_LOG_ERROR, "CPixelShader::%s: %s", __FUNCTION__, log);
      fprintf(stderr, "CPixelShader::%s: %s\n", __FUNCTION__, log);
      m_lastLog = log;
      m_compiled = false;
    }
    else
    {
      GLchar log[LOG_SIZE];
      glGetShaderInfoLog(m_pixelShader, LOG_SIZE, NULL, log);
      m_lastLog = log;
      m_compiled = true;
    }
    return m_compiled;
  }

  GLuint Handle() override { return m_pixelShader; }

protected:
  GLuint m_pixelShader = 0;
};

//////////////////////////////////////////////////////////////////////
// CShaderProgram
//////////////////////////////////////////////////////////////////////
class ATTRIBUTE_HIDDEN CShaderProgram
{
public:
  CShaderProgram()
  {
    m_pFP = new CPixelShader();
    m_pVP = new CVertexShader();
  }

  CShaderProgram(const std::string &vert, const std::string &frag)
  {
    m_pFP = new CPixelShader();
    m_pVP = new CVertexShader();
    CreateShader(vert, frag);
  }

  virtual ~CShaderProgram()
  {
    Free();
    delete m_pFP;
    delete m_pVP;
  }

  bool CreateShader(const std::string &vert, const std::string &frag)
  {
    if (!kodi::vfs::FileExists(vert) || !m_pVP->LoadSource(vert))
      return false;

    if (!kodi::vfs::FileExists(frag) || !m_pFP->LoadSource(frag))
      return false;

    return true;
  }

  bool Enable()
  {
    if (OK())
    {
      glUseProgram(m_shaderProgram);
      if (OnEnabled())
      {
        if (!m_validated)
        {
          // validate the program
          GLint params[4];
          glValidateProgram(m_shaderProgram);
          glGetProgramiv(m_shaderProgram, GL_VALIDATE_STATUS, params);
          if (params[0] != GL_TRUE)
          {
            GLchar log[LOG_SIZE];
            glGetProgramInfoLog(m_shaderProgram, LOG_SIZE, NULL, log);
            kodi::Log(ADDON_LOG_ERROR, "CShaderProgram::%s: %s", __FUNCTION__, log);
            fprintf(stderr, "CShaderProgram::%s: %s\n", __FUNCTION__, log);
          }
          m_validated = true;
        }
        return true;
      }
      else
      {
        glUseProgram(0);
        return false;
      }
      return true;
    }
    return false;
  }

  void Disable()
  {
    if (OK())
    {
      glUseProgram(0);
      OnDisabled();
    }
  }

  bool OK()
  {
    return m_ok;
  }

  void Free()
  {
    if (m_pVP)
      m_pVP->Free();
    if (m_pFP)
      m_pFP->Free();
    if (m_shaderProgram)
      glDeleteProgram(m_shaderProgram);
    m_shaderProgram = 0;
    m_ok = false;
  }

  CVertexShader* VertexShader()
  {
    return m_pVP;
  }
  CPixelShader* PixelShader()
  {
    return m_pFP;
  }

  bool CompileAndLink(const std::string& vertexExtraBegin = "",
                      const std::string& vertexExtraEnd = "",
                      const std::string& fragmentExtraBegin = "",
                      const std::string& fragmentExtraEnd = "")
  {
    GLint params[4];

    // free resources
    Free();

    // compiled vertex shader
    if (!m_pVP->Compile(vertexExtraBegin, vertexExtraEnd))
      return false;

    // compile pixel shader
    if (!m_pFP->Compile(fragmentExtraBegin, fragmentExtraEnd))
    {
      m_pVP->Free();
      return false;
    }

    // create program object
    if (!(m_shaderProgram = glCreateProgram()))
    {
      kodi::Log(ADDON_LOG_ERROR, "CShaderProgram::%s: Failed to create GL program", __FUNCTION__);
      goto error;
    }

    // attach the vertex shader
    glAttachShader(m_shaderProgram, m_pVP->Handle());
    glAttachShader(m_shaderProgram, m_pFP->Handle());

    // link the program
    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, params);
    if (params[0] != GL_TRUE)
    {
      GLchar log[LOG_SIZE];
      glGetProgramInfoLog(m_shaderProgram, LOG_SIZE, NULL, log);
      kodi::Log(ADDON_LOG_ERROR, "CShaderProgram::%s: %s", __FUNCTION__, log);
      fprintf(stderr, "CShaderProgram::%s: %s\n", __FUNCTION__, log);
      goto error;
    }

    m_validated = false;
    m_ok = true;
    OnCompiledAndLinked();
    return true;

  error:
    m_ok = false;
    Free();
    return false;
  }

  virtual void OnCompiledAndLinked() {};
  virtual bool OnEnabled() { return false; };
  virtual void OnDisabled() {};

  GLuint ProgramHandle() { return m_shaderProgram; }

protected:
  CVertexShader* m_pVP = nullptr;
  CPixelShader* m_pFP = nullptr;
  GLuint m_shaderProgram = 0;
  bool m_ok = false;
  bool m_validated = false;
};

}
}
}
