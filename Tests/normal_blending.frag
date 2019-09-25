//#version 330

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec4 pixel1 = texture2D(texture1, gl_TexCoord[0].xy);
	vec4 pixel2 = texture2D(texture2, gl_TexCoord[0].xy);

	float alpha1 = pixel1.a;
	float alpha2 = pixel2.a;

	float alpha = alpha2 + alpha1 * (1.0f - alpha2);

	vec3 v1 = normalize(2.0f * pixel1.xyz - 1.0f);
	vec3 v2 = normalize(2.0f * pixel2.xyz - 1.0f);

	float angle = acos(dot(v1, v2)) * alpha2 / alpha;

	mat3 rot = mat3(
		cos(angle),	-sin(angle), 0,
		sin(angle), cos(angle), 0,
		0, 0, 1);

	vec3 v3 = cross(v1, v2);
	v3 = normalize(v3);

	v2 = cross(v1, v3);
	v2 = normalize(v2);

	mat3 trans = mat3(v1, v2, v3);
	
	gl_FragColor = vec4(trans * rot * transpose(trans) * v1 / 2.0f + 0.5f, alpha);
}