#include "biomes.h"

Biome* getBiomeFromEnum(BiomeEnum val) {
    switch (val) {
        default:
        case Nether:
            return VanillaBiomes::mNether;

        case BasaltDeltas:
            return VanillaBiomes::mBasaltDeltas;

        case CrimsonForest:
            return VanillaBiomes::mCrimsonForest;

        case SoulsandValley:
            return VanillaBiomes::mSoulsandValley;

        case WarpedForest:
            return VanillaBiomes::mWarpedForest;
    }
}

vector<CustomBiome*> getAllCustomBiomes() {
    return vector<CustomBiome *>({
        new CustomBiome(CrimsonForest),
    });
}
