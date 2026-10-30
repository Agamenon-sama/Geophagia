#pragma vertex

layout (location = 0) in vec3 vPos;

out vec3 texCoord;

void main() {
    texCoord = vPos;
    // gl_Position = u_projection * u_view * u_translate * vec4(vPos, 1.0f);
    // gl_Position = u_projection * u_view * vec4(vPos, 1.0f);
    vec4 pos = u_projection * u_view * u_model * vec4(vPos, 1.0f);
    gl_Position = pos.xyww;
}

// ================================
// ===========FRAGMENT=============
// ================================

#pragma fragment

out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube tex;

void main() {
    // if (gl_FragCoord.x > 640) discard;

    FragColor = texture(tex, texCoord);
    // FragColor = vec4(texture(tex, texCoord).rgb, 0.5f);
}
