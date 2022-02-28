#version 330 core

// input
layout(location = 0) in vec2 iPos;

// uniform
uniform mat4 uScale;
uniform mat4 uTranslate;
uniform mat4 uOrtho;

void main() {
    gl_Position = uOrtho * uTranslate * uScale * vec4(iPos, 0, 1);
}