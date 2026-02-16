#version 330 core

out vec4 fragColor;

in vec2 tex;

uniform sampler2D texSampler;

void main() {
    fragColor = texture(texSampler, tex);
}