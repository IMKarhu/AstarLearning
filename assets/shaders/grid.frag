#version 460 core

#define MAX_SPOTLIGHTS 3

in vec3 outColor;
in vec3 worldPos;
in vec3 wnormal;
in vec2 UV;

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutoff;
};

uniform sampler2D tex;
uniform bool isGlowing;
uniform bool useTexture;
uniform float t;
uniform SpotLight spotlights[MAX_SPOTLIGHTS];
uniform int spotCount;
uniform vec3 ambient;

out vec4 fragColor;


vec3 applySpotlight(SpotLight light, vec3 normal, vec3 worldPos)
{
    vec3 lightDir = normalize(light.position - worldPos);
    float diff = max(dot(normal, lightDir), 0.0);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = 0.1;
    float intensity = smoothstep(light.cutoff, light.cutoff + epsilon, theta);

    return light.color * diff * intensity;
}

void main()
{
    vec4 color = vec4(0.0);
    if (isGlowing && !useTexture)
    {
        color = vec4(outColor, 1.0);
        float pulse = (sin(t * 5.0) + 1.0) * 0.5;
        float strength = 0.4 + pulse * 0.6;

        color.xyz *= strength;

        float dist = length(UV - vec2(0.5));
        float halo = smoothstep(0.6, 0.1, dist);

        color.xyz += halo * 0.5;
    }
    else
    {
        color = texture(tex, UV);
    }

    vec3 baseColor = color.rgb;
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < spotCount; i++)
    {
        Lo += applySpotlight(spotlights[i], normalize(wnormal), worldPos);
    }

    vec3 result = baseColor * (ambient + Lo);

    fragColor = vec4(result, 1.0);
}