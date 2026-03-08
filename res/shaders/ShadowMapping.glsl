#pragma vertex

layout (location = 0) in vec3 a_pos;

void main() {
    gl_Position = u_projection * u_view * vec4(a_pos.x, a_pos.y / 4.f, a_pos.z, 1.0f);
}

#pragma fragment

void main() {}
