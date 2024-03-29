#version 330 core

// input
layout(location = 0) in vec2 iPos;

// uniform
uniform mat4 uScale;
uniform mat4 uTranslate;
uniform mat4 uOrtho;

// output
out vec2 vertPos;

void main() {
    vec4 pos = uOrtho * uTranslate * uScale * vec4(iPos, 0, 1);
    vec4 scaled = vec4(pos.x, pos.y, 0, 1);
    gl_Position = scaled;
    vertPos = iPos;
}