#version 330 core

layout (location=0) in vec2 inPosition;
layout (location=1) in vec3 inColor;

out VertexData {vec4 color;} outData;
out gl_PerVertex {vec4 gl_Position;};

uniform mat4 modelViewTransform;

void main(){
	gl_Position = modelViewTransform*vec4(inPosition,0.0,1.0);
	outData.color = vec4(inColor,1.0);
}