#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool applyFilter;

void main() {
    vec3 col = texture(screenTexture, TexCoords).rgb;
    if(applyFilter) {
        col = 1.0 - col;
    }
    FragColor = vec4(col, 1.0);
}