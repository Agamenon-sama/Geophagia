#include "ErosionGenerator.h"

#include <random>

#include <imgui/imgui.h>

namespace Geophagia {

using namespace std::chrono_literals;

ErosionGenerator::ErosionGenerator(Terrain *terrain) : HeightmapGenerator(terrain), _isSimulationRunning(false), _updateFlag(false) {
    _init();
}

void ErosionGenerator::uiRender() {
    ImGui::Begin("Erosion Simulator");
        ImGui::InputScalar("Seed", ImGuiDataType_U64, &_seed);
        ImGui::SliderFloat("Time step", &_deltaTime, 0.0001f, 0.01f);
        ImGui::SliderFloat("Rain intensity", &_rainIntensity, 0.001f, 0.5f);
        ImGui::SliderFloat("Sediment capacity", &_sedimentCapacity, 0.1f, 3.f);
        ImGui::SliderFloat("Erosion constant", &_erosionConstant, 0.1f, 3.f);
        ImGui::SliderFloat("Deposition constant", &_depositionConstant, 0.1f, 3.f);
        ImGui::SliderFloat("Evaporation constant", &_evaporationConstant, 0.001f, 0.5f);

        bool isProcessing = _isSimulationRunning;
        if (isProcessing) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Run The Simulation")) {
            generateHeightmap();
        }
        if (isProcessing) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();

        if (!isProcessing) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("End The Simulation")) {
            _isSimulationRunning = false;
        }
        if (!isProcessing) {
            ImGui::EndDisabled();
        }

    ImGui::End();
}

void ErosionGenerator::_applyRainfall(float dt) {
    // float rainIntensity = 1.f; // param
    std::mt19937 mt(_seed);
    std::normal_distribution<float> dist(0.02f, .01f);
    auto rng = std::bind(dist, mt);

    // rain
    // for (size_t i = 0; i < _waterHeight.size(); i++) {
    //     float rt = std::max(0.f, rng());
    //
    //     _waterHeight[i] += dt * rt * _rainIntensity;
    // }

    auto distance = [this](float x, float y) {
        return std::sqrt((100-x)*(100-x) + (100-y)*(100-y));
    };

    // constant water source
    for (int y = 50; y < _terrain->getDepth() - 50; y++) {
        for (int x = 50; x < _terrain->getWidth() - 50; x++) {
            if (distance(x, y) < 15.f) {
                // if (distance(x, y) > 0.001f)
                //     _waterHeight[y * _terrain->getWidth() + x] += dt * 1.f / distance(x, y);
                // else
                    _waterHeight[y * _terrain->getWidth() + x] = dt * 1.f;
            }
        }
    }
}

void ErosionGenerator::_computeFlow(float dt) {
    const float PIPE_AREA = 1.f;
    const float GRAVITY = 9.81f;
    const float PIPE_LENGTH = 1.f;
    const float factor = dt * PIPE_AREA * GRAVITY / PIPE_LENGTH;

    const u32 width = _terrain->getWidth();
    const u32 depth = _terrain->getDepth();

    for (u32 y = 0; y < depth; y++) {
        for (u32 x = 0; x < width; x++) {
            const u32 i = y * width + x;
            // Δh of neighbour = h of current vertex - h of neighbour
            const float currentH = _heightmap[i] + _waterHeight[i];

            // h for neighbours
            // if neighbour on the edge, drain the water
            // else, calculate like previously
            const float hL = (x > 0) ? (_heightmap[i - 1] + _waterHeight[i - 1]) : 0.f;
            const float hR = (x < width - 1) ? (_heightmap[i + 1] + _waterHeight[i + 1]) : 0.f;
            const float hT = (y < depth - 1) ? (_heightmap[i + width] + _waterHeight[i + width]) : 0.f;
            const float hB = (y > 0) ? (_heightmap[i - width] + _waterHeight[i - width]) : 0.f;

            // update Fluxes
            _outflowFlux[i].x = std::max(0.f, _outflowFlux[i].x + factor * (currentH - hL));
            _outflowFlux[i].y = std::max(0.f, _outflowFlux[i].y + factor * (currentH - hR));
            _outflowFlux[i].z = std::max(0.f, _outflowFlux[i].z + factor * (currentH - hT));
            _outflowFlux[i].w = std::max(0.f, _outflowFlux[i].w + factor * (currentH - hB));

            // scaling factor (K) to prevent over-draining
            const float sumFlux = _outflowFlux[i].x + _outflowFlux[i].y + _outflowFlux[i].z + _outflowFlux[i].w;
            if (sumFlux > 0) {
                const float K = std::min(1.f, _waterHeight[i] / (sumFlux * dt));
                _outflowFlux[i] *= K;
            }
        }
    }

    // calculate the new water levels
    for (u32 y = 0; y < depth; y++) {
        for (u32 x = 0; x < width; x++) {
            const u32 i = y * width + x;

            const float flowL = (x > 0) ? _outflowFlux[i - 1].y : 0.f;
            const float flowR = (x < width - 1) ? _outflowFlux[i + 1].x : 0.f;
            const float flowT = (y < depth - 1) ? _outflowFlux[i + width].w : 0.f;
            const float flowB = (y > 0) ? _outflowFlux[i - width].z : 0.f;

            const float newWaterHeight = _waterHeight[i] + dt * (
                flowL + flowR + flowT + flowB -
                (_outflowFlux[i].x + _outflowFlux[i].y + _outflowFlux[i].z + _outflowFlux[i].w)
            ) / (PIPE_LENGTH * PIPE_LENGTH);

            const float deltaX = ((flowL - _outflowFlux[i].x) + (_outflowFlux[i].y - flowR)) * 0.5f;
            const float deltaY = ((flowB - _outflowFlux[i].w) + (_outflowFlux[i].z - flowT)) * 0.5f;

            const float avgWater = (_waterHeight[i] + newWaterHeight) * 0.5f;

            _velocity[i].x = deltaX / (PIPE_LENGTH * (avgWater + 0.001f));
            _velocity[i].y = deltaY / (PIPE_LENGTH * (avgWater + 0.001f));

            _waterHeight[i] = newWaterHeight;
        }
    }
}

