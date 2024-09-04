#version 460 core

out vec4 out_color;

in TES_OUT {
    vec2 tc;
} fs_in;
// tex coords, position

void main(void) {
    out_color = vec4(0.0f, 0.0f, 0.3f, 1.0f);
}

// this is the code for PBR water for Unity, but we can use
// other BRDF model by Ross et al who proposed a physically-based
// anisotropic BRDF based on the surface slope variances

// here exaple of using Jacobians from ifft passes
/*
uniform sampler2D derivatives;
uniform sampler2D turbulence;

uniform float length_scale;
uniform float foam_scale;
uniform float contact_foam;

// --- --- --- ---

_Color("Color", Color) = (1,1,1,1)
        _SSSColor("SSS Color", Color) = (1,1,1,1)
        _SSSStrength("SSSStrength", Range(0,1)) = 0.2
        _SSSScale("SSS Scale", Range(0.1,50)) = 4.0
        _SSSBase("SSS Base", Range(-5,1)) = 0
        _LOD_scale("LOD_scale", Range(1,10)) = 0
        _MaxGloss("Max Gloss", Range(0,1)) = 0
        _Roughness("Distant Roughness", Range(0,1)) = 0
        _RoughnessScale("Roughness Scale", Range(0, 0.01)) = 0.1
        _FoamColor("Foam Color", Color) = (1,1,1,1)
        _FoamTexture("Foam Texture", 2D) = "grey" {}
        _FoamBiasLOD0("Foam Bias LOD0", Range(0,7)) = 1
        _FoamScale("Foam Scale", Range(0,20)) = 1
        _ContactFoam("Contact Foam", Range(0,1)) = 1
        
        struct Input
        {
            float2 worldUV;
            float4 lodScales;
            float3 viewVector;
            float3 worldNormal;
            float4 screenPos;
            INTERNAL_DATA
        };


        sampler2D _Displacement_c0;
        sampler2D _Derivatives_c0;
        sampler2D _Turbulence_c0;
        
        float LengthScale0;
        float LengthScale1;
        float LengthScale2;
        float _LOD_scale;
        float _SSSBase;
        float _SSSScale;
        

fixed4 _Color, _FoamColor, _SSSColor;
        float _SSSStrength;
        float _Roughness, _RoughnessScale, _MaxGloss;
        float _FoamBiasLOD0, _FoamScale, _ContactFoam;
        sampler2D _CameraDepthTexture;
        sampler2D _FoamTexture;

vec3 WorldToTangentNormalVector(Input IN, float3 normal) {
    vec3 t2w0 = WorldNormalVector(IN, float3(1, 0, 0));
    vec3 t2w1 = WorldNormalVector(IN, float3(0, 1, 0));
    vec3 t2w2 = WorldNormalVector(IN, float3(0, 0, 1));
    mat3 t2w = mat3(t2w0, t2w1, t2w2);
    return normalize(mul(t2w, normal));
}

float pow5(float f) {
    return f * f * f * f * f;
}

void main(void) {
    vec4 derivatives = tex2D(_Derivatives_c0, IN.worldUV / LengthScale0);

    vec2 slope = vec2(derivatives.x / (1 + derivatives.z),
    derivatives.y / (1 + derivatives.w));
    vec3 worldNormal = normalize(vec3(-slope.x, 1, -slope.y));

    o.Normal = WorldToTangentNormalVector(IN, worldNormal);

    float jacobian = tex2D(_Turbulence_c0, IN.worldUV / LengthScale0).x;
    jacobian = min(1, max(0, (-jacobian + _FoamBiasLOD0) * _FoamScale));

    vec2 screenUV = IN.screenPos.xy / IN.screenPos.w;
    float backgroundDepth =
    LinearEyeDepth(SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, screenUV));
    float surfaceDepth = UNITY_Z_0_FAR_FROM_CLIPSPACE(IN.screenPos.z);
    float depthDifference = max(0, backgroundDepth - surfaceDepth - 0.1);
    float foam = tex2D(_FoamTexture, IN.worldUV * 0.5 + _Time.r).r;
    jacobian += _ContactFoam * saturate(max(0, foam - depthDifference) * 5) * 0.9;

    o.Albedo = lerp(0, _FoamColor, jacobian);
    float distanceGloss = lerp(1 - _Roughness, _MaxGloss, 1 / (1 + length(IN.viewVector) * _RoughnessScale));
    o.Smoothness = lerp(distanceGloss, 0, jacobian);
    o.Metallic = 0;

    vec3 viewDir = normalize(IN.viewVector);
    vec3 H = normalize(-worldNormal + _WorldSpaceLightPos0);
    float ViewDotH = pow5(saturate(dot(viewDir, -H))) * 30 * _SSSStrength;

    float fresnel = dot(worldNormal, viewDir);
    fresnel = saturate(1 - fresnel);
    fresnel = pow5(fresnel);

    o.Emission = lerp(color * (1 - fresnel), 0, jacobian);
    out_color = lerp(_Color, saturate(_Color + _SSSColor.rgb * ViewDotH * IN.lodScales.w), IN.lodScales.z);
//    out_color = vec4(0.0f, 0.0f, 0.3f, 1.0f);
}
*/
