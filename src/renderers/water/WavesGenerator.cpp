#include "WavesGenerator.h"

WavesGenerator::WavesGenerator(const Paths& paths)
    : init_spectrum_shader_(paths.shader_ocean_init_spectrum),
      time_spectrum_shader_(paths.shader_ocean_time_spectrum),
      textures_merger_shader_(paths.shader_ocean_tex_merger),
      size_(256),
      fft_(paths, 256),
      noise_tex_(GaussianNoise{paths}.Generate(256)),
      cascade_near_(256, init_spectrum_shader_, time_spectrum_shader_,
                    textures_merger_shader_, fft_, noise_tex_),
      cascade_mid_(256, init_spectrum_shader_, time_spectrum_shader_,
                   textures_merger_shader_, fft_, noise_tex_),
      cascade_far_(256, init_spectrum_shader_, time_spectrum_shader_,
                   textures_merger_shader_, fft_, noise_tex_) {
  InitialiseCascades();
}

void WavesGenerator::BindTextures() {
  cascade_near_.BindTextures();
  // TODO; calculate lod here
}

void WavesGenerator::Update() {
  auto time = static_cast<float>(glfwGetTime());
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  cascade_near_.CalculateWavesAtTime(time, gDeltaTime);
  //    cascade_mid_.CalculateWavesAtTime(time, gDeltaTime);
  //    cascade_far_.CalculateWavesAtTime(time, gDeltaTime);
}

float WavesGenerator::GetWaterHeight(/*Vector3 position*/) {
  //    Color c = physicsReadback.GetPixelBilinear(
  //        position.x / lengthScale0, position.z / lengthScale0);
  return 0.0f;
}

void WavesGenerator::InitialiseCascades() {
  float boundary1 = 2 * std::numbers::pi_v<float> / length_scale_mid_ * 6.0f;
  float boundary2 = 2 * std::numbers::pi_v<float> / length_scale_far_ * 6.0f;
  cascade_near_.CalculateInitials(waves_settings_, length_scale_near_, 0.0001f, boundary1);
  cascade_mid_.CalculateInitials(waves_settings_, length_scale_mid_, boundary1, boundary2);
  cascade_far_.CalculateInitials(waves_settings_, length_scale_far_, boundary2, 9999);
}
