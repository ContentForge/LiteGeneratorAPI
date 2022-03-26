#include "my_generator.h"
#include <cmath>
#include <LoggerAPI.h>

#define SCALE 500.0f
#define WORLD_HIGH 60


Logger dLogger("DEBUG");

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

    for (char xc = 0; xc < 16; xc++) {
        for (char zc = 0; zc < 16; zc++) {
            int yMax = high[xc][zc];
            if (yMax < WORLD_HIGH - 6) yMax = WORLD_HIGH - 6;

            for (short y = world->getMinY(); y < yMax; y++) {
                world->setBlockAt(xc, y, zc, high[xc][zc] < y? VanillaBlocks::mStillLava : VanillaBlocks::mNetherrack);
            }
            //world->setBlockAt(xc, yMax, zc, "shizotoaster:sacrifice_table");
        }
    }
}

void CursedGenerator::populateChunk(GEN_API::ChunkManager const* world, int chunkX, int chunkZ) {
    GEN_API::BlockTransaction transaction;

    transaction.addBlock(L2G_COORD(chunkX, -1),
                         70,
                         L2G_COORD(chunkZ, -1),
                         VanillaBlocks::mGlowStone);

    transaction.addBlock(L2G_COORD(chunkX, 16),
                         71,
                         L2G_COORD(chunkZ, 16),
                         VanillaBlocks::mRedstoneBlock);

    transaction.apply(world);
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
