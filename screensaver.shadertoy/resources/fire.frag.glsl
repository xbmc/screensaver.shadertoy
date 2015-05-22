// Taken from https://www.shadertoy.com/view/Xsl3zN

// by @301z

float fbm(vec2 n) {
	float total = 0.0;
	for (float i = 0.; i < 7.; i++) {
		total += texture2D( iChannel0, n/256.0 ).x * pow(0.5, i);
		n += n;
	}
	return total;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	const vec3 c1 = vec3(0.1, 0.0, 0.0);
	const vec3 c2 = vec3(0.7, 0.0, 0.0);
	const vec3 c3 = vec3(0.2, 0.0, 0.0);
	const vec3 c4 = vec3(1.0, 0.9, 0.0);
	const vec3 c5 = vec3(0.1, 0.1, 0.1);
	const vec3 c6 = vec3(0.9, 0.9, 0.9);
	vec2 p = fragCoord.xy * 8.0 / iResolution.xx;
	float q = fbm(p - iGlobalTime * 0.1);
	vec2 r = vec2(fbm(p + q + iGlobalTime * 0.7 - p.x - p.y), fbm(p + q - iGlobalTime * 0.4));
	vec3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);
	fragColor = vec4(c * cos(1.57 * fragCoord.y / iResolution.y), 1.0);
}