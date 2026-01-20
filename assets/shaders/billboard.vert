#version 460 core

const vec2 OFFSETS[4] = vec2[](
    vec2(-0.5, 0.0),
    vec2(0.5, 0.0),
    vec2(-0.5, 0.1),
    vec2(0.5, 0.1)
    );

uniform mat4 view;
uniform mat4 proj;
uniform vec3 position;
out vec2 fragOffset;

void main()
{
    vec2 pos = OFFSETS[gl_VertexID];
    vec3 cameraRight = {view[0][0], view[1][0], view[2][0]};
    vec3 cameraUp = {view[0][1], view[1][1], view [2][1]};

    vec3 billPositionWorld = position.xyz
        + pos.x * cameraRight
        + pos.y * cameraUp
        + vec3(0.0, 1.5, 0.0);

    gl_Position = proj * view * vec4(billPositionWorld, 1.0);

    fragOffset = vec2(pos.x + 0.5, pos.y + 10.0);
}
