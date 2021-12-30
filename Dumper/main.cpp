#include <fmt/core.h>
#include "dumper.h"
#include "utils.h"
#include "wrappers.h"

int main(int argc, char* argv[])
{
    uint64 start;
    uint64 end;
    uint64 time;

    auto dumper = Dumper::GetInstance();

    start = GetTime();
    switch (dumper->Init(argc, argv))
    {
    case STATUS::WINDOW_NOT_FOUND: { puts("Can't find UE4 window"); return 1; }
    case STATUS::PROCESS_NOT_FOUND: { puts("Can't find process"); return 1; }
    case STATUS::READER_ERROR: { puts("Can't init reader"); return 1; }
    case STATUS::CANNOT_GET_PROCNAME: { puts("Can't get process name"); return 1; }
    case STATUS::ENGINE_NOT_FOUND: { puts("Can't find offsets for this game"); return 1; }
    case STATUS::ENGINE_FAILED: { puts("Can't init engine for this game"); return 1; }
    case STATUS::MODULE_NOT_FOUND: { puts("Can't enumerate modules (protected process?)"); return 1; }
    case STATUS::CANNOT_READ: { puts("Can't read process memory"); return 1; }
    case STATUS::INVALID_IMAGE: { puts("Can't get executable sections"); return 1; }
    case STATUS::SUCCESS: { break; };
    default: { return 1; }
    }
    end = GetTime();
    time = (end - start) / 10000;
    fmt::print("Init time: {} ms\n", time);

    fmt::print("Loading all assets\n");
    start = GetTime();
    UE_UAssetRegistry* asset_registry = UE_UAssetRegistryHelpers::GetAssetRegistry();
    if (!asset_registry) {
        fmt::print("Was unable to load assets because AssetRegistry wasn't found, defaulting to whatever is already loaded.");
    } else {
        TArray all_assets;
        asset_registry->GetAllAssets(all_assets, false);
        fmt::print("Number of assets to load: {}", all_assets.Count);
        all_assets.ForEach([](void* asset) {
            if (!asset) { return false; }

            UE_FAssetData typed_asset = *static_cast<UE_FAssetData*>(asset);

            // Load asset
            UE_UAssetRegistryHelpers::GetAsset(typed_asset);

            // Returning true will stop the loop, but we want all assets so lets return false.
            return false;
        });
    }
    end = GetTime();
    time = (end - start) / 10000;
    fmt::print("Asset loading time: {} ms\n", time);

    start = GetTime();
    switch (dumper->Dump())
    {
    case STATUS::FILE_NOT_OPEN: { puts("Can't open file"); return 1; }
    case STATUS::ZERO_PACKAGES: { puts("Size of packages is zero"); return 1; }
    case STATUS::SUCCESS: { break; }
    default: { return 1; }
    }
    end = GetTime();
    time = (end - start) / 10000;
    fmt::print("Dump time: {} ms\n", time);

    return 0;
}
