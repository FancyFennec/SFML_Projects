//#version 430

uniform sampler2D normalMap;
uniform sampler2D layerTex;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform float shininess;
uniform float specInt;
uniform float ambInt;
uniform float difInt;

void main()
{
	vec3 cameraPos = vec3(0.0f, 0.0f, 800.0f);

	float total = specInt + ambInt + difInt;
	float specularIntensity = specInt / total;
	float ambientIntensity = ambInt / total;
	float diffuseIntensity = difInt / total;

	vec2 pixelPos = gl_TexCoord[0].xy;
    vec4 normalPixel = texture2D(normalMap, pixelPos);
	vec4 layerPixel = texture2D(layerTex, pixelPos);
	vec3 pixelCol = layerPixel.xyz;

	vec3 lightDir = normalize(lightPos - vec3(pixelPos, 0.0f));
	vec3 cameraDir = normalize(cameraPos - vec3(pixelPos, 0.0f));
	vec3 normal = normalize(2.0f * normalPixel.xyz - vec3(1.0f));

	vec3 ambient = lightCol * ambientIntensity;
	vec3 diffuse = lightCol * diffuseIntensity * max(dot(lightDir, normal), 0.0f);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = lightCol * specularIntensity * pow(max(dot(reflectDir, cameraDir), 0.0f), shininess);
	
	vec3 result = (diffuse + ambient + specular) * pixelCol;

	gl_FragColor = vec4(result, normalPixel.a);
}