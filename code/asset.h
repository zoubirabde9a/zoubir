#if !defined(ASSETS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define ASSETS_H
#include "asset_type_id.h"
#include "file_formats.h"

struct loaded_audio
{
    i16 *Samples[2];
    u32 SampleCount;
    u32 Channels;    
};

#define TEXTURE_NO_FILTER 0
#define TEXTURE_SOFT_FILTER 1

struct loaded_texture
{
    u32 ID;
    u32 Width;
    u32 Height;
};

struct asset_id
{
    asset_type_id Type;
    u32 Index;
};

enum asset_state
{
    AssetState_Unloaded,
    AssetState_Queued,
    AssetState_Loaded,
    AssetState_Locked 
};

struct asset
{
    u32 volatile State;
    union
    {
        loaded_texture Texture;
        loaded_audio Audio;            
    };
};
struct assets
{
    memory_arena Arena;
    
    zas_asset_type_slot *AssetTypes;//[AssetType_Count];

    u32 AssetCount;
    
    zas_asset_info *Infos;
    asset *Assets;
    
    platform_file_handle *FileHandle;
};

struct opengl_texture_queue_entry
{
    asset_id ID;
    assets *Assets;
    void *TextureMemory;
    u32 Width;
    u32 Height;
    i32 ImageFormat;

    u32 Flags;
};

struct opengl_texture_queue
{
    u32 volatile AThreadIsAlreadyWritingAnEntry;
    opengl_texture_queue_entry Entries[256];
    u32 EntryCount;

    u32 volatile CurrentEntryToRead;
    u32 volatile CurrentEntryToWrite;
};

#endif
