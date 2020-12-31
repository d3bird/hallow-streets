#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 fColor;

uniform vec3 colors[10];
uniform int color_bounds[9];

uniform vec3 test;

void main(){

    if(gl_InstanceID >= 0 && gl_InstanceID < color_bounds[0]){
        fColor = colors[0];//plains
    }else if(gl_InstanceID >= color_bounds[0] && gl_InstanceID < color_bounds[1]){
        fColor = colors[1];//forest
    }else {
        fColor = colors[2];//water
    }

    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
}