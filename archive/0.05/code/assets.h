#if !defined(ASSETS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define ASSETS_H

enum asset_id
{
    AAI_Invalid,
    AAI_Dragon,
    AAI_Zoubir,
    AAI_TileMap,
    AAI_Tree,
    AAI_Familiar,
    AAI_Shadow,
    AAI_FireBall,
    AAI_Count
};

struct app_assets
{
    gl_texture Textures[AAI_Count];
    
    loaded_audio ZoubirAudio;
    loaded_audio Dash;
    loaded_audio FireCast;
};

struct opengl_texture_queue_entry
{
    asset_id ID;
    app_assets *Assets;
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

struct load_texture_work
{
    opengl_texture_queue *Queue;
    app_memory *Memory;
    app_assets *Assets;
    asset_id ID;
    char *FilePath;
    u32 Tags;
};

inline gl_texture *
GetTexture(app_assets *Assets, asset_id ID)
{
    gl_texture *Result = 0;
    
    if (Assets->Textures[ID].ID)
    {        
        Result = &Assets->Textures[ID];
    }

    return Result;
}

#endif
