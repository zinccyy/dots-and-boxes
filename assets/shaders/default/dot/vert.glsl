#version 330 core

// input
layout(location = 0) in vec2 iPos;

// uniform
uniform mat4 uScale;
uniform mat4 uTranslate;

// output
out vec2 vertPos;

void main() {
    gl_Position = uTranslate * uScale * vec4(iPos, 0, 1);
    vertPos = iPos;
}