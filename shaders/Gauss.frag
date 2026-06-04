#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

layout (location = 0) uniform sampler2D image;
layout (location = 1) uniform bool horizontal;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

//TODO: add intensity factor OR replace:
// tight blur (lower σ)
//float weight[5] = float[](0.38774, 0.24477, 0.06136, 0.00420, 0.00005);
// wider blur (higher σ)
//float weight[5] = float[](0.17840, 0.16560, 0.13400, 0.09440, 0.05760);

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0); // получаем размер текселя
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // вклад текущего фрагмента
    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
