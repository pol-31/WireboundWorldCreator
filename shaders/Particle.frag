#version 460 core

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec4 color;
    flat uint materialId;
} fs_in;

layout (location = 0) uniform sampler2D uAlbedo;

void main() {
    vec4 texColor = texture(uAlbedo, fs_in.TexCoords);
    
//    if (texColor.a < 0.1f) {
//        discard;
//    }
    
    vec3 color = fs_in.color.rgb * texColor.rgb;
    float alpha = fs_in.color.a * texColor.a;
    
    FragColor = vec4(color, alpha);
//    FragColor = vec4(1.0f);
}
