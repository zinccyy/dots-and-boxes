#version 330 core

out vec4 oColor;

vec3 rgb(vec3 color) {
    return color / 255.f;
}

void main() {
    oColor = vec4(rgb(vec3(69, 69, 69)), 1);
}