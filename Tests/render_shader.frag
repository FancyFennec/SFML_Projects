//#version 330

uniform sampler2D normalMap;
uniform sampler2D layerTex;

uniform vec3 lightPos;

void main()
{
	vec3 ambient = vec3(0.5f, 0.0f, 0.2f);

	vec3 cameraPos = vec3(0.0f, 0.0f, 800.0f);
	float shininess = 64.0f;
	float specularIntensity = 1.0f;
	float ambientIntensity = 1.0f;
	float diffuseIntensity = 1.0f;

	vec2 pixelPos = gl_TexCoord[0].xy;
    vec4 normalPixel = texture2D(normalMap, pixelPos);
	vec4 layerPixel = texture2D(layerTex, pixelPos);
	vec3 pixelCol = layerPixel.xyz;

	ambient *= ambientIntensity * pixelCol;
	
	vec3 lightDir = normalize(lightPos - vec3(pixelPos, 0.0f));
	vec3 cameraDir = normalize(cameraPos - vec3(pixelPos, 0.0f));
	vec3 normal = normalize(2.0f * normalPixel.xyz - 1.0f);

	float diffuseScaling = diffuseIntensity * max(dot(lightDir, normal), 0.0f);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularScaling = specularIntensity * pow(max(dot(reflectDir, cameraDir), 0.0f), shininess);
	
	vec3 result = pixelCol * diffuseScaling + ambient + specularScaling;

	gl_FragColor = vec4(result, normalPixel.a);
}