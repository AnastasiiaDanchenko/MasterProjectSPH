#version 330 core

in FragmentData {
    vec4 color;
    vec2 texCoord;
} inData;

layout(location = 0) out vec4 fColor;

uniform sampler2D alpha;
uniform sampler2D diffuse;

void main(){
	vec4 alphaValue = texture(alpha, inData.texCoord); 
    vec4 diffuseValue = texture(diffuse, inData.texCoord);
    fColor = inData.color*alphaValue*0.99 + inData.color*diffuseValue*0.01;
}