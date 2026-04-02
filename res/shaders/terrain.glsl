#pragma vertex

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in vec3 a_tangent;


out Varyings {
    vec4 fragPosLightSpace;
    vec3 normal;
    vec3 fragPos;
    vec2 uvCoord;
    mat3 TBN;
} vary;

uniform mat4 u_lightSpaceMatrix;

void main() {
    vary.fragPos = vec3(u_model * vec4(a_pos, 1.f));
    vary.normal = mat3(transpose(inverse(u_model))) * a_normal;
    vary.uvCoord = a_texCoord;
    vary.fragPosLightSpace = u_lightSpaceMatrix * vec4(vary.fragPos, 1.f);

    vec3 N = normalize(mat3(u_model) * a_normal);
    vec3 T = normalize(mat3(u_model) * a_tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));
    vary.TBN = mat3(T, B, N);


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
    mat3 TBN;
} vary;


uniform sampler2D u_grass;
uniform sampler2D u_rock;
uniform sampler2D u_snow;

uniform sampler2D u_grass_normal;
uniform sampler2D u_rock_normal;
uniform sampler2D u_snow_normal;

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
    weights /= (weights.x + weights.y + weights.z);

    return x * weights.x + y * weights.y + z * weights.z;
}

vec3 boxmapNormal(sampler2D normalMap, vec3 p, vec3 n) {
    vec3 nx = texture(normalMap, p.yz).xyz * 2.f - 1.f;
    vec3 ny = texture(normalMap, p.zx).xyz * 2.f - 1.f;
    vec3 nz = texture(normalMap, p.xy).xyz * 2.f - 1.f;

    // Transform each to world space
    vec3 worldNx = vec3(nx.z, nx.y, nx.x);
    vec3 worldNy = vec3(ny.x, ny.z, ny.y);

    vec3 worldNz = vec3(nz.x, nz.y, nz.z);

    vec3 weights = pow(abs(n), vec3(8.f));
    weights /= (weights.x + weights.y + weights.z);

    vec3 finalNormal = normalize(
        worldNx * weights.x +
        worldNy * weights.y +
        worldNz * weights.z
    );

    return finalNormal;
}

vec3 colourWithUV(float ambient, vec3 lightDirection, vec3 normal) {
    // sample textures
    vec3 grass = texture(u_grass, vary.uvCoord).rgb;
    vec3 rock = texture(u_rock, vary.uvCoord).rgb;
    vec3 snow = texture(u_snow, vary.uvCoord).rgb;

    // compute normals
    vec3 grassNormal = texture(u_grass_normal, vary.uvCoord).xyz;
    vec3 rockNormal = texture(u_rock_normal, vary.uvCoord).xyz;
    vec3 snowNormal = texture(u_snow_normal, vary.uvCoord).xyz;

    grassNormal = grassNormal * 2.f - 1.f;
    rockNormal = rockNormal * 2.f - 1.f;
    snowNormal = snowNormal * 2.f - 1.f;

    grassNormal = normalize(vary.TBN * grassNormal);
    rockNormal = normalize(vary.TBN * rockNormal);
    snowNormal = normalize(vary.TBN * snowNormal);

    // blend textures
    float slope = dot(normal, vec3(0, 1.f, 0));
    slope = 1.f - slope; // 0 -> flat | 1 -> steep

    float yPos = vary.fragPos.y / u_verticalScale;

    float h1 = smoothstep(110.f, 200.f, yPos);
    float h2 = smoothstep(200.f, 240.f, yPos);

    vec3 baseColour = mix(grass, rock, h1);
    vec3 baseNormal = mix(grassNormal, rockNormal, h1);

    vec3 snowAdjusted = mix(snow, rock, slope);
    baseColour = mix(baseColour, snowAdjusted, h2);
    vec3 snowAdjustedNormal = mix(snowNormal, rockNormal, slope);
    baseNormal = mix(baseNormal, snowAdjustedNormal, h2);

    // apply rocks depending on the slope
    float slopeBlend = smoothstep(0.3f, 0.8f, slope);
    vec3 colour = mix(baseColour, rock, slopeBlend);

    vec3 finalNormal = mix(baseNormal, rockNormal, slopeBlend);
    finalNormal = normalize(finalNormal);

    // compute lighting
    float diffuse = 0.f;

    diffuse = max(0, dot(lightDirection, finalNormal));

    float shadow = 1.f;
    if (u_isShadowEnabled) {
        shadow = inTheShadow(vary.fragPosLightSpace);
    }
    float brightness = shadow * diffuse + ambient;

    return colour * brightness;
}

