#pragma once

#include <future>

#include "HeightmapGenerator.h"

namespace Geophagia {

class ErosionGenerator : public HeightmapGenerator {
public:
    ErosionGenerator() = default;
    ErosionGenerator(Terrain *terrain);
    virtual ~ErosionGenerator() override = default;

    void uiRender() override;
    void update();

    void generateHeightmap();

private:
    // simulation parameters
    float _deltaTime = 0.005f; ///< @brief Simulation time step
    float _rainIntensity = 0.015f;
    float _sedimentCapacity = 1.0f;
    float _erosionConstant = 0.5f;
    float _depositionConstant = 0.5f;
    float _evaporationConstant = 0.005f;

    // simulation data
    std::vector<float> _heightmap;
    std::vector<float> _waterHeight;
    std::vector<float> _suspendedSedimentAmount;
    std::vector<glm::vec4> _outflowFlux;
    std::vector<glm::vec2> _velocity;

    // multithreading data
    std::future<void> _simulationTask;
    std::atomic<bool> _isSimulationRunning;
    /**
     * @brief Temporary heightmap buffer for safe communication between
     * the working thread and the main thread that uploads the heightmap to the gpu
     */
    std::vector<float> _heightmapB;
    std::atomic<bool> _updateFlag;

    // simulation step methods
    void _applyRainfall(float dt);
    void _computeFlow(float dt);
    void _computeErosionDeposition(float dt);
    void _transportSediment(float dt);
    void _applyEvaporation(float dt);

    [[nodiscard]]
    float _sampleSediment(float x, float y) const;
    void _init();
};
}
