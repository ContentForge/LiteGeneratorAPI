#pragma once

#include <utility>

#include "pch.h"
#include "generator_tools.h"

#define fs std::filesystem

extern "C" {
    #include "lua/lua.h"
    #include "lua/lauxlib.h"
    #include "lua/lualib.h"
}

#ifdef _WIN32
    #pragma comment(lib, "liblua54.a")
#endif


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

        void generateChunk(const GEN_API::ChunkManager *world, int chunkX, int chunkZ) override;
    };

    int lua_setBlockAt(lua_State* state);
    int lua_getBlockAt(lua_State* state);
    int lua_setBiomeAt(lua_State* state);
    int lua_getBiomeAt(lua_State* state);
    int lua_getMinY(lua_State* state);
    int lua_getMaxY(lua_State* state);
    int lua_getHighestBlockAt(lua_State* state);

}