void ErosionGenerator::_computeErosionDeposition(float dt) {
    const u32 width = _terrain->getWidth();
    const u32 depth = _terrain->getDepth();
    const float PIPE_LENGTH = 1.f;

    std::vector<float> heightDelta(_heightmap.size(), 0.0f);
    std::vector<float> sedimentDelta(_heightmap.size(), 0.0f);

    auto H = [&](int j) {
        return _heightmap[j] + _waterHeight[j];
    };

    for (u32 y = 1; y < depth - 1; y++) {
        for (u32 x = 1; x < width - 1; x++) {
            u32 i = y * width + x;

            // calculate local slope (alpha)
            // we use the central difference to find the gradient
            float dhdx = (H(i+1) - H(i-1)) / (2.0f * PIPE_LENGTH);
            float dhdy = (H(i+width) - H(i-width)) / (2.0f * PIPE_LENGTH);

            // sin(alpha) is related to the magnitude of the gradient
            // float sinAlpha = std::min(0.05f, std::sqrt(dhdx*dhdx + dhdy*dhdy));
            float grad = std::sqrt(dhdx*dhdx + dhdy*dhdy);
            float sinAlpha = grad / std::sqrt(1.f + grad * grad);
            if (sinAlpha < 1e-4f) {
                continue; // don't erode on flat terrain
            }

            // calculate transport capacity (C)
            float velocityMag = glm::length(_velocity[i]);
            if (velocityMag < 1e-5f) {
                continue;
            }

            float C = _sedimentCapacity * sinAlpha * velocityMag * _waterHeight[i];

            float capacityDiff = C - _suspendedSedimentAmount[i];
            float water = _waterHeight[i];

            float amount = capacityDiff * water;// * dt;

            if (capacityDiff > 0.0f) {
                // erosion
                amount *= _erosionConstant;
                amount = std::min(amount, _heightmap[i]);
                heightDelta[i] -= amount;
                sedimentDelta[i] += amount;
            }
            else {
                // deposition
                amount *= _depositionConstant;
                amount = std::min(-amount, _suspendedSedimentAmount[i]);
                heightDelta[i] += amount;
                sedimentDelta[i] -= amount;
            }

            // if (C > _suspendedSedimentAmount[i]) {
            //     // erode terrain
            //     float amount = _erosionConstant * (C - _suspendedSedimentAmount[i]);
            //     amount = std::min(amount, _heightmap[i]);
            //     _heightmap[i] = std::max(0.f, _heightmap[i] - amount);
            //     _suspendedSedimentAmount[i] += amount;
            // } else {
            //     // deposit sediment
            //     float amount = _depositionConstant * (_suspendedSedimentAmount[i] - C);
            //     amount = std::min(amount, _suspendedSedimentAmount[i]);
            //     _heightmap[i] += amount;
            //     _suspendedSedimentAmount[i] -= amount;
            // }
        }
    }

    for (size_t i = 0; i < _heightmap.size(); i++) {
        _heightmap[i] = _heightmap[i] + heightDelta[i];
        _suspendedSedimentAmount[i] = _suspendedSedimentAmount[i] + sedimentDelta[i];
    }
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float ErosionGenerator::_sampleSediment(float x, float y) const {
    const u32 width = _terrain->getWidth();
    const u32 depth = _terrain->getDepth();

    x = std::clamp(x, 0.0f, (float)width - 1.001f);
    y = std::clamp(y, 0.0f, (float)depth - 1.001f);

    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float dx = x - x0;
    float dy = y - y0;

    float s00 = _suspendedSedimentAmount[y0 * width + x0];
    float s10 = _suspendedSedimentAmount[y0 * width + x1];
    float s01 = _suspendedSedimentAmount[y1 * width + x0];
    float s11 = _suspendedSedimentAmount[y1 * width + x1];

    // Bilinear interpolation
    float row0 = lerp(s00, s10, dx);
    float row1 = lerp(s01, s11, dx);

    return lerp(row0, row1, dy);
}

void ErosionGenerator::_transportSediment(float dt) {
    const u32 width = _terrain->getWidth();
    const u32 depth = _terrain->getDepth();

    // We need a temporary buffer because advection is a global operation
    std::vector<float> nextSediment(_suspendedSedimentAmount.size());

    for (u32 y = 0; y < depth; y++) {
        for (u32 x = 0; x < width; x++) {
            u32 i = y * width + x;

            // Look back along the velocity vector
            float srcX = (float)x - _velocity[i].x * dt;
            float srcY = (float)y - _velocity[i].y * dt;

            // Sample the sediment amount at the source position
            nextSediment[i] = _sampleSediment(srcX, srcY);
        }
    }

    // Update the sediment buffer
    _suspendedSedimentAmount = std::move(nextSediment);
}

void ErosionGenerator::_applyEvaporation(float dt) {
    const u32 width = _terrain->getWidth();
    const u32 depth = _terrain->getDepth();

    for (u32 i = 0; i < width * depth; i++) {
        // Reduce the water height
        _waterHeight[i] *= (1.f - _evaporationConstant * dt);

        if (_waterHeight[i] < 0.0001f) {
            _waterHeight[i] = 0.f;
            _outflowFlux[i] = glm::vec4(0.f);
            _velocity[i] = glm::vec2(0.f);
        }
    }
}

void ErosionGenerator::generateHeightmap() {
    _isSimulationRunning = true;
    _init();

    _simulationTask = std::async(std::launch::async, [this]() {
        u64 i = 0;
        while (_isSimulationRunning) {
            _applyRainfall(_deltaTime);
            _computeFlow(_deltaTime);
            _computeErosionDeposition(_deltaTime);
            _transportSediment(_deltaTime);
            _applyEvaporation(_deltaTime);

            if (i % 10 == 0) {
                _heightmapB = _heightmap;
                _updateFlag.store(true, std::memory_order_release);
            }

            i++;
        }
    });


}

void ErosionGenerator::update() {
    // periodic updates to the gpu buffers
    if (_updateFlag.exchange(false, std::memory_order_acquire)) {
        _terrain->loadRawFromMemory(_heightmapB, _terrain->getWidth(), _terrain->getDepth());
    }
    // finish simulation
    if (_simulationTask.valid() && _simulationTask.wait_for(0s) == std::future_status::ready) {
        _simulationTask.get();

        _terrain->loadRawFromMemory(_heightmap, _terrain->getWidth(), _terrain->getDepth());
        _isSimulationRunning = false;
    }
}


void ErosionGenerator::_init() {
    if (_terrain) {
        _heightmap = _terrain->getHeights();
        _waterHeight = std::vector<float>(_heightmap.size(), 0.f);
        _suspendedSedimentAmount = std::vector<float>(_heightmap.size(), 0.f);
        _outflowFlux = std::vector<glm::vec4>(_heightmap.size(), glm::vec4(0.f));
        _velocity = std::vector<glm::vec2>(_heightmap.size(), glm::vec2(0.f));
    }
    else {
        _heightmap = std::vector<float>(256*256, 0.f);
        _waterHeight = std::vector<float>(256*256, 0.f);
        _suspendedSedimentAmount = std::vector<float>(256*256, 0.f);
        _outflowFlux = std::vector<glm::vec4>(256*256, glm::vec4(0.f));
        _velocity = std::vector<glm::vec2>(256*256, glm::vec2(0.f));
    }
}



}
