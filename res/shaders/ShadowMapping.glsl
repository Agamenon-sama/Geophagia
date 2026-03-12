#pragma vertex

layout (location = 0) in vec3 a_pos;

void main() {
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0f);
}

#pragma fragment

void main() {}
