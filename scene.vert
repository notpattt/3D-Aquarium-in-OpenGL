#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4  model;
uniform mat4  view;
uniform mat4  projection;
uniform float time;
uniform bool  isSand;

void main() {
    vec3 pos = aPos;

    if (isSand) {
        float wave1 = sin(pos.x * 2.1 + time * 0.4) * 0.008;
        float wave2 = sin(pos.z * 1.8 + time * 0.35) * 0.007;
        float wave3 = sin(pos.x * 3.5 + pos.z * 2.2 + time * 0.55) * 0.005;
        pos.y += wave1 + wave2 + wave3;
    }

    FragPos   = vec3(model * vec4(pos, 1.0));
    Normal    = mat3(transpose(inverse(model))) * aNormal;

    if (isSand) {
        TexCoords = aTexCoords + vec2(time * 0.012, time * 0.008);
    } else {
        TexCoords = aTexCoords;
    }

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
