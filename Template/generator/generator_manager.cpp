#include "generator_manager.h"

#include <utility>

using namespace luabridge;
using namespace luabridge::detail;


Logger genLogger("GEN_LUA");
lua_State* L;
std::mutex chunkGeneratorMutex;
GEN_API::ChunkManager* currentChunkManager;

class WrapperRandom {
private:
    bool newState;

public:
    GEN_API::Random* random;

    WrapperRandom(int seed) {
        random = new GEN_API::Random(seed);
        newState = true;
    }

    WrapperRandom(GEN_API::Random* random) {
        this->random = random;
        newState = false;
    }

    ~WrapperRandom() {
        if (newState) delete random;
    }

    int nextInt() {
        return random->nextInt();
    }

    int nextIntBound(int bound) {
        return random->nextInt(bound);
    }

    int nextIntRange(int min, int max) {
        return random->nextInt(min, max);
    }

    bool nextBool() {
        return random->nextBool();
    }

    float nextFloat() {
        return random->nextFloat();
    }

    float nextSignedFloat() {
        return random->nextSignedFloat();
    }
};

void initChunkFunctions(lua_State* state) {
    getGlobalNamespace(state).beginNamespace("world")
        .addFunction("setBlockAt", GEN_LUA::lua_setBlockAt)
        .addFunction("getBlockAt", GEN_LUA::lua_getBlockAt)
        .addFunction("setBiomeAt", GEN_LUA::lua_setBiomeAt)
        .addFunction("getBiomeAt", GEN_LUA::lua_getBiomeAt)
        .addFunction("getHighestBlockAt", GEN_LUA::lua_getHighestBlockAt)
        .endNamespace();
}

void initFunctions(lua_State* state) {
    //chunk manager
    getGlobalNamespace(state).beginNamespace("world")
            .addConstant("MIN_Y", WORLD_MIN_Y)
            .addConstant("MAX_Y", WORLD_MAX_Y)
            .endNamespace();

    getGlobalNamespace(L)
        .beginClass<WrapperRandom>("Random")
                .addConstructor<void(*)(int)>()
                .addFunction("nextInt", &WrapperRandom::nextInt)
                .addFunction("nextIntBound", &WrapperRandom::nextIntBound)
                .addFunction("nextIntRange", &WrapperRandom::nextIntRange)
                .addFunction("nextBool", &WrapperRandom::nextBool)
                .addFunction("nextFloat", &WrapperRandom::nextFloat)
                .addFunction("nextSignedFloat", &WrapperRandom::nextSignedFloat)
        .endClass();

    //TODO: simplex

    //TODO: block transaction
}


GEN_API::WorldGenerator* handleScript(int seed) {
    LuaRef onInitFunc = getGlobal(L, "onInit");
    if (!onInitFunc.isFunction()) {
        throw GEN_LUA::AdapterException("Function `onInit(seed)` is not defined");
    }

    LuaRef generateChunkFunc = getGlobal(L, "generateChunk");
    if (!generateChunkFunc.isFunction()) {
        throw GEN_LUA::AdapterException("Function `generateChunk(chunkX, chunkZ, rand)` is not defined");
    }

    GEN_API::WorldGenerator* generator = new GEN_LUA::CustomGenerator(seed);

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
    return generator;
}

void GEN_LUA::onInit(CustomGenerator* generator) {
    initFunctions(L);

    try {
        LuaRef func = getGlobal(L, "onInit");
        func(generator->getSeed());
    } catch (LuaException const& ex) {
        genLogger.error(ex.what());
    }
}

void GEN_LUA::CustomGenerator::generateChunk(GEN_API::ChunkManager *world, int chunkX, int chunkZ) {
    chunkGeneratorMutex.lock();
    currentChunkManager = world;
    clock_t time = clock();

    initChunkFunctions(L);

    try {
        LuaRef func = getGlobal(L, "generateChunk");
        func(chunkX, chunkZ);
    } catch (LuaException const& ex) {
        genLogger.error(ex.what());
    }

    std::cout << clock() - time << std::endl;

    currentChunkManager = nullptr;
    chunkGeneratorMutex.unlock();
}

void GEN_LUA::lua_setBlockAt(int x, int y, int z, std::string const& blockId) {
    currentChunkManager->setBlockAt(x, y, z, blockId);
}

std::string GEN_LUA::lua_getBlockAt(int x, int y, int z) {
    return currentChunkManager->getBlockAt(x, y, z).getTypeName();
}

void GEN_LUA::lua_setBiomeAt(int x, int z, int biomeId) {
    //TODO
}

int GEN_LUA::lua_getBiomeAt(int x, int z) {
    return 0; //TODO
}

int GEN_LUA::lua_getHighestBlockAt(int x, int z) {
    return currentChunkManager->getHighestBlockAt(x, z);
}