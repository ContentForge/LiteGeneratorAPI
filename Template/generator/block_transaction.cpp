#include "block_transaction.h"

#define STR_TO_INT(val) (std::atoi(val.c_str()))

#define DELIMITER "|"

using std::to_string;

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    std::stringstream ss;
    ss.str(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

GEN_API::BlockTransactionElement::BlockTransactionElement(string encoded) {
    vector<string> tokens = split(encoded, '|');
    blockId = "" + tokens.at(0);
    localX = (char) STR_TO_INT(tokens.at(1));
    localY = (short) STR_TO_INT(tokens.at(2));
    localZ = (char) STR_TO_INT(tokens.at(3));
    tileData = (unsigned short) STR_TO_INT(tokens.at(4));
    placeIsNotFree = (bool) STR_TO_INT(tokens.at(5));
}

void GEN_API::BlockTransactionElement::tryPlace(LevelChunk* levelChunk) {
    ChunkBlockPos pos(localX, localY, localZ);
    if (!placeIsNotFree) {
        if (levelChunk->getBlock(pos).getId() != 0) return;
    }
    levelChunk->setBlockSimple(pos, *Block::create(blockId, tileData));
}

string GEN_API::BlockTransactionElement::encode() {
    return blockId + "|" +
            to_string((short) localX) + "|" +
            to_string(localY) + "|" +
            to_string((short) localZ) + "|" +
            to_string(tileData) + "|" +
            to_string((short) placeIsNotFree);
}

void GEN_API::transactionPostProcessingGeneration(LevelChunk* levelChunk, ChunkPos const& chunkPos) {
    vector <GEN_API::BlockTransactionElement> elements;
    std::string path = Level::getCurrentLevelPath() +
            "/transactions/" + to_string(chunkPos.x) +
            "." + to_string(chunkPos.z);

    for (const auto &entry: std::filesystem::directory_iterator(path)) { //FIXIT
        std::ifstream transaction(entry.path());

        if (transaction.is_open()) {
            std::string line;
            while (std::getline(transaction, line))
                elements.push_back(BlockTransactionElement(line));
        }

        transaction.close();
        std::remove(entry.path().string().c_str());
    }

    for (GEN_API::BlockTransactionElement element: elements){
        element.tryPlace(levelChunk);
    }
}

void GEN_API::createTransactionCache(ChunkPos const& chunkPos, vector<GEN_API::BlockTransactionElement> elements) {
    for (GEN_API::BlockTransactionElement element: elements) {
        std::string path = Level::getCurrentLevelPath() +
                "/transactions/" + to_string(chunkPos.x) +
                "." + to_string(chunkPos.z);
        std::ifstream itransaction(path);

        if (itransaction.is_open()) {
            std::string line;
            std::string content;

            while (std::getline(itransaction, line))
                content.insert(content.length(), line + "\n");

            content.insert(content.length(), element.encode());
            std::ofstream otransaction(path);
            otransaction << content;
        }
        else {
            std::ofstream otransaction(path);
            otransaction << element.encode();
        }
    }
}

void GEN_API::BlockTransaction::addBlock(int x, short y, int z, string blockId, unsigned short tileData, bool force) {
    GEN_API::BlockTransactionElement element(x, y, z, blockId, tileData,force);

    for (GEN_API::ChunkTransactionLink chunk: chunks) {
        if (chunk.x != G2C_COORD(x) || chunk.z != G2C_COORD(z)) continue;

        chunk.elements.push_back(element);
        return;
    }

    chunks.push_back({
        G2C_COORD(x),
        G2C_COORD(z),
        {
            element,
        }
    });
}

void GEN_API::BlockTransaction::apply(LevelChunk* levelChunk, ChunkPos* chunkPos) {
    for (auto chunk: chunks) {
        if (chunk.x != chunkPos->x || chunk.z != chunkPos->z) {
            GEN_API::createTransactionCache(*chunkPos, chunk.elements);
            continue;
        }

        for (auto element: chunk.elements) {
            element.tryPlace(levelChunk);
        }
    }
}
