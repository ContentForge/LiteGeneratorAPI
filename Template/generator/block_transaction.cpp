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
    vector<GEN_API::BlockTransactionElement> elements;
    /*
     * TODO: Получение всех элементов и удаление файла с транзакцией
     * Если файл не найден или дан пустой файл, то ничего не делать.
     *
     * Файл жолжен быть по пути: ПАПКА_СЕРВЕРА/worlds/{levelChunk->getLevel().getCurrentLevelName()}/transactions/{chunkPos.x}:{chunkPos.z}
     */

    for (GEN_API::BlockTransactionElement element: elements) {
        element.tryPlace(levelChunk);
    }
}

void GEN_API::createTransactionCache(Level* level, ChunkPos const& chunkPos, vector<GEN_API::BlockTransactionElement> elements) {
    for (GEN_API::BlockTransactionElement element: elements) {
        /*
         * TODO: Запись элементов транзакции в файл
         * Запись должна быть дополняющая чтобы предыдущие эелементы не удалялись.
         * Каждый элемент должен быть записан с новой строки
         *
         * Функция для сериализации элемента: element.encode()
         *
         * Файл жолжен быть по пути: ПАПКА_СЕРВЕРА/worlds/{level->getCurrentLevelName()}/transactions/{chunkPos.x}:{chunkPos.z}
         */
    }
}

void GEN_API::BlockTransaction::addBlock(int x, short y, int z, Block* block, bool force) {
    BlockTransactionElement element(x, y, z, block ,force);

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
            GEN_API::createTransactionCache(&(levelChunk->getLevel()), *chunkPos, chunk.elements);
            continue;
        }

        for (auto element: chunk.elements) {
            element.tryPlace(levelChunk);
        }
    }
}
