#version 330 core
layout (location = 0) in vec4 iVertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 uOrtho;

void main()
{
    gl_Position = uOrtho * vec4(iVertex.xy, 0.0, 1.0);
    TexCoords = iVertex.zw;
}  