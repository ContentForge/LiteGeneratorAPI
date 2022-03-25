#pragma once
#include "generator_tools.h"

enum BiomeEnum {
    BasaltDeltas,
    Nether,
    CrimsonForest,
    WarpedForest,
    SoulsandValley,

    ENUM_END
};

class CustomBiome {
private:
    BiomeEnum biome;

public:
    CustomBiome(BiomeEnum biome) {
        this->biome = biome;
    }

    BiomeEnum getBiome() {
        return biome;
    }
};


Biome* getBiomeFromEnum(BiomeEnum val);

vector<CustomBiome*> getAllCustomBiomes();