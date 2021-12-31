#version 330 core

// input
in vec2 vertPos;

// uniforms
uniform vec2 uPosition;
uniform vec3 uInnerColor;
uniform vec3 uOuterColor;
uniform float uInnerRadius;
uniform float uOuterRadius;
uniform bool uSmoothstep;
uniform bool uHovered;

// output
out vec4 oColor;

vec3 rgb(vec3 color) {
    return color / 255.f;
}

vec4 circle(vec2 pos) {
    float len = length(pos);

    vec4 innerColor = vec4(rgb(uInnerColor), 1);
    vec4 outerColor = vec4(rgb(uOuterColor), 1);

    if(uSmoothstep) {
        if(len <= uOuterRadius) {
            if(uHovered) {
                return mix(innerColor * 0.8, outerColor, smoothstep(uInnerRadius, uOuterRadius, len));
            } else {
                return mix(innerColor, outerColor, smoothstep(uInnerRadius, uOuterRadius, len));
            }
        }  else {
            return outerColor;
        }
    } else {
        if(len <= uInnerRadius) {
            return uHovered? innerColor * 0.5 : innerColor;
        } else if(len > uInnerRadius && len <= uOuterRadius) {
            return uHovered? outerColor * 0.5 : outerColor;
        } else {
            return vec4(0);
        }
    }
}

void main() {
    oColor = circle(vertPos);
    if(oColor.a < 0.1) {
        discard;
    }
}