#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inuv;

out vec3 outColor;
out vec3 worldPos;
out vec3 wnormal;
out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(inPos, 1.0);
    worldPos = vec3(model * vec4(inPos, 1.0));
    outColor = inColor;
    wnormal = mat3(transpose(inverse(model))) * inNormal;
    UV = inuv;
}
