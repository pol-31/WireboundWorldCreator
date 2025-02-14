#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESCASCADE_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESCASCADE_H_

#include "../../common/Shader.h"
#include "../../common/Texture.h"
#include "Ifft.h"
#include "WaterBiome.h"

class WavesCascade {
 public:
  explicit WavesCascade(int size);

  void CalculateInitials(float length_scale,
                         float cutoff_low, float cutoff_high);

  /// we've separated them to:
  /// - bind the same shader only once;
  /// - set memory barriers only 2+1 times - between each stage,
  ///   so we PackIfftData() for each layer, then barrier,
  ///   then ProcessIfft() for each layer, then barrier,
  ///   then UnPackIfftData() for each layer, then (maybe) barrier (+1);
  ///   otherwise we would need to set it 6+1 times

  void PackIfftData();
  void ProcessIfft(const Ifft& ifft);
  void UnPackIfftData();

  void SetLambda(float lambda);

  inline void BindDerivativesTex() const {
    derivatives_tex_.Bind();
  }
  inline void BindDisplacementTex() const {
    displacement_tex_.Bind();
  }
  inline void BindTurbulenceTex() const {
    turbulence_tex_.Bind();
  }

 private:
  int size_;
  float lambda_{1.0f};

  Texture init_spectrum_tex_;
  Texture precomputed_data_tex_;

  Texture buffer_tex_;
  Texture dxdz_tex_;
  Texture dydxz_tex_;
  Texture dyxdyz_tex_;
  Texture dxxdzz_tex_;

  Texture derivatives_tex_;
  Texture displacement_tex_;
  Texture turbulence_tex_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESCASCADE_H_
