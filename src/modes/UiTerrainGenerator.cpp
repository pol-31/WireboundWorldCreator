#include "UiTerrainGenerator.h"

void UiTerrainGenerator::ErodeWithFlow(std::vector<std::vector<float>>& heightmap,
                   const std::vector<std::vector<Vec2>>& flow_dir,
                   const std::vector<std::vector<float>>& water_accum,
                   int cycles) {
  Vec2 dim = Vec2(heightmap.size(), heightmap[0].size());
  float dt = 1.2f;
  float density = 1.0f;
  float evapRate = 0.001f;
  float depositionRate = 0.1f;
  float minVol = 0.01f;
  float friction = 0.05f;

  for(int i = 0; i < cycles; i++) {
    // Spawn droplet at a random position weighted by water accumulation to spawn more where water is abundant
    Vec2 newpos;
    {
      // Example: choose position randomly, but biased by water_accum
      // For simplicity, just random now
      newpos = Vec2(rand() % (int)dim.x, rand() % (int)dim.y);
    }

    Particle drop(newpos);
    drop.volume = water_accum[(int)newpos.y][(int)newpos.x];  // Use water accumulation as initial volume

    while(drop.volume > minVol) {
      Vec2i ipos{static_cast<int>(drop.pos.x), static_cast<int>(drop.pos.y)};
      if (ipos.x < 0 || ipos.x >= (int)dim.x || ipos.y < 0 || ipos.y >= (int)dim.y)
        break;

      // Instead of updating speed by surface normal, move drop along flow_dir vector at current position
      Vec2 dir = flow_dir[ipos.y][ipos.x];  // assuming [row][col] access
      // Apply velocity along flow direction scaled by dt and drop volume and maybe some factor
      drop.speed.x = dir.x * dt * drop.volume;
      drop.speed.y = dir.y * dt * drop.volume;

      drop.pos.x += drop.speed.x;
      drop.pos.y += drop.speed.y;

      // Compute sediment capacity difference as before
      int x1 = ipos.x;
      int y1 = ipos.y;
      int x2 = (int)drop.pos.x;
      int y2 = (int)drop.pos.y;

      if (x2 < 0 || x2 >= (int)dim.x || y2 < 0 || y2 >= (int)dim.y)
        break;

      float heightDiff = heightmap[y1][x1] - heightmap[y2][x2];
      float maxSediment = drop.volume * Vec2::length(drop.speed) * std::max(0.0f, heightDiff);

      float sdiff = maxSediment - drop.sediment;

      // Deposit or erode sediment
      drop.sediment += dt * depositionRate * sdiff;
      heightmap[y1][x1] -= dt * drop.volume * depositionRate * sdiff;
      heightmap[y1][x1] = std::max(0.0f, heightmap[y1][x1]);

      // Evaporate
      drop.volume *= (1.0f - dt * evapRate);
    }
  }
}


Vec3 UiTerrainGenerator::surfaceNormal(std::vector<std::vector<float>>& heightmap, int i, int j){
  double scale = 16.0f * 16.0f;                  //"Physical" Height scaling of the map
  /*
    Note: Surface normal is computed in this way, because the square-grid surface is meshed using triangles.
    To avoid spatial artifacts, you need to weight properly with all neighbors.
  */
  i = std::clamp(i, 1, static_cast<int>(heightmap[0].size()) - 2);
  j = std::clamp(j, 1, static_cast<int>(heightmap.size()) - 2);
  Vec3 n = Vec3::normalize(Vec3(scale*(heightmap[i][j]-heightmap[i+1][j]), 1.0, 0.0)) * 0.15;  //Positive X
  n += Vec3::normalize(Vec3(scale*(heightmap[i-1][j]-heightmap[i][j]), 1.0, 0.0)) * 0.15;  //Negative X
  n += Vec3::normalize(Vec3(0.0, 1.0, scale*(heightmap[i][j]-heightmap[i][j+1]))) * 0.15;    //Positive Y
  n += Vec3::normalize(Vec3(0.0, 1.0, scale*(heightmap[i][j-1]-heightmap[i][j]))) * 0.15;  //Negative Y

  //Diagonals! (This removes the last spatial artifacts)
  n += Vec3::normalize(Vec3(scale*(heightmap[i][j]-heightmap[i+1][j+1])/sqrt(2), sqrt(2), scale*(heightmap[i][j]-heightmap[i+1][j+1])/sqrt(2))) * 0.1;    //Positive Y
  n += Vec3::normalize(Vec3(scale*(heightmap[i][j]-heightmap[i+1][j-1])/sqrt(2), sqrt(2), scale*(heightmap[i][j]-heightmap[i+1][j-1])/sqrt(2))) * 0.1;    //Positive Y
  n += Vec3::normalize(Vec3(scale*(heightmap[i][j]-heightmap[i-1][j+1])/sqrt(2), sqrt(2), scale*(heightmap[i][j]-heightmap[i-1][j+1])/sqrt(2))) * 0.1;    //Positive Y
  n += Vec3::normalize(Vec3(scale*(heightmap[i][j]-heightmap[i-1][j-1])/sqrt(2), sqrt(2), scale*(heightmap[i][j]-heightmap[i-1][j-1])/sqrt(2))) * 0.1;    //Positive Y

  return n;
}

