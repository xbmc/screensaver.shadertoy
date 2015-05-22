// Taken from https://www.shadertoy.com/view/XsSGDc

#define SIZE (iResolution.x/12.) // cell size in texture coordinates
#define ZOOM (2. *256./iResolution.x)
#define STRIP  1.
#define V_ADV  1.      
#define V_BOIL .5
#define t iGlobalTime

vec3 flow(vec2 uv) {
  vec2 iuv = floor(SIZE*(uv)+.5) / SIZE;
  vec2 fuv = 2.*SIZE*(uv - iuv);

  vec2 pos = .01*V_ADV*vec2(cos(t) + sin(.356*t) + 2.*cos(.124*t), sin(.854*t) + cos(.441*t) + 2.*cos(.174*t));
  vec3 tex = 2.*texture2D(iChannel0, iuv / (ZOOM*SIZE) - pos).rgb - 1.;
  float ft = fract(t*V_BOIL)*3.;
  if (ft<1.) tex = mix(tex.rgb, tex.gbr, ft);
  else if (ft<2.) tex = mix(tex.gbr, tex.brg, ft - 1.);
  else            tex = mix(tex.brg, tex.rgb, ft - 2.);
  return tex;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
  vec2 uv = fragCoord.xy / iResolution.y;
  vec3 col;

  vec2 iuv = floor(SIZE*(uv)+.5) / SIZE;
  vec2 fuv = 2.*SIZE*(uv - iuv);
  vec3 tex = flow(uv);
  float v = fuv.x*tex.x + fuv.y*tex.y;
  // v = length(fuv);
  v = sin(STRIP*v);
  float tmp = 1. - v*v*SIZE;
  col = vec3(tmp, tmp, tmp) * mix(tex, vec3(1., 1., 1.), .5);

  // col = tex;
  fragColor = vec4(col, 1.0);
}

