#pragma once
#include "generator_tools.h"

class CursedGenerator: public GEN_API::WorldGenerator {
private:
    GEN_API::Simplex* simplex;

public:
    CursedGenerator(int seed) : WorldGenerator(seed) {
        simplex = new GEN_API::Simplex(random, 1, 1/4.0f, 1/32.0f);
    }

    ~CursedGenerator() {
        delete simplex;
    }

    void generateChunk(GEN_API::ChunkManager const& world, int chunkX, int chunkZ) override;

    void populateChunk(GEN_API::ChunkManager const& world, int chunkX, int chunkZ) override;
};
