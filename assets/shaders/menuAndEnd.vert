#version 460 core

const vec2 VERTICES[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
    );



out vec2 UV;

void main()
{
    gl_Position = vec4(VERTICES[gl_VertexID], 0.0, 1.0);
    UV = gl_Position.xy * 0.5 + 0.5;
    //UV.y = 1.0 - UV.y;
}