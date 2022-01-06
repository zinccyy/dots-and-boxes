#version 330 core

out vec4 oColor;

uniform vec3 uColor;

vec3 rgb(vec3 color) {
    return color / 255.f;
}

void main() {
    oColor = vec4(rgb(uColor), 1);
}