void UiTerrainGenerator::ErodeWeathering(std::vector<std::vector<float>>& heightmap, int cycles){
  Vec2 dim = Vec2(1024, 1024);  //Size of the heightmap array
  //Erosion Steps
  //  int remaining = 200000;
  //  int erosionstep = 1000;

  //Particle Properties
  float dt = 1.2;
  float density = 1.0;  //This gives varying amounts of inertia and stuff...
  float evapRate = 0.001;
  //  float depositionRate = 1.0f;
  float depositionRate = 0.1;

  float minVol = 0.01;
  float friction = 0.05;

  for(int i = 0; i < cycles; i++){

    //Spawn New Particle
    Vec2 newpos = Vec2(rand()%(int)dim.x, rand()%(int)dim.y);
    Particle drop(newpos);
    std::cout << "cycle " << i << std::endl;

    //As long as the droplet exists...
    while(drop.volume > minVol){

      Vec2i ipos{static_cast<int>(drop.pos.x), static_cast<int>(drop.pos.y)};                   //Floored Droplet Initial Position
      Vec3 n = surfaceNormal(heightmap, ipos.x, ipos.y);  //Surface Normal at Position

      //Accelerate particle using newtonian mechanics using the surface normal.
      //F = ma, so a = F/m
      drop.speed.x += n.x * dt/(drop.volume*density);
      drop.speed.y += n.z * dt/(drop.volume*density);
      drop.pos.x   += drop.speed.x;
      drop.pos.y   += drop.speed.y;
      drop.speed.x *= (1.0-dt*friction);       //Friction Factor
      drop.speed.y *= (1.0-dt*friction);       //Friction Factor

      if (!(drop.pos.x >= 0.0f && drop.pos.y >= 0.0f &&
            drop.pos.x < dim.x && drop.pos.y < dim.y))
        break;

      //Compute sediment capacity difference
      float maxsediment = drop.volume*Vec2::length(drop.speed)*(heightmap[ipos.x][ipos.y]-heightmap[(int)drop.pos.x][(int)drop.pos.y]);
      if(maxsediment < 0.0) maxsediment = 0.0;
      float sdiff = maxsediment - drop.sediment;

      //Act on the Heightmap and Droplet!
      drop.sediment += dt*depositionRate*sdiff;
      heightmap[ipos.x][ipos.y] -= dt*drop.volume*depositionRate*sdiff;
      heightmap[ipos.x][ipos.y] = std::max(0.0f, heightmap[ipos.x][ipos.y]);

      //Evaporate the Droplet (Note: Proportional to Volume! Better: Use shape factor to make proportional to the area instead.)
      drop.volume *= (1.0-dt*evapRate);
    }
  }
}


