// Taken from https://www.shadertoy.com/view/4dlGR2

// by @301z
// noise 3D by by inigo quilez

float fbm(vec2 n) {
	float total = 0.0;
	for (float i = 0.; i < 7.; i++) {
		total += texture2D( iChannel0, n/256.0 ).x * pow(0.5, i);
		n += n;
	}
	return total;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	const vec3 c1 = vec3(0.1f, 0.0f, 0.0f);
	const vec3 c2 = vec3(0.7f, 0.0f, 0.0f);
	const vec3 c3 = vec3(0.2f, 0.0f, 0.0f);
	const vec3 c4 = vec3(1.0f, 0.9f, 0.0f);
	const vec3 c5 = vec3(0.1f, 0.1f, 0.1f);
	const vec3 c6 = vec3(0.9f, 0.9f, 0.9f);
	vec2 p = fragCoord.xy * 8.0f / iResolution.xx;
	float q = fbm(p - iGlobalTime * 0.1f);
	vec2 r = vec2(fbm(p + q + iGlobalTime * 0.7f - p.x - p.y), fbm(p + q - iGlobalTime * 0.4f));
	vec3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);
	fragColor = vec4(c * -cos(1.57f * fragCoord.y / iResolution.y), 1.0f);
}