#version 460 core

in vec2 fragOffset;
uniform float percent;
out vec4 fragColor;

void main()
{
    if (fragOffset.x > percent)
        discard;

    vec3 color = mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), percent);
    fragColor = vec4(color, 1.0);
}
