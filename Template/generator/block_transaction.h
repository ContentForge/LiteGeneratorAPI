#pragma once
#include "generator_tools.h"

namespace GEN_API {
    class BlockTransactionElement {
    private:
        string blockId;
        char localX;
        short localY;
        char localZ;
        unsigned short tileData;
        bool placeIsNotFree;

    public:
        BlockTransactionElement(string encoded);

        BlockTransactionElement(int x, short y, int z, Block* block, bool forcePlace) {
            localX = G2L_COORD(x);
            localY = y;
            localZ = G2L_COORD(z);
            blockId = block->getTypeName();
            tileData = block->getTileData();
            placeIsNotFree = forcePlace;
        }

        void tryPlace(LevelChunk* levelChunk);

        string encode();
    };

    void transactionPostProcessingGeneration(LevelChunk* levelChunk, ChunkPos const& chunkPos);

    void createTransactionCache(Level* level, ChunkPos const& chunkPos, vector<GEN_API::BlockTransactionElement> elements);
}
