#type vertex
#version 430 core


layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 vUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float waveSpeed;
uniform float waveSize;
uniform float waterHeight;
uniform float time;

out float vTime;

void main() {
    vUv = texCoords;
   // gl_Position = projection * view *  model * vec4(position, 1.0);

	vec3 newPosition = position;
    newPosition.y += sin((texCoords.x + time) * waveSpeed) * waveSize;
    newPosition.y = max(newPosition.y, waterHeight);
   gl_Position = projection * view *  model * vec4(newPosition, 1.0);
}


#type fragment
#version 430 core

in vec2 vUv;

out vec4 color;

uniform sampler2D tex;
in float vTime;

void main() {
    vec2 uv = vUv;
    uv.y += sin(uv.x * 10.0 + vTime) * 0.1;
    color = texture2D(tex, uv);
}
