/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */

#include "assets.h"
internal void
AddOpenglTextureToQueue(opengl_texture_queue *Queue,
                        app_assets *Assets,
                        asset_id ID, void *MemoryToFree,
                        void *TextureMemory, u32 Width,
                        u32 Height, i32 ImageFormat,
                        u32 Flags)
{
    u32 NextEntryToWrite =
        (Queue->CurrentEntryToWrite + 1) %
        ArrayCount(Queue->Entries);
    opengl_texture_queue_entry *Entry =
        &Queue->Entries[Queue->CurrentEntryToWrite];
    Entry->ID = ID;
    Entry->Assets = Assets;
    Entry->MemoryToFree = MemoryToFree;
    Entry->TextureMemory = TextureMemory;
    Entry->Width = Width;
    Entry->Height = Height;
    Entry->ImageFormat = ImageFormat;
    Entry->Flags = Flags;
    
    CompletePreviousWritesBeforeFutureWrites;
    
    Queue->CurrentEntryToWrite = NextEntryToWrite;
}

internal void
LoadTexture(opengl_texture_queue *Queue,
            app_assets *Assets,
            asset_id ID,
            char *FileName, u32 Flags)
{
    debug_read_file_result FileResult =
        DEBUGPlatformReadEntireFile(FileName);
           
    if (FileResult.Size >= sizeof(zi_header))
    {

    zi_header *Header = (zi_header *)FileResult.Memory;
    void *Data = Header + 1;

    Assert(Header->Channels == 4 || Header->Channels == 3);
    int ImageFormat = (Header->Channels == 4) ? GL_RGBA : GL_RGB;
    
    AddOpenglTextureToQueue(Queue, Assets,
                            ID, Header, Data,
                            Header->Width,
                            Header->Height,
                            ImageFormat,
                            Flags);
    }
}

internal void
LoadOpenglTextureFromEntry(open_gl *OpenGL,
                           opengl_texture_queue_entry *Entry)
{
    gl_texture Texture;
    Texture.Width = Entry->Width;
    Texture.Height = Entry->Height;
    
    glGenTextures(1, &Texture.ID);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);

    glTexImage2D(GL_TEXTURE_2D, 0, Entry->ImageFormat,
                 Entry->Width, Entry->Height, 0,
                 Entry->ImageFormat, GL_UNSIGNED_BYTE, Entry->TextureMemory);

    DEBUGPlatformFreeFileMemory(Entry->MemoryToFree);
    //Set some texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if ((Entry->Flags & TEXTURE_SOFT_FILTER) == TEXTURE_SOFT_FILTER) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    OpenGL->glGenerateMipmap(GL_TEXTURE_2D);

    Entry->Assets->Textures[Entry->ID] = Texture;
}

internal void
LoadOpenglTexturesFromQueue(open_gl *OpenGL, opengl_texture_queue *Queue)
{
    while (Queue->CurrentEntryToRead != Queue->CurrentEntryToWrite)
    {
        opengl_texture_queue_entry *Entry =
            &Queue->Entries[Queue->CurrentEntryToRead];
        LoadOpenglTextureFromEntry(OpenGL, Entry);
        Entry->TextureMemory = 0;
        Queue->CurrentEntryToRead =
            (Queue->CurrentEntryToRead + 1) %
            ArrayCount(Queue->Entries);
    }
}

internal PLATFORM_WORK_QUEUE_CALLBACK(LoadTextureWork)
{
    load_texture_work *Work = (load_texture_work *)Data;

    LoadTexture(Work->Queue, Work->Assets,
                Work->ID,
                Work->FilePath, Work->Tags);
}

internal void
LoadAsset(app_assets *Assets,
          app_state *AppState,
          app_memory *Memory,
          memory_arena *Arena,
          asset_id ID)
{
    load_texture_work *Work = AllocateStruct(Arena, load_texture_work);
    Work->Queue = &AppState->OpenglTextureQueue;
    Work->Memory = Memory;
    Work->Assets = Assets;
    Work->ID = ID;

    // TODO(zoubir): make a temporary memory to
    // Push Strings in it     
    switch(ID)
    {
        case AAI_Dragon:
        {
            Work->FilePath = "a.zi";
            Work->Tags = TEXTURE_SOFT_FILTER;
            break;
        }
        
        case AAI_Zoubir:
        {
            Work->FilePath = "zoubir.zi";
            Work->Tags = TEXTURE_SOFT_FILTER;
            break;
        }
        
        case AAI_Shadow:
        {
            Work->FilePath = "player_shadow.zi";
            Work->Tags = TEXTURE_SOFT_FILTER;
            break;
        }
        
        case AAI_TileMap:
        {
            Work->FilePath = "forest_pallet_01.zi";
            Work->Tags = TEXTURE_NO_FILTER;
            break;
        }
        
        case AAI_Tree:
        {
            Work->FilePath = "tree_summer_01.zi";
            Work->Tags = TEXTURE_SOFT_FILTER;
            break;
        }
        
        case AAI_Familiar:
        {
            Work->FilePath = "familiar.zi";
            Work->Tags = TEXTURE_SOFT_FILTER;
            break;
        }
        
        case AAI_FireBall:
        {
            Work->FilePath = "Fireball.zi";
            Work->Tags = TEXTURE_SOFT_FILTER;
            break;
        }
    }

    PlatformAddWorkEntry(AppState->WorkQueue,
                                 LoadTextureWork, Work);
}
