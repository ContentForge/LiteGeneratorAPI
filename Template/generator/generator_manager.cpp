#include "generator_manager.h"


Logger genLogger("GEN_LUA");
lua_State* L;


void initFunctions() {
    //chunk manager
    lua_register(L, "setBlockAt", GEN_LUA::lua_setBlockAt);
    lua_register(L, "getBlockAt", GEN_LUA::lua_getBlockAt);
    lua_register(L, "setBiomeAt", GEN_LUA::lua_setBiomeAt);
    lua_register(L, "getBiomeAt", GEN_LUA::lua_getBiomeAt);
    lua_register(L, "getMinY", GEN_LUA::lua_getMinY);
    lua_register(L, "getMaxY", GEN_LUA::lua_getMaxY);
    lua_register(L, "getHighestBlockAt", GEN_LUA::lua_getHighestBlockAt);

    //TODO: random

    //TODO: simplex

    //TODO: block transaction
}


GEN_API::WorldGenerator* handleScript(int seed) {
    lua_getglobal(L, "onInit");
    if (!lua_isfunction(L, -1)) {
        throw GEN_LUA::AdapterException("Function `onInit(seed)` is not defined");
    }

    lua_getglobal(L, "generateChunk");
    if (!lua_isfunction(L, -1)) {
        throw GEN_LUA::AdapterException("Function `generateChunk(world, chunkX, chunkZ)` is not defined");
    }

    GEN_API::WorldGenerator* generator = new GEN_LUA::CustomGenerator(seed);
    initFunctions();
    //TODO: Генерация класса

    return generator;
}

GEN_API::WorldGenerator* GEN_LUA::init(int seed) {
    GEN_API::WorldGenerator* generator;
    try {
        int r = fs::create_directory("generator");
        if (!fs::exists("generator/main.lua")) {
            throw GEN_LUA::AdapterException("File generator/main.lua does not exists!");
        }

        L = luaL_newstate();
        luaL_openlibs(L);

        luaL_dofile(L, "generator/main.lua");
        if (r == LUA_OK) {
            generator = handleScript(seed);
        } else {
            throw GEN_LUA::AdapterException(lua_tostring(L, -1));
        }
    } catch (GEN_LUA::AdapterException& ex) {
        genLogger.fatal(ex.getMessage());
        genLogger.warn("Due to an error, a void generator was chosen.");

        generator = new GEN_API::WorldGenerator(seed);
    }
}

void GEN_LUA::onInit(CustomGenerator* generator) {

}

void GEN_LUA::CustomGenerator::generateChunk(const GEN_API::ChunkManager *world, int chunkX, int chunkZ) {

}

int GEN_LUA::lua_setBlockAt(lua_State* state) {
    // function setBlockAt(x, y, z, blockId)  end

    return 0;
}

int GEN_LUA::lua_getBlockAt(lua_State* state) {
    // function getBlockAt(x, y, z)  end

    return 1;
}

int GEN_LUA::lua_setBiomeAt(lua_State* state) {
    // function setBiomeAt(x, z, biomeId)  end

    return 0;
}

int GEN_LUA::lua_getBiomeAt(lua_State* state) {
    // function getBiomeAt(x, z)  end

    return 1;
}

int GEN_LUA::lua_getMinY(lua_State* state) {
    // function getMinY()  end

    return 1;
}

int GEN_LUA::lua_getMaxY(lua_State* state) {
    // function getMaxY()  end

    return 1;
}

int GEN_LUA::lua_getHighestBlockAt(lua_State* state) {
    // function getHighestBlockAt(x, z)  end

    return 1;
}
