// Taken from https://www.shadertoy.com/view/MdlXz8

#define t iGlobalTime
mat2 m(float a){ float c = cos(a), s = sin(a); return mat2(c, -s, s, c); }
float map(vec3 p){
  p.xz = mul(p.xz, m(t*0.4)); p.xy = mul(p.xy, m(t*0.3));
  vec3 q = p*2. + t*1.;
  float tmp = sin(t*0.7);
  return length(p + vec3(tmp,tmp,tmp))*log(length(p) + 1.) + sin(q.x + sin(q.z + sin(q.y)))*0.5 - 1.;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord){
  vec2 p = fragCoord.xy / iResolution.y - vec2(.9, .5);
  vec3 cl = vec3(0., 0., 0.);
  float d = 2.5;
  for (int i = 4; i <= 6; i++)	{
    vec3 p1 = vec3(0, 0, 5.) + normalize(vec3(p.x,p.y, -1.))*d;
    float rz = map(p1);
    float f = clamp((rz - map(p1 + .1))*0.5, -.1, 1.);
    vec3 l = vec3(0.1, 0.3, .4) + vec3(5., 2.5, 3.)*f;
    cl = cl*l + (1. - smoothstep(0., 2.5, rz))*.7*l;
    d += min(rz, 1.);
  }
  fragColor = vec4(cl, 1.);
}
