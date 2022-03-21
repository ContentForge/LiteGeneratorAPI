#include "pch.h"
#include <LLAPI.h>
#pragma comment(lib, "../SDK/Lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/Lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/Lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/Lib/LiteLoader.lib")


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason){
        case DLL_PROCESS_ATTACH:
            LL::registerPlugin("BetterNetherGeneration",
                               "Plugin for generation nether world",
                               LL::Version(1, 0, 0));
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

void PluginInit();

extern "C" {
    // Do something after all the plugins loaded
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
        PluginInit();
    }
}
