#include "pch.h"
#include "generator/my_generator.h"

Logger logger("BetterNetherGeneration");
GEN_API::WorldGenerator* worldGenerator = new CursedGenerator(12345);

void PluginInit() {
    std::filesystem::create_directories(Level::getCurrentLevelPath() + "/transactions");
}

//Отмена стандартной генерации поверхностей
TClasslessInstanceHook(void, "?_prepareHeights@OverworldGeneratorMultinoise@@EEAAXAEAVBlockVolume@@AEBVChunkPos@@AEBVWorldGenCache@@PEAVAquifer@@$$QEAV?$function@$$A6AXAEBVBlockPos@@AEBVBlock@@H@Z@std@@_NPEAV?$vector@FV?$allocator@F@std@@@7@H@Z",
        BlockVolume& blockVolume,
        ChunkPos const& chunkPos,
        class WorldGenCache const& worldGenCache,
        class Aquifer* aquifier,
        std::function<void (BlockPos const&,Block const&, int)>&& wtfFunc,
        bool a5,
        std::vector<short>* a6,
        int a7) {

}

//Отключение лишнего постпроцессинга
TClasslessInstanceHook(bool, "?postProcess@OverworldGenerator@@UEAA_NAEAVChunkViewSource@@@Z",
                       class ChunkViewSource& a0) {

    return true;
}

//Отключение постпроцессинга стандартного генератора
TClasslessInstanceHook(void, "?_prepareStructureBlueprints@OverworldGenerator@@AEAAXAEBVChunkPos@@AEBVBiomeSource@@AEBVIPreliminarySurfaceProvider@@@Z",
                       ChunkPos const& chunkPos,
                       class BiomeSource const& biomeSource,
                       class IPreliminarySurfaceProvider const& wtfProvider) {
}

//Генератор ландшафта и популятор
TClasslessInstanceHook(void, "?buildSurfaces@OverworldGenerator@@QEAAXAEAUThreadData@1@AEAVBlockVolume@@AEAVLevelChunk@@AEBVChunkPos@@AEBVSurfaceLevelCache@@@Z",
                       struct OverworldGenerator::ThreadData& threadData,
                       BlockVolume& blockVolume,
                       LevelChunk& levelChunk,
                       ChunkPos const& chunkPos,
                       SurfaceLevelCache const& surfaceLevelCache) {

    GEN_API::ChunkManager chunkManager(levelChunk, chunkPos);
    int chunkX = chunkPos.x;
    int chunkZ = chunkPos.z;

    worldGenerator->getRandom()->setSeed((int)(0xdeadbeef ^ (chunkX << 8) ^ chunkZ ^ worldGenerator->getSeed()));
    worldGenerator->generateChunk(&chunkManager, chunkX, chunkZ);

    worldGenerator->getRandom()->setSeed((int)(0xdeadbeef ^ (chunkX << 8) ^ chunkZ ^ worldGenerator->getSeed()));
    worldGenerator->populateChunk(&chunkManager, chunkX, chunkZ);

    levelChunk.markSaveIfNeverSaved();
}

TClasslessInstanceHook(void, "?decorateWorldGenLoadChunk@OverworldGenerator@@MEBAXAEAVBiome@@AEAVLevelChunk@@AEAVBlockVolumeTarget@@AEAVRandom@@AEBVChunkPos@@@Z",
                       Biome& biome,
                       LevelChunk& levelChunk,
                       class BlockVolumeTarget& a2,
                       class Random& a3,
                       ChunkPos const& chunkPos) {

    GEN_API::transactionPostProcessingGeneration(&levelChunk, chunkPos);
}
