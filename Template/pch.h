﻿#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <cmath>
#include <fstream>
#include <utility>
#include <filesystem>
#include <Global.h>
#include <EventAPI.h>
#include <LoggerAPI.h>
#include <MC/Level.hpp>
#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Player.hpp>
#include <MC/ItemStack.hpp>
#include <LLAPI.h>
#include <Utils/FileHelper.h>
#include <MC/Block.hpp>
#include <MC/VanillaBlocks.hpp>
#include <MC/VanillaDimensions.hpp>
#include <MC/VanillaBiomes.hpp>
#include <MC/BlockVolume.hpp>
#include <MC/Level.hpp>
#include <MC/LevelChunk.hpp>
#include <MC/Biome.hpp>
#include <MC/ChunkBlockPos.hpp>
#include <MC/OverworldGenerator.hpp>

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "liblua54.a")
#endif

#endif //PCH_H