Texture UiTerrainGenerator::ProcessErosion(std::vector<std::vector<float>>& height_map_data,
                       const std::vector<std::vector<Vec2>>& flow_dir,
                       const std::vector<std::vector<float>>& water_accum,
                       int iterations) {
  int height = height_map_data.size();
  int width = height_map_data[0].size();

  auto start_height_map_data = height_map_data;

  ErodeWithFlow(height_map_data, flow_dir, water_accum, iterations);

  // Convert float -> uint8 for saving
  std::vector<uint8_t> height_map_data_updated(width * height);
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = -height_map_data[i][j] + start_height_map_data[i][j];
      height_map_data_updated[i * width + j] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  stbi_write_png("erosion_wear.png", width, height, 1, height_map_data_updated.data(), width);
  //  Texture erosion_wear("erosion_wear.png", GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = height_map_data[i][j];
      height_map_data_updated[i * width + j] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  stbi_write_png("height_map_erosed.png", width, height, 1, height_map_data_updated.data(), width);
  Texture height_map_erosed("height_map_erosed.png", GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  return height_map_erosed;
}

Texture UiTerrainGenerator::ProcessThermalWeathering(std::vector<std::vector<float>>& height_map_data, int iterations, float talus = 0.02f) {
  int height = height_map_data.size();
  int width = height_map_data[0].size();

  auto start_height_map_data = height_map_data;

  ErodeWeathering(height_map_data, iterations);
  //  RunThermalWeathering(height_map_data, 50, 0.2f);

  // Convert float -> uint8 for saving
  std::vector<uint8_t> height_map_data_updated(width * height);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = height_map_data[i][j] - start_height_map_data[i][j];
      height_map_data_updated[i * width + j] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  stbi_write_png("weathering_wear.png", width, height, 1, height_map_data_updated.data(), width);
  //  Texture weathering_wear("weathering_wear.png", GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = height_map_data[i][j];
      height_map_data_updated[i * width + j] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  stbi_write_png("height_map_weathered.png", width, height, 1, height_map_data_updated.data(), width);
  Texture height_map_weathered("height_map_weathered.png", GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  return height_map_weathered;
}

Texture UiTerrainGenerator::GenerateSplatmap();

Texture UiTerrainGenerator::GenerateNmap(
    const Shader& shader_gen_nmap,
    const Texture& tex_hmap) {
  Texture nmap(1024, 1024, GL_RG8);
  shader_gen_nmap.Bind();
  glBindImageTexture(
      0, tex_hmap.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap.GetFormat());
  glBindImageTexture(
      1, nmap.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, nmap.GetFormat());
  glDispatchCompute(1024 / 8, 1024 / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  nmap.Store("normal_map.png", 2, GL_RG, GL_UNSIGNED_BYTE);
  return nmap;
}

Texture UiTerrainGenerator::GenerateSlope(
    const Shader& shader_gen_slope_map,
    const Texture& tex_hmap) {
  Texture slope_map(1024, 1024, GL_R8);
  shader_gen_slope_map.Bind();
  glBindImageTexture(
      0, tex_hmap.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap.GetFormat());
  glBindImageTexture(
      1, slope_map.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, slope_map.GetFormat());
  glDispatchCompute(1024 / 8, 1024 / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  slope_map.Store("slope_map.png", 1, GL_RED, GL_UNSIGNED_BYTE);
  return slope_map;
}

Texture UiTerrainGenerator::GenerateAo(
    const Shader& shader_gen_ao_map,
    const Texture& tex_hmap) {
  Texture ao_map(1024, 1024, GL_R8);
  shader_gen_ao_map.Bind();
  glBindImageTexture(
      0, tex_hmap.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap.GetFormat());
  glBindImageTexture(
      1, ao_map.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, ao_map.GetFormat());
  glDispatchCompute(1024 / 8, 1024 / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  ao_map.Store("ao_map.png", 1, GL_RED, GL_UNSIGNED_BYTE);
  return ao_map;
}

Texture UiTerrainGenerator::Perturbate(
    const Shader& shader_perturbate,
    const Texture& tex_hmap) {
  Texture tex_new_hmap(1024, 1024, GL_R8);
  shader_perturbate.Bind();
  glBindImageTexture(
      0, tex_hmap.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap.GetFormat());
  glBindImageTexture(
      1, tex_new_hmap.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_new_hmap.GetFormat());
  glDispatchCompute(1024 / 8, 1024 / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_new_hmap.Store("tex_new_hmap.png", 1, GL_RED, GL_UNSIGNED_BYTE);
  return tex_new_hmap;
}

void UiTerrainGenerator::ComputeFlowMaps(
    std::vector<std::vector<float>> height_map_data,
    std::vector<std::vector<Vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int height = height_map_data.size();
  int width = height_map_data[0].size();

  flow_dir.assign(height, std::vector<Vec2>(width, {0.0f, 0.0f}));
  flow_accum.assign(height, std::vector<float>(width, 1.0f)); // Start with 1 unit of water

  // Step 1: Flow direction
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      float h0 = height_map_data[y][x];
      float maxSlope = 0.0f;
      Vec2 best = {0.0f, 0.0f};

      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          if (dx == 0 && dy == 0) continue;

          float h1 = height_map_data[y + dy][x + dx];
          float drop = h0 - h1;
          float dist = std::sqrt(dx * dx + dy * dy);
          float slope = drop / dist;

          if (slope > maxSlope) {
            maxSlope = slope;
            best = { (float)dx, (float)dy };
          }
        }
      }

      float len = std::sqrt(best.x * best.x + best.y * best.y);
      if (len > 0.0f)
        flow_dir[y][x] = { best.x / len, best.y / len };
    }
  }

  // Step 2: Flow accumulation (simple topological sort by descending height)
  struct Cell {
    int x, y;
    float height;
  };
  std::vector<Cell> cells;
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      cells.push_back({x, y, height_map_data[y][x]});

  std::sort(cells.begin(), cells.end(), [](const Cell& a, const Cell& b) {
    return a.height > b.height;
  });

  for (auto& cell : cells) {
    int x = cell.x;
    int y = cell.y;
    int nx = x + std::round(flow_dir[y][x].x);
    int ny = y + std::round(flow_dir[y][x].y);
    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
      flow_accum[ny][nx] += 0.1f * flow_accum[y][x];
    }
  }
}

Texture UiTerrainGenerator::GenerateFlowMap(std::vector<std::vector<float>>& height_map_data,
                        std::vector<std::vector<Vec2>>& flow_dir,
                        std::vector<std::vector<float>>& flow_accum) {
  int height = height_map_data.size();
  int width = height_map_data[0].size();
  ComputeFlowMaps(height_map_data, flow_dir, flow_accum);

  //  int width = flow_accum[0].size();
  //  int height = flow_accum.size();

  // flow_dir: RGB -> (dx, dy, 0)
  std::vector<float> flow_dir_data(width * height * 3);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      int idx = (y * width + x) * 3;
      flow_dir_data[idx + 0] = flow_dir[y][x].x * 0.5f + 0.5f; // map [-1,1] to [0,1]
      flow_dir_data[idx + 1] = flow_dir[y][x].y * 0.5f + 0.5f;
      flow_dir_data[idx + 2] = 0.0f; // unused
    }

  // flow_accum: single channel
  std::vector<float> flow_accum_data(width * height);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      flow_accum_data[y * width + x] = std::log(1.0f + flow_accum[y][x]);  // tone-mapped

  GLuint flowDirTex, flowAccumTex;

  // Upload flowDirTex (RGB)
  glGenTextures(1, &flowDirTex);
  glBindTexture(GL_TEXTURE_2D, flowDirTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, flow_dir_data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Upload flowAccumTex (R)
  glGenTextures(1, &flowAccumTex);
  glBindTexture(GL_TEXTURE_2D, flowAccumTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, flow_accum_data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Convert float -> uint8 for saving
  std::vector<uint8_t> flowDirData_u8(width * height * 3);
  std::vector<uint8_t> flowAccumData_u8(width * height);

  for (int i = 0; i < width * height; ++i) {
    // Clamp and convert each channel
    for (int c = 0; c < 3; ++c) {
      float v = flow_dir_data[i * 3 + c];
      flowDirData_u8[i * 3 + c] = static_cast<uint8_t>(std::clamp(v, 0.0f, 1.0f) * 255.0f);
    }

    float acc = flow_accum_data[i];
    flowAccumData_u8[i] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
  }

  stbi_write_png("flowDirTex.png", width, height, 3, flowDirData_u8.data(), width * 3);
  stbi_write_png("flowAccumTex.png", width, height, 1, flowAccumData_u8.data(), width);
  return Texture{};
}

void UiTerrainGenerator::Bake(std::vector<unsigned char>& height_map_data,
          int width, int height, Texture& height_map) {
  std::vector<std::vector<float>> height_map_data_f(height, std::vector<float>(width));
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      height_map_data_f[y][x] = height_map_data[y * width + x] / 255.0f;

  std::vector<std::vector<Vec2>> flow_dir;
  std::vector<std::vector<float>> flow_accum;
  auto water_flow_map = GenerateFlowMap(height_map_data_f, flow_dir, flow_accum);

  /// erosion & thermal weathering
  auto hmap_map_erosed = ProcessErosion(
      height_map_data_f, flow_dir, flow_accum, 100000);
  auto hmap_map_weathered = ProcessThermalWeathering(
      height_map_data_f, 100000, 0.1f);

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      height_map_data[y * width + x] = static_cast<uint8_t>(height_map_data_f[y][x] * 255.0f);

  stbi_write_png("height_map_updated.png", width, height, 1, height_map_data.data(), width);
  height_map = Texture("height_map_updated.png", GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  Shader shader_gen_nmap("../generate_shaders/NormalMap.comp");
  auto nmap = GenerateNmap(shader_gen_nmap, height_map);

  Shader shader_gen_slope_map("../generate_shaders/SlopeMap.comp");
  auto slope_map = GenerateSlope(shader_gen_slope_map, height_map);

  Shader shader_gen_ao_map("../generate_shaders/AoMap.comp");
  auto ao_map = GenerateAo(shader_gen_ao_map, height_map);
}