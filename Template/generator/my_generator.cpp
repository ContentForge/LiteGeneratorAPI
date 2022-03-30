#include "my_generator.h"

#define SCALE 500.0f
#define WORLD_HIGH 60


Logger dLogger("DEBUG");

enum BlockIterType {
    Nothing,
    Air,
    Top,
    Land,
    TopAndLand,
    Lava,
    Block,
};

void CursedGenerator::generateChunk(GEN_API::ChunkManager const* world, int chunkX, int chunkZ) {
    vector<BiomeMapNode> biomeMap = generateBiomeMap(chunkX, chunkZ);
    short high[16][16];
    for (char xc = 0; xc < 16; xc++) {
        int x = L2G_COORD(chunkX, xc);
        for (char zc = 0; zc < 16; zc++) {
            int z = L2G_COORD(chunkZ, zc);

            world->setBiomeAt(xc, zc, getBiomeFromEnum(pickBiomeAt(x, z, biomeMap)->getBiome()));

            float fy = simplex->getNoise2D(x / SCALE, z / SCALE) -
                    powf(abs(simplex->getNoise2D(x / SCALE * 4, z / SCALE * 4)), 2) +
                    powf((simplex->getNoise2D(x / SCALE * 2, z / SCALE * 2) + 1), 4);
            fy = fy * 5 + WORLD_HIGH;

            float erosion = 1 - abs(simplex->getNoise2D(z / SCALE * 10, x / SCALE * 10));
            erosion *= 5 + (0.01 * (fy - 55));
            fy -= erosion;

            if (fy < WORLD_HIGH - 5) {
                float t = (fy - WORLD_HIGH - 8) / (fy - WORLD_HIGH - 5);
                t *= fy;
                fy = WORLD_HIGH - 22 - (WORLD_HIGH - 5 - t);
            }

            high[xc][zc] = (int) fy;
        }
    }

    bool land[16][16];
    for (char xc = 0; xc < 16; xc++) {
        int x = L2G_COORD(chunkX, xc);
        for (char zc = 0; zc < 16; zc++) {
            int z = L2G_COORD(chunkZ, zc);
            int yMax = high[xc][zc];
            if (yMax < WORLD_HIGH - 6) yMax = WORLD_HIGH - 6;

            BlockIterType iter = Nothing;
            for (short y = world->getMinY(); y <= yMax; y++) {
                //Фильтры
                int preY = y - 1;
                //TODO: Применение фильтров. Пример ниже для наглядности работы
                switch (iter) {
                    case Land:
                        world->setBlockAt(x, preY, z, VanillaBlocks::mObsidian);
                        break;

                    case Top:
                        world->setBlockAt(x, preY, z, VanillaBlocks::mRedstoneBlock);
                        break;

                    case TopAndLand:
                        world->setBlockAt(x, preY, z, VanillaBlocks::mGoldBlock);
                        break;
                }

                //Ландшафт
                if (y == yMax) continue;

                float caveNoise = simplex->getNoise2D(-x / 25.0f, -z / 25.0f) * 3;
                if (y + caveNoise< WORLD_HIGH - 2) caveNoise = 1.0f;
                else caveNoise = abs(simplex->getNoise3D(x / 35.0f, y / 25.0f, z / 35.0f));
                if (caveNoise < 0.30f * (simplex->getNoise2D(z / 100.0f, x / 100.f) + 1) / 2.0f) {
                    if (iter == Block || iter == Top) iter = Land;
                    else iter = Air;
                    continue;
                }

                if (high[xc][zc] < y) {
                    world->setBlockAt(x, y, z, VanillaBlocks::mStillLava);
                    iter = Lava;
                } else {
                    world->setBlockAt(x, y, z, VanillaBlocks::mNetherrack);
                    if (iter == Air) iter = Top;
                    else iter = Block;
                    high[xc][zc] = y;
                }

                if (y == yMax - 1){
                    if (iter == Lava) {
                        land[xc][zc] = false;

                    } else {
                        land[xc][zc] = true;
                        if (iter == Block) iter = Land;
                        else if (iter == Top) iter = TopAndLand;
                    }
                }
            }
        }
    }

    //TODO: Действия с поверхностью
}

void CursedGenerator::populateChunk(GEN_API::ChunkManager const* world, int chunkX, int chunkZ) {

}

vector<BiomeMapNode> CursedGenerator::generateBiomeMap(int chunkX, int chunkZ) {
    int cx = chunkX >> 4;
    int cz = chunkZ >> 4;

    vector<BiomeMapNode> map;
    for (char i = -1; i < 2; i++) {
        int tcx = cx + i;
        for (char j = -1; j < 2; j++) {
            int tcz = cz + j;
            biomeRand->setSeed(tcx * 9901 + tcz);

            map.push_back({
                (tcx << 8) + biomeRand->nextInt(0,256),
                (tcz << 8) + biomeRand->nextInt(0,256),
                biomes.at(biomeRand->nextInt(biomes.size()))
            });
        }
    }
    return map;
}

CustomBiome* CursedGenerator::pickBiomeAt(int x, int z, vector<BiomeMapNode> biomeMap) {
    long long hash = x * 2345803 ^ z * 9236449 ^ seed;
    hash *= hash + 223;
    int xNoise = hash >> 20 & 3;
    int zNoise = hash >> 22 & 3;
    if(xNoise == 3){
        xNoise = 1;
    }
    if(zNoise == 3){
        zNoise = 1;
    }
    float nx = x + xNoise - 1;
    float nz = z + zNoise - 1;

    nx = nx + nx * biomeNoise->getNoise2D(x / 256, z / 256) * 0.05f;
    nz = nz + nz * biomeNoise->getNoise2D(z / -256, x / -256) * 0.05f;

    BiomeMapNode nearestBiome;
    float d = -1;
    for (BiomeMapNode biome: biomeMap) {
        float td = powf(nx - biome.x, 2) + powf(nz - biome.z, 2);
        if (d == -1 || td < d) {
            d = td;
            nearestBiome = biome;
        }
    }

    return nearestBiome.biome;
}
