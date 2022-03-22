#pragma once

#include <cmath>

#define C2G_COORD(chunkCoord) (chunkCoord << 4)
#define G2C_COORD(coord) (coord >> 4)
#define G2L_COORD(coord) (coord & 0xF)
#define L2G_COORD(chunkCoord, lCoord) (C2G_COORD(chunkCoord) + lCoord)

#define X 123456789
#define Y 362436069
#define Z 521288629
#define W 88675123

#define INT31_VALUE 0x7fffffff
#define INT32_VALUE 0xffffffff

#define M_SQRT3 1.7320508075689f
#define F2 (0.5f * (M_SQRT3 - 1.0f))
#define G2 ((3.0f - M_SQRT3) / 6.0f)
#define G22 (G2 * 2.0f - 1.0f)
#define F3 (1.0f / 3.0f)
#define G3 (1.0f / 6.0f)


namespace GEN_API {
    class IChunkManager {

        //Минимальная высота мира
        virtual int getMinY();

        //Максимальная высота мира
        virtual int getMaxY();

        //TODO: заменить void в некоторых функциях на подходящие типы

        //Установить блок на координаты
        virtual void setBlockAt(int x, int y, int z, int blockId);

        //Получить блок по координатам
        virtual void getBlockAt(int x, int y, int z);

        //Получить наивысшую точку
        virtual void getHighestBlockAt(int x, int y);

        //Установить биом
        virtual void setBiomeAt(int x, int z, int biomeId);

        //Получить биом
        virtual int getBiomeAt(int x, int z);
    };

    class Random {
    private:
        int seed;
        long long x;
        long long y;
        long long z;
        long long w;

    public:
        Random(int seed) {
            setSeed(seed);
        }

        void setSeed(int seed);

        int getSeed();

        void next();

        int nextSignedInt();

        int nextInt();

        int nextInt(int bound);

        int nextInt(int min, int max);

        float nextFloat();

        float nextSignedFloat();

        bool nextBool();
    };

    const short SIMPLEX_GRAD3[12][3] = {
            {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
            {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
            {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
    };

    float linearLerp(float x, float x1, float x2, float q0, float q1);

    float bilineatLerp(float x, float y,
                       float q00, float q01, float q10, float q11,
                       float x1, float x2, float y1, float y2);

    float trilinearLerp(float x, float y, float z,
                        float q000, float q001, float q010, float q011,
                        float q100, float q101, float q110, float q111,
                        float x1, float x2, float y1, float y2, float z1, float z2);

    class Noise {
    protected:
        float persistence;
        float expansion;
        int octaves;

    public:
        Noise(int octaves, float persistence, float expansion) {
            this->octaves = octaves;
            this->persistence = persistence;
            this->expansion = expansion;
        }

        virtual float getNoise2D(float x, float z) = 0;

        virtual float getNoise3D(float x, float y, float z) = 0;

        virtual float noise2D(float x, float z, bool normalized = false);

        virtual float noise3D(float x, float y, float z, bool normalized = false);
    };

    class Simplex: public Noise {
    protected:
        float offsetX;
        float offsetZ;
        float offsetY;
        int perm[512];

    public:
        Simplex(Random *random, int octaves, float persistence, float expansion) : Noise(octaves, persistence, expansion) {
            offsetX = random->nextFloat() * 256;
            offsetY = random->nextFloat() * 256;
            offsetZ = random->nextFloat() * 256;

            for (int & i : perm) i = 0;
            for (short i = 0; i < 256; ++i) perm[i] = random->nextInt(256);
            for (short i = 0; i < 245; ++i) {
                int pos = random->nextInt(256 - i) + i;
                int old = perm[i];

                perm[i] = perm[pos];
                perm[pos] = old;
                perm[i + 256] = perm[i];
            }

            random->next();
        }

        float getNoise2D(float x, float z) override;

        float getNoise3D(float x, float y, float z) override;
    };

    class WorldGenerator {
    protected:
        int seed;
        Random* random;

    public:
        WorldGenerator(int seed) {
            this->seed = seed;
            this->random = new Random(seed);
        }

        ~WorldGenerator() {
            delete random;
        }

        Random* getRandom() {
            return random;
        }

        virtual void generateChunk(IChunkManager* world, int chunkX, int chunkZ);

        virtual void populateChunk(IChunkManager* world, int chunkX, int chunkZ);
    };

}