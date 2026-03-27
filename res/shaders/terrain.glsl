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


uniform sampler2D u_grass;
uniform sampler2D u_rock;
uniform sampler2D u_snow;
uniform sampler2DShadow u_shadowMap;
uniform vec3 u_lightPos;
uniform bool u_isShadowEnabled;
uniform bool u_isBoxMappingEnabled;
uniform float u_verticalScale;

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

vec4 boxmap(sampler2D s, vec3 p, vec3 n) {
    vec4 x = texture(s, p.yz);
    vec4 y = texture(s, p.zx);
    vec4 z = texture(s, p.xy);

    vec3 weights = pow(abs(n), vec3(8.f));
    weights /= weights.x + weights.y + weights.z;

    return x * weights.x + y * weights.y + z * weights.z;
}

void main() {
    vec3 lightDirection = normalize(u_lightPos);
    vec3 normal = normalize(vary.normal);

    float ambient = 0.6f;
    float diffuse = 0.f;

    diffuse = max(0, dot(lightDirection, normal));

    float shadow = 1.f;
    if (u_isShadowEnabled) {
        shadow = inTheShadow(vary.fragPosLightSpace);
    }
    float brightness = shadow * diffuse + ambient;

    vec3 grass;
    vec3 rock;
    vec3 snow;

    if (u_isBoxMappingEnabled) {
        grass = boxmap(u_grass, vary.fragPos / 50.f, normal).rgb;
        rock = boxmap(u_rock, vary.fragPos / 50.f, normal).rgb;
        snow = boxmap(u_snow, vary.fragPos / 50.f, normal).rgb;
    }
    else {
        grass = texture(u_grass, vary.uvCoord).rgb;
        rock = texture(u_rock, vary.uvCoord).rgb;
        snow = texture(u_snow, vary.uvCoord).rgb;
    }

    float slope = dot(normal, vec3(0, 1.f, 0));
    slope = 1.f - slope; // 0 -> flat | 1 -> steep

    float yPos = vary.fragPos.y / u_verticalScale;

    float h1 = smoothstep(110.0, 200.0, yPos);
    float h2 = smoothstep(200.0, 240.0, yPos);

    // Base color from height
    vec3 baseColour = mix(grass, rock, h1);

    vec3 snowAdjusted = mix(snow, rock, slope);
    baseColour = mix(baseColour, snowAdjusted, h2);

    // apply rocks depending on the slope
    float slopeBlend = smoothstep(0.3, 0.8, slope);
    vec3 colour = mix(baseColour, rock, slopeBlend);


    FragColor = vec4(colour * brightness, 1.f);
    // FragColor = vec4(normal * brightness, 1.f);
}