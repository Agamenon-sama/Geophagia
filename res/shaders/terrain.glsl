#vertex

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;


out vec3 Normal;
out vec3 FragPos;
out vec2 texCoord;

out float elevation;

void main() {
    texCoord = a_texCoord;
    gl_Position = u_projection * u_view /** u_model*/ * vec4(a_pos.x, a_pos.y / 4.f, a_pos.z, 1.0f);
    FragPos = vec3(/*u_model **/ vec4(a_pos, 1.f));
    Normal = /*mat3(transpose(inverse(u_model))) **/ a_normal;
    elevation = a_pos.y / 80.f;
}

// ================================
// ===========FRAGMENT=============
// ================================

#fragment

out vec4 FragColor;

in vec2 texCoord;
in vec3 FragPos;
in vec3 Normal;

in float elevation;

uniform sampler2D tex;

void main() {
    FragColor = vec4(elevation * vec3(0.298f, 0.118f, 0.32f), 1.f);
}