/*
 * Screensavers from Shadertoy for Kodi DirectX 11
 * Copyright (c) 2015 Team Kodi
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

#pragma comment(lib, "d3dcompiler.lib")

#include "xbmc_scr_dll.h"
#include <p8-platform/util/timeutils.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "toys.h"

#define SAFE_RELEASE(p) do { if(p) { (p)->Release(); (p)=nullptr; } } while (0)
#define ALIGN(x, a) (((x)+(a)-1)&~((a)-1))
#define SHADER_SOURCE(...) #__VA_ARGS__

using namespace DirectX;

std::string g_pathPresets;

struct MYVERTEX {
  float x, y, z;
  float u, v;
};

typedef enum ShaderType {
  VERTEX_SHADER,
  PIXEL_SHADER
};

struct Preset {
  std::string name;
  std::string file;
  int channel[4];
};

struct Params {
  XMFLOAT3 iResolution;
  float    iGlobalTime;
  XMFLOAT4 iChannelTime;
  XMFLOAT4 iMouse;
  XMFLOAT4 iDate;
  float    iSampleRate;
  XMFLOAT3 iChannelResolution[4];
};

const std::vector<Preset> g_presets =
{
  { "Ball",                     "ball.frag.glsl",              99, -1, -1, -1 },
  { "Bleepy Blocks",            "bleepyblocks.frag.glsl",      99, -1, -1, -1 },
  { "Bob Scanline",             "blobandscsanline.glsl",       -1, -1, -1, -1 },
  { "Blobs",                    "blobs.frag.glsl",             -1, -1, -1, -1 },
  { "Blue Fire",                "bluefire.frag.glsl",          -1, -1, -1, -1 },
  { "Classic RGB Plasma",       "rgbplasma.frag.glsl",         -1, -1, -1, -1 },
  { "Dance Floor",              "dancefloor.frag.glsl",        -1, -1, -1, -1 },
  { "Effect Ringblobs",         "ringblobs.frag.glsl",         -1, -1, -1, -1 },
  //{ "Ether",                    "ether.frag.glsl",             -1, -1, -1, -1 },
  { "Fire",                     "fire.frag.glsl",              -1, -1, -1, -1 },
  { "Flaring 1",                "flaringtype1.frag.glsl",      -1, -1, -1, -1 },
  { "Flaring 2",                "flaringtype2.frag.glsl",      -1, -1, -1, -1 },
  { "Flaring 3",                "flaringtype3.frag.glsl",      -1, -1, -1, -1 },
  { "Flaring 4",                "flaringtype4.frag.glsl",      -1, -1, -1, -1 },
  { "Flaring 5",                "flaringtype5.frag.glsl",      -1, -1, -1, -1 },
  { "Interstellar",             "stellar.frag.glsl",           -1, -1, -1, -1 },
  { "Interweaving Sine bands",  "sinebands.frag.glsl",         -1, -1, -1, -1 },
  { "Julia Trap",               "juliatrap.frag.glsl",         -1, -1, -1, -1 },
  { "Layers Of Plasma",         "layersplasma.frag.glsl",      -1, -1, -1, -1 },
  { "Liquid Spectrum",          "liquidspectrum.frag.glsl",    -1, -1, -1, -1 },
  { "Noise Animation Electric", "noiseanimelectric.frag.glsl", -1, -1, -1, -1 },
  { "Noise Animation Flow",     "noiseanimflow.frag.glsl",     -1, -1, -1, -1 },
  { "Noise Animation Lava",     "noiseanimlava.frag.glsl",     -1, -1, -1, -1 },
  { "Overly Satisfying",        "overlysatisfying.frag.glsl",  -1, -1, -1, -1 },
  { "Plasma Circles",           "plasmacircles.frag.glsl",     -1, -1, -1, -1 },
  { "Plasma Triangles",         "plasmatriangle.frag.glsl",    -1, -1, -1, -1 },
  { "Plas.mAtrix. Spin",        "plasmatrixspin.frag.glsl",    -1, -1, -1, -1 },
  { "Pixellated Plasma",        "pixellated.frag.glsl",        -1, -1, -1, -1 },
  { "Silexarst Creation",       "silexarst.frag.glsl",         -1, -1, -1, -1 },
  { "Sky at Night",             "skyatnight.frag.glsl",        -1, -1, -1, -1 },
  { "Sphere",                   "sphere.frag.glsl",            99, -1, -1, -1 },
  { "Star Tunnel",              "startunnel.frag.glsl",        -1, -1, -1, -1 },
  { "Vector Field",             "vectorfield.frag.glsl",       -1, -1, -1, -1 },
  { "Venus",                    "venus.frag.glsl",             -1, -1, -1, -1 },
  { "Warp",                     "warp.frag.glsl",              99, -1, -1, -1 },
  { "Watch It Burn",            "watchitburn.frag.glsl",       -1, -1, -1, -1 },
  { "Water Caustic",            "watercaustic.frag.glsl",      -1, -1, -1, -1 },
  { "Worley Noise Waters",      "worleynoisewaters.frag.glsl"  -1, -1, -1, -1 },
  //"balloffire.frag.glsl",
  //"noiseanimwatery.frag.glsl",
  //"tweets.frag.glsl",
  //"wiggly.frag.glsl",
};

ID3D11Device*             g_pDevice      = nullptr;
ID3D11DeviceContext*      g_pContext     = nullptr;
ID3D11InputLayout*        g_pInputLayout = nullptr;
ID3D11Buffer*             g_pCBParams    = nullptr;
ID3D11Buffer*             g_pVBuffer     = nullptr;
ID3D11VertexShader*       g_pVShader     = nullptr;
ID3D11PixelShader*        g_pPShader     = nullptr;
ID3D11ShaderResourceView* g_pChannelView[4]    = { nullptr, nullptr, nullptr, nullptr };
ID3D11SamplerState*       g_pChannelSampler[4] = { nullptr, nullptr, nullptr, nullptr };
ID3D11ShaderResourceView* g_pNoiseView   = nullptr;
ID3D11ShaderResourceView* g_pBackBuffer  = nullptr;

Params                    g_cbParams;
bool                      g_bInitialized  = false;
int                       g_numberPresets = g_presets.size();
int                       g_currentPreset = 0;
int                       g_iWidth;
int                       g_iHeight;

float fCubicInterpolate(float y0, float y1, float y2, float y3, float t)
{
  float a0, a1, a2, a3, t2;

  t2 = t*t;
  a0 = y3 - y2 - y0 + y1;
  a1 = y0 - y1 - a0;
  a2 = y2 - y0;
  a3 = y1;

  return(a0*t*t2 + a1*t2 + a2*t + a3);
}

DWORD dwCubicInterpolate(DWORD y0, DWORD y1, DWORD y2, DWORD y3, float t)
{
  // performs cubic interpolation on a D3DCOLOR value.
  DWORD ret = 0;
  DWORD shift = 0;
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
    ret |= ((DWORD)(f * 255)) << shift;
    shift += 8;
  }
  return ret;
}

const std::string vsSource = SHADER_SOURCE(
  void main(in  float4 pos   : POSITION,  in float2      uv : TEXCOORD0,
            out float2 outUV : TEXCOORD0, out float4 outPos : SV_POSITION)
  { 
    outPos = pos; 
    outUV = uv; 
  }
);

std::string fsHeader = SHADER_SOURCE(
#define vec2 float2\n
#define vec3 float3\n
#define vec4 float4\n
#define ivec2 int2\n
#define ivec3 int3\n
#define ivec4 int4\n
#define mat2 float2x2\n
#define mat3 float3x3\n
#define mat4 float4x4\n
#define fract frac\n
  float atan(float y, float x)
{
  if (x == 0 && y == 0) x = 1;   // Avoid producing a NaN
  return atan2(y, x);
}
float mod(float x, float y)
{
  return x - y * floor(x / y);
}
float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}
float2 mod(float2 x, float y)
{
  return x - y * floor(x / y);
}
float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
float3 mod(float3 x, float y)
{
  return x - y * floor(x / y);
}
float4 mod(float4 x, float4 y)
{
  return x - y * floor(x / y);
}
float4 mod(float4 x, float y)
{
  return x - y * floor(x / y);
}
float3 mix(float3 x, float3 y, float a)
{
  return x*(1 - a) + y*a;
}
float mix(float x, float y, float a)
{
  return x*(1 - a) + y*a;
}
float mix(float x, float y, bool a)
{
  return a ? y : x;
}
float2 mix(float2 x, float2 y, bool2 a)
{
  return a ? y : x;
}
float3 mix(float3 x, float3 y, bool3 a)
{
  return a ? y : x;
}
float4 mix(float4 x, float4 y, bool4 a)
{
  return a ? y : x;
}
cbuffer cbParams : register(b0)
{
  float3    iResolution;
  float     iGlobalTime;
  float     iChannelTime[4];
  float4    iMouse;
  float4    iDate;
  float     iSampleRate;
  float3    iChannelResolution[4];
};
Texture2D iChannel0 : register(t0);
Texture2D iChannel1 : register(t1);
Texture2D iChannel2 : register(t2);
Texture2D iChannel3 : register(t3);
SamplerState iChannel0Samp : register(s0);
SamplerState iChannel1Samp : register(s1);
SamplerState iChannel2Samp : register(s2);
SamplerState iChannel3Samp : register(s3);
\n#define texture2D(x, uv) (##x##.Sample(##x##Samp, uv))\n
);

std::string fsFooter = SHADER_SOURCE(
  void main(in float2 uv : TEXCOORD0, out float4 color : SV_TARGET)
  {
    float2 fragCoord = uv;
    fragCoord.x *= (iResolution.x - 0.5);
    fragCoord.y *= (iResolution.y - 0.5);
    mainImage(color, fragCoord);
    color.w = 1.0;
  }
);

HRESULT compileShader(ShaderType shaderType, const char *shader, size_t srcSize, ID3DBlob** ppBlob)
{
  ID3DBlob* pErrors = nullptr;
  HRESULT hr;

  hr = D3DCompile(shader, srcSize, nullptr, nullptr, nullptr, "main",
    shaderType == PIXEL_SHADER ? "ps_4_0_level_9_3" : "vs_4_0_level_9_1",
    0, 0, ppBlob, &pErrors);

  if (FAILED(hr))
  {
    SAFE_RELEASE(pErrors);
    return hr;
  }

  SAFE_RELEASE(pErrors);
  return S_OK;
}

HRESULT createTexture(DXGI_FORMAT format, unsigned int w, unsigned int h, const void * data, size_t size, ID3D11Texture2D** ppTexture, ID3D11ShaderResourceView** ppView)
{
  HRESULT hr;

  CD3D11_TEXTURE2D_DESC texDesc(format, w, h, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  D3D11_SUBRESOURCE_DATA srData = { data, size };

  ID3D11Texture2D* tex = nullptr;
  hr = g_pDevice->CreateTexture2D(&texDesc, data ? &srData : nullptr, &tex);
  if (SUCCEEDED(hr) && tex != nullptr)
  {
    if (ppView != nullptr)
    {
      CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(tex, D3D11_SRV_DIMENSION_TEXTURE2D);
      hr = g_pDevice->CreateShaderResourceView(tex, &srvDesc, ppView);
      if (FAILED(hr))
      {
        tex->Release();
        return hr;
      }
    }
    if (ppTexture != nullptr)
      *ppTexture = tex;
  }

  return hr;
}

HRESULT InitDXStuff(ID3D11DeviceContext* pContex)
{
  HRESULT hr = S_OK;
  g_pContext = pContex;
  g_pContext->GetDevice(&g_pDevice);

  if (g_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_9_3)
    return S_FALSE;

  ID3DBlob* pVSBlob = nullptr;
  if (SUCCEEDED(compileShader(VERTEX_SHADER, vsSource.c_str(), vsSource.size(), &pVSBlob)))
  {
    hr = g_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVShader);
    if (SUCCEEDED(hr))
    {
      D3D11_INPUT_ELEMENT_DESC layout[] =
      {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      };
      hr = g_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pInputLayout);
    }
    SAFE_RELEASE(pVSBlob);
  }
  if (FAILED(hr))
    return hr;

  MYVERTEX verts[] =
  {
    { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f },
    { -1.0f,  1.0f, 0.0f, 0.0f, 1.0f },
    {  1.0f, -1.0f, 0.0f, 1.0f, 0.0f },
    {  1.0f,  1.0f, 0.0f, 1.0f, 1.0f },
  };
  CD3D11_BUFFER_DESC bDesc(sizeof(MYVERTEX) * 4, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
  D3D11_SUBRESOURCE_DATA srData = { verts };
  if (FAILED(hr = g_pDevice->CreateBuffer(&bDesc, &srData, &g_pVBuffer)))
    return hr;

  bDesc.ByteWidth = ALIGN(sizeof(Params), 16);
  bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bDesc.Usage = D3D11_USAGE_DYNAMIC;
  bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  srData.pSysMem = &g_cbParams;
  if (FAILED(hr = g_pDevice->CreateBuffer(&bDesc, &srData, &g_pCBParams)))
    return hr;

  D3D11_SAMPLER_DESC sampDesc;
  ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0; 
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
  g_pDevice->CreateSamplerState(&sampDesc, &g_pChannelSampler[0]);

  ID3D11DeviceContext* pCtx = nullptr;
  g_pDevice->GetImmediateContext(&pCtx);

  ID3D11Texture2D* pNoise = nullptr;
  int size = 256, zoom_factor = 1;
  createTexture(DXGI_FORMAT_B8G8R8A8_UNORM, size, size, nullptr, 0, &pNoise, &g_pNoiseView);

  // generate noise texture (from MilkDrop)
  if (pNoise)
  {
    D3D11_MAPPED_SUBRESOURCE r;
    if (FAILED(hr = pCtx->Map(pNoise, 0, D3D11_MAP_WRITE_DISCARD, 0, &r)))
    {
      SAFE_RELEASE(pCtx);
      return hr;
    }
    
    // write to the bits...
    DWORD* dst = (DWORD*)r.pData;
    int dwords_per_line = r.RowPitch / sizeof(DWORD);
    int RANGE = 256;
    for (int y = 0; y<size; y++) {
      LARGE_INTEGER q;
      QueryPerformanceCounter(&q);
      srand(q.LowPart ^ q.HighPart ^ rand());
      int x;
      for (x = 0; x<size; x++) {
        dst[x] = (((DWORD)(rand() % RANGE) + RANGE / 2) << 24) |
          (((DWORD)(rand() % RANGE) + RANGE / 2) << 16) |
          (((DWORD)(rand() % RANGE) + RANGE / 2) << 8) |
          (((DWORD)(rand() % RANGE) + RANGE / 2));
      }
      // swap some pixels randomly, to improve 'randomness'
      for (x = 0; x<256; x++)
      {
        int x1 = (rand() ^ q.LowPart) % size;
        int x2 = (rand() ^ q.HighPart) % size;
        DWORD temp = dst[x2];
        dst[x2] = dst[x1];
        dst[x1] = temp;
      }
      dst += dwords_per_line;
    }

    // smoothing
    if (zoom_factor > 1)
    {
      // first go ACROSS, blending cubically on X, but only on the main lines.
      DWORD* dst = (DWORD*)r.pData;
      for (int y = 0; y<size; y += zoom_factor)
        for (int x = 0; x<size; x++)
          if (x % zoom_factor)
          {
            int base_x = (x / zoom_factor)*zoom_factor + size;
            int base_y = y*dwords_per_line;
            DWORD y0 = dst[base_y + ((base_x - zoom_factor) % size)];
            DWORD y1 = dst[base_y + ((base_x) % size)];
            DWORD y2 = dst[base_y + ((base_x + zoom_factor) % size)];
            DWORD y3 = dst[base_y + ((base_x + zoom_factor * 2) % size)];

            float t = (x % zoom_factor) / (float)zoom_factor;

            DWORD result = dwCubicInterpolate(y0, y1, y2, y3, t);

            dst[y*dwords_per_line + x] = result;
          }

      // next go down, doing cubic interp along Y, on every line.
      for (int x = 0; x<size; x++)
        for (int y = 0; y<size; y++)
          if (y % zoom_factor)
          {
            int base_y = (y / zoom_factor)*zoom_factor + size;
            DWORD y0 = dst[((base_y - zoom_factor) % size)*dwords_per_line + x];
            DWORD y1 = dst[((base_y) % size)*dwords_per_line + x];
            DWORD y2 = dst[((base_y + zoom_factor) % size)*dwords_per_line + x];
            DWORD y3 = dst[((base_y + zoom_factor * 2) % size)*dwords_per_line + x];

            float t = (y % zoom_factor) / (float)zoom_factor;

            DWORD result = dwCubicInterpolate(y0, y1, y2, y3, t);

            dst[y*dwords_per_line + x] = result;
          }
    }

    pCtx->Unmap(pNoise, 0);
    pNoise->Release();
  }

  // get current backbuffer view
  ID3D11RenderTargetView* pRTView = nullptr;
  g_pContext->OMGetRenderTargets(1, &pRTView, nullptr);
  if (pRTView != nullptr)
  {
    ID3D11Resource* pResource = nullptr;
    pRTView->GetResource(&pResource);
    pRTView->Release();

    if (pResource == nullptr)
      return E_FAIL;

    ID3D11Texture2D* pRTTexture = nullptr;
    hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pRTTexture));
    if (FAILED(hr))
    {
      pResource->Release();
      return hr;
    }

    D3D11_TEXTURE2D_DESC texDesc;
    pRTTexture->GetDesc(&texDesc);
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    pRTTexture->Release();

    ID3D11Texture2D* pCopyTexture = nullptr;
    hr = g_pDevice->CreateTexture2D(&texDesc, nullptr, &pCopyTexture);
    if (FAILED(hr))
    {
      pResource->Release();
      return hr;
    }

    pCtx->CopyResource(pCopyTexture, pResource);
    pResource->Release();

    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(pCopyTexture, D3D11_SRV_DIMENSION_TEXTURE2D);
    hr = g_pDevice->CreateShaderResourceView(pCopyTexture, &srvDesc, &g_pBackBuffer);

    pCopyTexture->Release();
  }

  SAFE_RELEASE(pCtx);

  return hr;
}

HRESULT createPixelShader(const std::string &file, ID3D11PixelShader** ppPShader)
{
  HRESULT hr;
  ID3D11PixelShader* pPSShader = nullptr;

  std::ostringstream ss;
  ss << g_pathPresets << "/resources/" << file;
  std::string fullPath = ss.str();

  std::string msg = "Debug Print: Opening shader: " + fullPath + "\n";
  OutputDebugString(msg.c_str());

  FILE* f = fopen(fullPath.c_str(), "r");
  if (f != NULL)
  {
    std::string source;
    fseek(f, 0, SEEK_END);
    source.resize(ftell(f));
    fseek(f, 0, SEEK_SET);
    size_t read = fread(&source[0], 1, source.size(), f);
    fclose(f);

    std::ostringstream ss;
    ss << fsHeader << "\n" << source << "\n" << fsFooter;
    std::string fsSource = ss.str();

    ID3DBlob* pPSBlob = nullptr;
    hr = compileShader(PIXEL_SHADER, fsSource.c_str(), fsSource.size(), &pPSBlob);

    if (SUCCEEDED(hr) && pPSBlob != nullptr)
      g_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, ppPShader);

    SAFE_RELEASE(pPSBlob);
  }
  else
    return E_FAIL;

  return S_OK;
}

void unloadPreset()
{
  SAFE_RELEASE(g_pPShader);
  for (size_t i = 1; i < ARRAYSIZE(g_pChannelView); i++)
    if (g_pChannelView[i])
    {
      SAFE_RELEASE(g_pChannelSampler[i]);
      SAFE_RELEASE(g_pChannelView[i]);
    }
}

void loadPreset(int number)
{
  if (number >= 0 && number < g_numberPresets)
  {
    g_currentPreset = number;
    unloadPreset();
    if (FAILED(createPixelShader(g_presets[g_currentPreset].file, &g_pPShader)))
    {
      // TODO fallback to "default" shader
    }

    if (g_presets[g_currentPreset].channel[0] == -1)
      g_pChannelView[0] = g_pNoiseView;
    else if (g_presets[g_currentPreset].channel[0] == 99)
      g_pChannelView[0] = g_pBackBuffer;
    else
    {
      // TODO load texture
    }

    for (size_t i = 1; i < ARRAYSIZE(g_pChannelView); ++i)
    {
      if (g_presets[g_currentPreset].channel[i] != -1)
      {
        // TODO load texture
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////
// Kodi has loaded us into memory, we should set our core values
// here and load any settings we may have from our config file
//
extern "C" ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  SCR_PROPS* scrprops = (SCR_PROPS*)props;

  g_iWidth = scrprops->width;
  g_iHeight = scrprops->height;
  g_pathPresets = scrprops->presets;

  if (FAILED(InitDXStuff(reinterpret_cast<ID3D11DeviceContext*>(scrprops->device))))
    return ADDON_STATUS_PERMANENT_FAILURE;

  if (!g_bInitialized)
  {
    srand(time(nullptr));
    //loadPreset(27/*rand() % g_numberPresets*/);
    g_bInitialized = true;
  }

  return ADDON_STATUS_NEED_SETTINGS;
}

