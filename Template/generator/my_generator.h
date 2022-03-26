#pragma once
#include "biomes.h"
#include "block_transaction.h"


struct BiomeMapNode {
    int x;
    int z;
    CustomBiome* biome;
};

class CursedGenerator: public GEN_API::WorldGenerator {
private:
    GEN_API::Simplex* simplex;
    GEN_API::Simplex* biomeNoise;
    GEN_API::Random* biomeRand;
    vector<CustomBiome*> biomes;

public:
    CursedGenerator(int seed) : WorldGenerator(seed) {
        simplex = new GEN_API::Simplex(random, 1, 1/4.0f, 1/32.0f);
        biomeNoise = new GEN_API::Simplex(random, 1, 1/8.0f, 1/64.0f);
        biomeRand = new GEN_API::Random(0);
        biomes = getAllCustomBiomes();
    }

    ~CursedGenerator() {
        delete simplex;
        delete biomeNoise;
        delete biomeRand;
    }

    void generateChunk(GEN_API::ChunkManager const* world, int chunkX, int chunkZ) override;

    void populateChunk(GEN_API::ChunkManager const* world, int chunkX, int chunkZ) override;

    vector<BiomeMapNode> generateBiomeMap(int chunkX, int chunkZ);

    CustomBiome* pickBiomeAt(int x, int z, vector<BiomeMapNode> biomeMap);
};
