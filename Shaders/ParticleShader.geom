#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

in VertexData {vec4 color;} inData[];
in gl_PerVertex {vec4 gl_Position;} gl_in[];

out FragmentData {
    vec4 color;
    vec2 texCoord;
} outData;

uniform float radius;
uniform float windowRatio;

void main() {    

    gl_Position = gl_in[0].gl_Position+vec4(-radius/windowRatio,radius,0.0,0.0); 
    outData.color = inData[0].color;
    outData.texCoord = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+vec4(-radius/windowRatio,-radius,0.0,0.0); 
    outData.color = inData[0].color;
    outData.texCoord = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+vec4(radius/windowRatio,radius,0.0,0.0); 
    outData.color = inData[0].color;
    outData.texCoord = vec2(1.0, 1.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+vec4(radius/windowRatio,-radius,0.0,0.0); 
    outData.color = inData[0].color;
    outData.texCoord = vec2(1.0, 0.0);
    EmitVertex();

    EndPrimitive();
}  