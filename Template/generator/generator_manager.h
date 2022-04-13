#pragma once

#include "pch.h"
#include "generator_tools.h"

#define fs std::filesystem


namespace GEN_LUA {
    class AdapterException: public std::exception {
    private:
        string message;

    public:
        AdapterException(string message) {
            this->message = std::move(message);
        }

        string getMessage() {
            return message;
        }
    };

    GEN_API::WorldGenerator* init(int seed);

    class CustomGenerator;
    void onInit(CustomGenerator* generator);

    class CustomGenerator: public GEN_API::WorldGenerator {
    public:
        CustomGenerator(int seed) : GEN_API::WorldGenerator(seed) {
            onInit(this);
        }

        void generateChunk(GEN_API::ChunkManager* world, int chunkX, int chunkZ) override;
    };

    void lua_setBlockAt(int x, int y, int z, std::string const& blockId);

    std::string lua_getBlockAt(int x, int y, int z);

    void lua_setBiomeAt(int x, int z, int biomeId);

    int lua_getBiomeAt(int x, int z);

    int lua_getHighestBlockAt(int x, int z);
}