vec3 colourWithBoxMapping(float ambient, vec3 lightDirection, vec3 normal) {
    // sample textures
    vec3 grass = boxmap(u_grass, vary.fragPos / 50.f, normal).rgb;
    vec3 rock = boxmap(u_rock, vary.fragPos / 50.f, normal).rgb;
    vec3 snow = boxmap(u_snow, vary.fragPos / 50.f, normal).rgb;

    // compute normals
    vec3 grassNormal = boxmapNormal(u_grass_normal, vary.fragPos / 50.f, normal);
    vec3 rockNormal  = boxmapNormal(u_rock_normal,  vary.fragPos / 50.f, normal);
    vec3 snowNormal  = boxmapNormal(u_snow_normal,  vary.fragPos / 50.f, normal);

    // blend textures
    float slope = dot(normal, vec3(0, 1.f, 0));
    slope = 1.f - slope; // 0 -> flat | 1 -> steep

    float yPos = vary.fragPos.y / u_verticalScale;

    float h1 = smoothstep(110.f, 200.f, yPos);
    float h2 = smoothstep(200.f, 240.f, yPos);

    vec3 baseColour = mix(grass, rock, h1);
    vec3 snowAdjusted = mix(snow, rock, slope);
    baseColour = mix(baseColour, snowAdjusted, h2);
    float slopeBlend = smoothstep(0.3f, 0.8f, slope);
    vec3 colour = mix(baseColour, rock, slopeBlend);

    // blend normals
    vec3 accumulatedNormal = vec3(0.f);
    float totalWeight = 0.f;

    float wGrass = (1.f - h1);
    float wRock  = h1;

    float wSnow = h2 * (1.f - slope);
    wRock += slope; // from slope blend

    // accumulate
    accumulatedNormal += grassNormal * wGrass;
    accumulatedNormal += rockNormal  * wRock;
    accumulatedNormal += snowNormal  * wSnow;

    totalWeight = wGrass + wRock + wSnow;

    // normalize once
    vec3 finalNormal = normalize(accumulatedNormal / totalWeight);

    // compute lighting
    float diffuse = 0.f;

    diffuse = max(0, dot(lightDirection, finalNormal));

    float shadow = 1.f;
    if (u_isShadowEnabled) {
        shadow = inTheShadow(vary.fragPosLightSpace);
    }
    float brightness = shadow * diffuse + ambient;

    return colour * brightness;
}

void main() {
    vec3 lightDirection = normalize(u_lightPos);
    vec3 normal = normalize(vary.normal);
    float ambient = 0.6f;
//    float diffuse = 0.f;
//
//    diffuse = max(0, dot(lightDirection, normal));
//
//    float shadow = 1.f;
//    if (u_isShadowEnabled) {
//        shadow = inTheShadow(vary.fragPosLightSpace);
//    }
//    float brightness = shadow * diffuse + ambient;

    vec3 colour;

    if (u_isBoxMappingEnabled) {
        colour = colourWithBoxMapping(ambient, lightDirection, normal);
    }
    else {
        colour = colourWithUV(ambient, lightDirection, normal);
    }

    FragColor = vec4(colour, 1.f);
//     FragColor = vec4(normal * brightness, 1.f);
}