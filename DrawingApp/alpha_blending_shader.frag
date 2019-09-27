

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float alpha;

void main()
{
    vec4 pixel1 = texture2D(texture1, gl_TexCoord[0].xy);
	vec4 pixel2 = texture2D(texture2, gl_TexCoord[0].xy);
	pixel2.a = pixel2.a * alpha;

	float delta = 0;
	vec3 col = vec3(255,255,255);

	if(!(pixel1.a == 0 && pixel2.a == 0)) {
		delta = pixel2.a + pixel1.a - pixel2.a * pixel1.a;
		col = (pixel2.a / delta) * pixel2.xyz + (pixel1.a * (1 - pixel2.a) / delta) * pixel1.xyz;
	}
	gl_FragColor = vec4(col, delta);
}