#pragma vertex

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;


out Varyings {
    vec4 fragPosLightSpace;
    vec3 normal;
    vec3 fragPos;
    vec2 uvCoord;
} vary;

uniform mat4 u_lightSpaceMatrix;

void main() {
    vary.fragPos = vec3(u_model * vec4(a_pos, 1.f));
    vary.normal = mat3(transpose(inverse(u_model))) * a_normal;
    vary.uvCoord = a_texCoord;
    vary.fragPosLightSpace = u_lightSpaceMatrix * vec4(vary.fragPos, 1.f);

    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0f);
}

// ================================
// ===========FRAGMENT=============
// ================================

#pragma fragment

out vec4 FragColor;

in Varyings {
    vec4 fragPosLightSpace;
    vec3 normal;
    vec3 fragPos;
    vec2 uvCoord;
} vary;


uniform sampler2D u_tex;
uniform sampler2DShadow u_shadowMap;
uniform vec3 u_lightPos;
uniform bool u_isShadowEnabled;

/**
    @return 0.f if fragment is in the shadow, else 1.f
*/
float inTheShadow(vec4 lightSpacePosition) {
    vec3 coord = lightSpacePosition.xyz / lightSpacePosition.w;
    coord = coord * 0.5f + 0.5f;

    float bias = 0.01f;
    float visibility = 0.f;
    vec2 texelSize = 1.f / textureSize(u_shadowMap, 0);

    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            visibility += texture(
                u_shadowMap,
                vec3(coord.xy + vec2(x, y) * texelSize, coord.z - bias)
            );
        }
    }

    return visibility / 9.f;
}

void main() {
    vec3 lightDirection = normalize(u_lightPos);
    vec3 normal = normalize(vary.normal);

    float ambient = 0.8f;
    float diffuse = 0.f;

    diffuse = max(0, dot(lightDirection, normal));

    float shadow = 1.f;
    if (u_isShadowEnabled) {
        shadow = inTheShadow(vary.fragPosLightSpace);
    }
    float brightness = shadow * diffuse + ambient;

    vec3 colour = texture(u_tex, vary.uvCoord).rgb;

    FragColor = vec4(colour * brightness, 1.f);
    // FragColor = vec4(normal * brightness, 1.f);
}