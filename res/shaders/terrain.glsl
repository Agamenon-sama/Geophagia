#vertex

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;


out vec3 Normal;
out vec3 FragPos;
out vec2 texCoord;

void main() {
    texCoord = a_texCoord;
    gl_Position = u_projection * u_view /** u_model*/ * vec4(a_pos.x, a_pos.y / 4.f, a_pos.z, 1.0f);
    FragPos = vec3(/*u_model **/ vec4(a_pos, 1.f));
    Normal = /*mat3(transpose(inverse(u_model))) **/ a_normal;
}

// ================================
// ===========FRAGMENT=============
// ================================

#fragment

out vec4 FragColor;

in vec2 texCoord;
in vec3 FragPos;
in vec3 Normal;


uniform sampler2D tex;

void main() {
    vec3 lightPos = vec3(2.f, 6.f, 8.f);
    vec3 lightDirection = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);

    float ambient = 0.4f;
    float diffuse = 0.f;

    diffuse = max(0, dot(lightDirection, normal));

    float brightness = diffuse + ambient;
    // vec3 colour = vec3(0.44f, 0.33, 0.23f);
    vec3 colour = texture(tex, texCoord).rgb;

    FragColor = vec4(colour * brightness, 1.f);
    // FragColor = vec4(normal, 1.f);
}