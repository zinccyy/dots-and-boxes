#version 330 core

// uniforms
uniform vec3 uColor;

// output
out vec4 oColor;

vec3 rgb(vec3 color) {
    return color / 255.f;
}

void main() {
    oColor = vec4(rgb(uColor), 1.0);
}