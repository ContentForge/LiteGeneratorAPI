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

    void createTransactionCache(Level* level, ChunkPos const& chunkPos, vector<BlockTransactionElement> elements);

    struct ChunkTransactionLink {
        int x;
        int z;
        vector<BlockTransactionElement> elements;
    };

    class BlockTransaction {
    private:
        vector<ChunkTransactionLink> chunks;
    public:
        BlockTransaction() {
            chunks = vector<ChunkTransactionLink>();
        }

        void addBlock(int x, short y, int z, Block* block) {
            addBlock(x, y, z, block, true);
        }

        void addBlock(int x, short y, int z, Block* block, bool force);

        void addBlock(int x, short y, int z, string blockId) {
            addBlock(x, y, z, blockId, true);
        }

        void addBlock(int x, short y, int z, string blockId, bool force) {
            addBlock(x, y, z, blockId, 0, force);
        }

        void addBlock(int x, short y, int z, string blockId, unsigned short tileData) {
            addBlock(x, y, z, blockId, tileData, true);
        }

        void addBlock(int x, short y, int z, string blockId, unsigned short tileData, bool force) {
            addBlock(x, y, z, Block::create(blockId, tileData), force);
        }

        void apply(LevelChunk* levelChunk, ChunkPos* chunkPos);
    };
}