extern "C" void Start()
{
  loadPreset(g_currentPreset < 0 ? (rand() % g_numberPresets) : g_currentPreset);
}

extern "C" void Render()
{
  if (g_bInitialized) 
  {
    g_pContext->VSSetShader(g_pVShader, nullptr, 0);
    g_pContext->PSSetShader(g_pPShader, nullptr, 0);
    g_pContext->PSSetShaderResources(0, ARRAYSIZE(g_pChannelView), g_pChannelView);
    g_pContext->PSSetSamplers(0, ARRAYSIZE(g_pChannelSampler), g_pChannelSampler);

    float t = (float)P8PLATFORM::GetTimeMs() / 1000.0f;
    time_t now = time(NULL);
    tm *ltm = localtime(&now);
    float year = (float)(1900 + ltm->tm_year);
    float month = (float)ltm->tm_mon;
    float day = (float)ltm->tm_mday;
    float sec = (float)(ltm->tm_hour * 3600 + ltm->tm_min * 60 + ltm->tm_sec);

    g_cbParams.iResolution = XMFLOAT3((float)g_iWidth, (float)g_iHeight, (float)g_iWidth/(float)g_iHeight);
    g_cbParams.iGlobalTime = t;
    g_cbParams.iSampleRate = 0;
    g_cbParams.iChannelTime = XMFLOAT4(t, t, t, t);
    g_cbParams.iDate = XMFLOAT4(year, month, day, sec);

    D3D11_MAPPED_SUBRESOURCE res = {};
    if (SUCCEEDED(g_pContext->Map(g_pCBParams, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
    {
      memcpy(res.pData, &g_cbParams, sizeof(Params));
      g_pContext->Unmap(g_pCBParams, 0);
    }
    g_pContext->PSSetConstantBuffers(0, 1, &g_pCBParams);

    g_pContext->IASetInputLayout(g_pInputLayout);
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    size_t strides = sizeof(MYVERTEX), offsets = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_pVBuffer, &strides, &offsets);

    g_pContext->Draw(4, 0);

    g_pContext->PSSetShader(nullptr, nullptr, 0);
    g_pContext->VSSetShader(nullptr, nullptr, 0);
  }
}


extern "C" void Stop()
{
}

// Kodi tells us to stop the screensaver
// we should free any memory and release
// any resources we have created.
extern "C" void ADDON_Stop()
{
  unloadPreset();

  SAFE_RELEASE(g_pChannelSampler[0]);
  SAFE_RELEASE(g_pNoiseView);
  SAFE_RELEASE(g_pBackBuffer);

  SAFE_RELEASE(g_pDevice);
  SAFE_RELEASE(g_pInputLayout);
  SAFE_RELEASE(g_pCBParams);
  SAFE_RELEASE(g_pVBuffer);
  SAFE_RELEASE(g_pVShader);
  SAFE_RELEASE(g_pPShader);
}

void ADDON_Destroy()
{
}

ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

bool ADDON_HasSettings()
{
  return true;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void *value)
{
  if (!strSetting || !value)
    return ADDON_STATUS_UNKNOWN;

  if (strcmp(strSetting, "preset") == 0)
  {
    int c = atoi((char*)value);
    if (c < 0) c = 0;
    if (c > g_presets.size()) c = g_presets.size();
    g_currentPreset = c - 1;
  }

  return ADDON_STATUS_OK;
}

void ADDON_FreeSettings()
{
}

void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

void GetInfo(SCR_INFO *info)
{
}

