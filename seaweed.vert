#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float seaweedPhase;

void main() {
    float h        = aTexCoords.y;
    float sway     = h * h;

    float swayX    = sin(time * 1.3 + seaweedPhase) * 0.18 * sway
    + sin(time * 2.1 + seaweedPhase * 1.7) * 0.06 * sway;
    float swayZ    = cos(time * 1.0 + seaweedPhase * 0.8) * 0.08 * sway;

    vec3 swayedPos = aPos + vec3(swayX, 0.0, swayZ);

    FragPos    = vec3(model * vec4(swayedPos, 1.0));
    Normal     = mat3(transpose(inverse(model))) * aNormal;
    TexCoords  = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
