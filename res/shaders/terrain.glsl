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
    vary.fragPos = vec3(/*u_model **/ vec4(a_pos.x, a_pos.y / 4.f, a_pos.z, 1.f));
    vary.normal = /*mat3(transpose(inverse(u_model))) **/ a_normal;
    vary.uvCoord = a_texCoord;
    vary.fragPosLightSpace = u_lightSpaceMatrix * vec4(vary.fragPos, 1.f);

    gl_Position = u_projection * u_view /** u_model*/ * vec4(a_pos.x, a_pos.y / 4.f, a_pos.z, 1.0f);
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
uniform sampler2D u_shadowMap;
uniform vec3 u_lightPos;

/**
    @return 0.f if fragment is in the shadow, else 1.f
*/
float inTheShadow(vec4 lightSpacePosition) {
    vec3 coord = lightSpacePosition.xyz / lightSpacePosition.w;
    coord = coord * 0.5f + 0.5f;

    float closestDepth = texture(u_shadowMap, coord.xy).r;
    float currentDepth = coord.z;

    return currentDepth > closestDepth ? 0.f : 1.f;
}

void main() {
    vec3 lightDirection = normalize(u_lightPos);
    vec3 normal = normalize(vary.normal);

    float ambient = 0.8f;
    float diffuse = 0.f;

    diffuse = max(0, dot(lightDirection, normal));

    float shadow = inTheShadow(vary.fragPosLightSpace);
    float brightness = shadow * diffuse + ambient;

    vec3 colour = texture(u_tex, vary.uvCoord).rgb;

    FragColor = vec4(colour * brightness, 1.f);
    // FragColor = vec4(normal * brightness, 1.f);
}