#version 460 core

out vec4 out_color;

// DEBUG
layout(location = 0) uniform sampler2D tex_displacement_near;
layout(location = 1) uniform sampler2D tex_displacement_mid;
layout(location = 2) uniform sampler2D tex_displacement_far;

layout (location = 3) uniform sampler2D tex_derivatives_near;
layout (location = 4) uniform sampler2D tex_derivatives_mid;
layout (location = 5) uniform sampler2D tex_derivatives_far;

layout (location = 6) uniform sampler2D tex_turbulence_near;
layout (location = 7) uniform sampler2D tex_turbulence_mid;
layout (location = 8) uniform sampler2D tex_turbulence_far;

layout(location = 9) uniform float scale_near;
layout(location = 10) uniform float scale_mid;
layout(location = 11) uniform float scale_far;

layout(location = 12) uniform vec3 sun_direction = vec3(0.4, -0.8, -0.9);
layout(location = 13) uniform vec3 sun_color = vec3(1.0, 0.95, 0.8);
layout(location = 14) uniform vec3 ambient_color = vec3(0.1, 0.1, 0.15);

layout (location = 15) uniform sampler2D tex_terrain_height;

layout(std140, binding = 1) uniform Matrices {
    mat4 transform;
    float dmap_depth;
    // Adding padding to ensure 16-byte alignment as per std140 layout rules.
    float padding[3];
};

in TES_OUT {
    vec2 tc;
} fs_in;
// tex coords, position

float scale_factor = 1000.0f;

void main() {
//    vec3 albedo = vec3(0.45f, 0.6f, 0.2f);
//    vec3 albedo = vec3(0.9f, 0.2f, 0.0f);
    vec3 albedo = vec3(0.2f, 0.2f, 1.0f);
    vec2 tc = fs_in.tc;
    vec4 derivatives = texture(tex_derivatives_near, scale_factor * tc / scale_near)
    + texture(tex_derivatives_mid, scale_factor * tc / scale_mid)
    + texture(tex_derivatives_far, scale_factor * tc / scale_far);

    vec2 slope = vec2(derivatives.x / (1 + derivatives.z),
    derivatives.y / (1 + derivatives.w));
    float factor = 1.0f;
    vec3 normal = normalize(vec3(-slope.x * factor, -slope.y * factor, 1.0f));

    float diffuse_factor = max(dot(normal.rbg, normalize(-sun_direction)), 0.0);
    vec3 lighting = ambient_color + sun_color * diffuse_factor;

    // --- ---- ---- ---- ----
    float jacobian = texture(tex_turbulence_near, scale_factor * tc).r
    + texture(tex_turbulence_mid, scale_factor * tc / (scale_mid + scale_near)).r
    + texture(tex_turbulence_far, scale_factor * tc / scale_far).r;
    float _FoamScale = 1.0f;
    jacobian -= 2.99987f;
//    jacobian -= 2.9995f;
    jacobian = min(1, max(0, (-jacobian) * _FoamScale));

    //TODO: we can use texture for foam, not just grey color
    float foam = 1.0f;
    float _ContactFoam = 1.0f;
    float depthDifference = 1.0f;
    float _FoamColor = 1.0;
    jacobian += _ContactFoam * clamp(max(0.0, foam - depthDifference) * 5.0, 0.0, 1.0) * 0.9;
    float foam_add = mix(0, _FoamColor, jacobian * 100000.0f);

//    float foam_add = 0.0f;
//    if (jacobian > 2.9995f) {
//        foam_add = 0.0f;
//    } else {
//        foam_add = 1.0f;
//    }
//    foam_add = clamp(jacobian + 0.6f, 0.0f, 1.0f);

    float terrain_height = texture(tex_terrain_height, tc).r * dmap_depth / 2.0f + 2.0f;
    float water_height = (texture(tex_displacement_near, scale_factor * tc / scale_near).g
    + texture(tex_displacement_mid, scale_factor * tc / scale_mid).g
    + texture(tex_displacement_far, scale_factor * tc / scale_far).g) * dmap_depth
    + 4.0f;

    float height_difference = water_height - terrain_height;
//    foam_add = max(foam_add, height_difference);

    float foam_shore_add = mix(1.0f, 0.0f, height_difference * 10.0f);
//    float foam_shore_add = clamp(height_difference * 10.0f, 0.0f, 1.0f);
//    if (height_difference < 0.1f) {
//        foam_add = 1.0f;
//    }

//out_color = vec4(mix(albedo * lighting + foam_add * 0.3f, 1.0f, height_difference), 1.0f);

    out_color = vec4(albedo * lighting + foam_shore_add * 0.04f + foam_add * 0.3f, 1.0f);
//TODO:
    //  foam_add = max(foam_add, height_difference);
}
