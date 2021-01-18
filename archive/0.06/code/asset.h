#if !defined(ASSETS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define ASSETS_H


enum asset_type
{
    //NOTE(zoubir): Textures
    AssetType_Invalid,
    AssetType_Dragon,
    AssetType_Zoubir,
    AssetType_TileMap,
    AssetType_Tree,
    AssetType_Familiar,
    AssetType_Shadow,
    AssetType_FireBall,
    AssetType_Sword,
    //NOTE(zoubir): Audio
    AssetType_ZoubirAudio,
    AssetType_Dash,
    AssetType_FireCast,
    AssetType_BattleTheme,
    AssetType_Count
};

#pragma pack(push, 1)
struct wav_header {
    union
    {
        u32 FileTypeBlockID32;
        char FileTypeBlockID[4];
    };
    u32 Size;
    union
    {
        u32 FileFormatID32;
        char FileFormatID[4];
    };
    union
    {
        u32 FormatBlockID32;
        char FormatBlockID[4];
    };    
    u32 ChunkSize;
    i16 FormatType;
    i16 Channels;
    u32 SampleRate;
    u32 AvgBytesPerSec;
    i16 BytesPerSample;
    i16 BitsPerSample;
    union
    {
        u32 DataBlockID32;
        char DataBlockID[4];     
    };
    u32 DataSize;
};
#pragma pack(pop)

struct wav_file
{
    void *FileContents;
    u32 FileSize;
    wav_header *Header;
    void *Data;
    bool32 Corrupt;
};

struct loaded_audio
{
    i16 *Samples[2];
    u32 SampleCount;
    u32 Channels;    
};
struct asset_id
{
    asset_type Type;
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
    asset_state State;
    union
    {
        struct
        {
            gl_texture Texture;
        };
        struct
        {
            loaded_audio Audio;            
        };
    };
};

struct asset_slot
{
    asset *Array;
    u32 AllocatedCount;
    u32 ArrayCount;
};

enum asset_family
{
    AssetFamily_Texture,
    AssetFamily_Audio,
    AssetFamily_Count
};

struct asset_info
{
    asset_family Family;
    
    char *FilePath;
    u32 Tags;
    v2 Origin;
};

struct asset_info_array
{
    asset_info *Infos;
    u32 AllocatedCount;
    u32 InfoCount;
};

struct assets
{
    memory_arena Arena;
    
    asset_info_array TexturesInfo[AssetType_Count];
    asset_slot Assets[AssetType_Count];
    
    asset_type DEBUGCurrentAssetType;    
};

struct opengl_texture_queue_entry
{
    asset_id ID;
    assets *Assets;
    void *MemoryToFree;
    void *TextureMemory;
    u32 Width;
    u32 Height;
    i32 ImageFormat;

    u32 Flags;
};

struct opengl_texture_queue
{
    opengl_texture_queue_entry Entries[256];
    u32 EntryCount;

    u32 volatile CurrentEntryToRead;
    u32 volatile CurrentEntryToWrite;
};

struct load_asset_work
{
    opengl_texture_queue *Queue;
    app_memory *Memory;
    assets *Assets;
    asset_id ID;
    asset_family Family;
    char *FilePath;
    u32 Tags;
};

#endif
