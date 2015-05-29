// based on https://www.shadertoy.com/view/4d2XzG#


vec2 getNewUV( vec2 uv, vec2 pos, float radius, float strength, out float dist)
{
	vec2 fromUVToPoint = pos - uv;
	dist = length( fromUVToPoint );
	
	float mag = (1.0 - (dist / radius)) * strength;
	mag *= step( dist, radius );
	
    vec2 newUV = uv + (mag * fromUVToPoint);
    newUV.x *= 0.5 * iResolution.y / iResolution.x; newUV.x += 0.5; 
    newUV.y = 1.0 - newUV.y; newUV.y *= .5;
	return newUV;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) 
{
	vec2 uv = (fragCoord.xy / iResolution.xy) * 2.0 - 1.0;
	uv.x *= iResolution.x / iResolution.y;
	
	float radius = 0.50;
	
	float ct = cos( iGlobalTime / 3.0 );
	float st0 = sin( iGlobalTime / 3.0 );
	float st1 = sin( iGlobalTime );
	
	vec2 origin = vec2( 0.0, 0.0 );
	float x = origin.x + ( ct * st0) * 2.70;
	float y = origin.y + ( st1 ) * 0.50;
	vec2 pos = vec2(x,y);
	
	float dist = 0.0;
	vec2 newUV = getNewUV( uv, pos, radius, 0.5, dist);
	
	float start = 0.42;
	float glowT = sin(iGlobalTime)*0.5 + 0.5;
	float outlineRadius = radius +  (1.0-glowT)*0.01 + (glowT * 0.08);
	float t = (outlineRadius - start) / max( (dist - start), 0.01);
    
    vec3 texCol = texture2D(iChannel0, newUV).xyz;
	vec3 color = texCol + mix( vec3( 0.00, 0.00, 0.00 ), texCol, t);
	
	fragColor = vec4(color*.1, 1.0);

}