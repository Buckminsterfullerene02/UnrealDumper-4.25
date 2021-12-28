#pragma once
#include "defs.h"
#include <functional>

UE_UStruct StaticAssetData{ nullptr };

struct TArray {
  template<typename Callable>
  uint8* ForEach(Callable callable) {
    StaticAssetData = ObjObjects->FindObjects("ScriptStruct AssetRegistry.AssetData");
    for (size_t i = 0; i < Count; i++)
    {
      uint8 elem = reinterpret_cast<uint8*>(reinterpret_cast<uintptr_t>(Data) + (i + StaticAssetData->GetSize()));
      if (callable(elem)) { break; }
    }
  }
  auto copyFast(const TArray& other) -> void {
    Data = other.Data;
    Count = other.Count;
    Max = other.Max;
  }
  uint8* Data;
  uint32 Count;
  uint32 Max;
};

struct FNameEntryHandle {
  uint32 Block = 0;
  uint32 Offset = 0;
  FNameEntryHandle(uint32 block, uint32 offset) : Block(block), Offset(offset){};
  FNameEntryHandle(uint32 id) : Block(id >> 16), Offset(id & 65535){};
  operator uint32() const { return (Block << 16 | Offset); }
};

struct FNamePool {
  uint8 Lock[8];
  uint32 CurrentBlock;
  uint32 CurrentByteCursor;
  uint8* Blocks[8192];
  uint8* GetEntry(FNameEntryHandle handle) const;
  void DumpBlock(uint32 blockId, uint32 blockSize, std::function<void(std::string_view, uint32)> callback) const;
  void Dump(std::function<void(std::string_view, uint32)> callback) const;
};

struct TUObjectArray {
  uint8** Objects;
  uint8* PreAllocatedObjects;
  uint32 MaxElements;
  uint32 NumElements;
  uint32 MaxChunks;
  uint32 NumChunks;

  uint8* GetObjectPtr(uint32 id) const;
  void Dump(std::function<void(uint8*)> callback) const;
  class UE_UObject FindObject(const std::string &name) const;
  void ForEachObjectOfClass(const class UE_UClass cmp, std::function<bool(uint8*)> callback) const;
  bool IsObject(UE_UObject address) const;
};

using ProcessEventType = UE_UObject * (*)(UE_UObject* p_this, class UFunction*, void* parms);

extern TUObjectArray ObjObjects;
extern FNamePool NamePoolData;
extern ProcessEventType ProcessEvent;
