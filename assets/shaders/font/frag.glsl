#version 330 core

in vec2 TexCoords;

uniform sampler2D uTexSampler;
uniform vec3 uCharColor;

out vec4 oColor;

vec3 rgb(vec3 color) {
    return color / 255.0;
}

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTexSampler, TexCoords).r);
    oColor = vec4(rgb(uCharColor), 1.0) * sampled;
}  