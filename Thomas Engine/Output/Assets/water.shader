#type vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vPosition;
out vec2 vTexCoords;
out vec3 vNormal;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1);

	vTexCoords = texCoords;
	vPosition = vec3(model * vec4(position, 1));
	vNormal = normalize((model * vec4(normal, 0.0)).xyz);
}


#type fragment
#version 430 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoords;

out vec4 fragColor;

uniform sampler2D tex;
uniform vec3 camPos;

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(vec3(1));
	float diff = max(dot(normal, lightDir), 0.0);
	return vec3(diff);
}

void main()
{
	vec3 norm = normalize(vNormal);
	vec3 viewDir = normalize(camPos - vPosition);

	//vec3 result = CalcDirLight(norm, viewDir);

	fragColor = texture(tex, vTexCoords);// * vec4(result, 1);
}
