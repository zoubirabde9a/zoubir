/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */

#include "asset.h"

//NOTE(zoubir): Cannot exceed 32 bit File Size
loaded_audio LoadWav(memory_arena *AssetArena, char* FilePath) {
    loaded_audio Audio = {};
    
    debug_read_file_result ReadResult
        = DEBUGPlatformReadEntireFile(FilePath);

    if (ReadResult.Memory == 0) {
        InvalidCodePath;
    }
    
    void *FileContents = ReadResult.Memory;
    u32 FileSize = ReadResult.Size;

    wav_header *Header = (wav_header *)ReadResult.Memory;
    if (Header->FileTypeBlockID32 != 0x46464952 ||
        Header->FileFormatID32 != 0x45564157||
        Header->FormatBlockID32 != 0x20746D66||
        Header->DataBlockID32 != 0x61746164)
    {
        InvalidCodePath;
    }
    else
    {
        i16 *Data = (i16 *)(Header + 1);
        u32 SampleCount = Header->DataSize / Header->BytesPerSample / Header->Channels;
        u32 Channels = Header->Channels;
        for(u32 ChannelIndex = 0;
            ChannelIndex < Channels;
            ChannelIndex++)
        {
            Audio.Samples[ChannelIndex] =
                AllocateArray(AssetArena, SampleCount, i16);
        }
        for(u32 SampleIndex = 0;
            SampleIndex < SampleCount;
            SampleIndex++)
        {
            for(u32 ChannelIndex = 0;
                ChannelIndex < Channels;
                ChannelIndex++)
            {
                Audio.Samples[ChannelIndex][SampleIndex] =
                    Data[SampleIndex * Channels + ChannelIndex];
            }
        }
        Audio.SampleCount = SampleCount;
        Audio.Channels = Channels;
    }

    DEBUGPlatformFreeFileMemory(ReadResult.Memory);
    return Audio;
}

internal void
AddOpenglTextureToQueue(opengl_texture_queue *Queue,
                        assets *Assets,
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
            assets *Assets,
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

inline void
UploadAudio(assets *Assets, loaded_audio Audio,
              asset_id ID)
{
    asset *Asset = &Assets->Assets[ID.Type].Array[ID.Index];
    Asset->Audio = Audio;

    CompletePreviousWritesBeforeFutureWrites;
    Asset->State = AssetState_Loaded;
}

internal void
LoadAudio(assets *Assets,
            asset_id ID,
            char *FilePath)
{
    loaded_audio Audio = LoadWav(&Assets->Arena, FilePath);
    UploadAudio(Assets, Audio, ID);
}

inline void
UploadTexture(assets *Assets, gl_texture Texture,
              asset_id ID)
{
    asset *Asset = &Assets->Assets[ID.Type].Array[ID.Index];
    Asset->Texture = Texture;
    
    CompletePreviousWritesBeforeFutureWrites;
    Asset->State = AssetState_Loaded;
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


    UploadTexture(Entry->Assets, Texture, Entry->ID);
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

internal PLATFORM_WORK_QUEUE_CALLBACK(LoadAssetWork)
{
    load_asset_work *Work = (load_asset_work *)Data;

    switch(Work->Family)
    {
        case AssetFamily_Texture:
        {
            LoadTexture(Work->Queue, Work->Assets,
                        Work->ID,
                        Work->FilePath, Work->Tags);
            break;
        }
        case AssetFamily_Audio:
        {
            LoadAudio(Work->Assets, Work->ID, Work->FilePath);
            break;
        }
    };
}

internal void
InitializeAssets(assets *Assets, memory_arena *MemoryArena)
{
    memory_arena *AssetArena = &Assets->Arena;
    SubArena(AssetArena, MemoryArena, Megabytes(32));
    
    asset_info_array *Infos =
        &Assets->TexturesInfo[0];
    Infos->AllocatedCount = 1;    
    Infos->InfoCount = 1;
    Infos->Infos = AllocateArray(AssetArena,
                                 1, asset_info);
    asset_slot *Textures =
        &Assets->Assets[0];
    Textures->AllocatedCount = 1;
    Textures->ArrayCount = 1;
    Textures->Array = AllocateArray(AssetArena, 1,
                                    asset);
}


inline void
BeginAssetType(assets *Assets,
               asset_type Type, u32 Count)
{
    Assert(Assets->DEBUGCurrentAssetType == 0);
    memory_arena *AssetArena = &Assets->Arena;
    Assets->DEBUGCurrentAssetType = Type;
    asset_info_array *Infos =
        &Assets->TexturesInfo[Type];

    Infos->AllocatedCount = Count;    
    Infos->Infos = AllocateArray(AssetArena,
                                 Count, asset_info);
    
    asset_slot *Textures =
        &Assets->Assets[Type];

    Textures->AllocatedCount = Count;
    Textures->Array = AllocateArray(AssetArena, Count,
                                    asset);
}

inline asset_info *
GetAssetInfo(assets *Assets, asset_id ID)
{    
    Assert(ID.Type < AssetType_Count);
    asset_info_array *InfoArray = &Assets->TexturesInfo[ID.Type];
    Assert(ID.Index < InfoArray->InfoCount);
    asset_info * Result = &InfoArray->Infos[ID.Index];
    
    return Result;    
}

inline asset *
GetAsset(assets *Assets, asset_id ID)
{    
    Assert(ID.Type < AssetType_Count);
    asset_slot *Slot = &Assets->Assets[ID.Type];
    Assert(ID.Index < Slot->ArrayCount);
    asset *Asset = &Slot->Array[ID.Index];
    return Asset;
}

inline gl_texture *
GetTexture(assets *Assets, asset_id ID)
{
    gl_texture *Result = 0;
    Assert(ID.Type < AssetType_Count);
    asset_slot *Slot = &Assets->Assets[ID.Type];
    Assert(ID.Index < Slot->ArrayCount);
    asset *Asset = &Slot->Array[ID.Index];
    
    if (Asset->State == AssetState_Loaded)
    {
        Result = &Asset->Texture;
    }

    return Result;
}
inline loaded_audio *
GetAudio(assets *Assets, asset_id ID)
{
    loaded_audio *Result = 0;
    Assert(ID.Type < AssetType_Count);
    asset_slot *Slot = &Assets->Assets[ID.Type];
    Assert(ID.Index < Slot->ArrayCount);
    asset *Asset = &Slot->Array[ID.Index];
    
    if (Asset->State == AssetState_Loaded)
    {
        Result = &Asset->Audio;
    }

    return Result;
}

inline void
AddTextureAsset(assets *Assets, char *FilePath, v2 Origin, u32 Tags)
{
    asset_info_array *Infos =
        &Assets->TexturesInfo[Assets->DEBUGCurrentAssetType];
    asset_slot *Textures =
        &Assets->Assets[Assets->DEBUGCurrentAssetType];

    Assert(Infos->InfoCount < Infos->AllocatedCount);
    Assert(Textures->ArrayCount < Textures->AllocatedCount);
        
    asset_info *NewInfo = &Infos->Infos[Infos->InfoCount];
    NewInfo->Family = AssetFamily_Texture;
    // TODO(zoubir): make a temporary memory to
    // Push Strings in it
    NewInfo->FilePath = AllocateString(&Assets->Arena, FilePath);
    NewInfo->Tags = Tags;
    NewInfo->Origin = Origin;

    Infos->InfoCount++;
    Textures->ArrayCount++;
}

inline void
AddAudioAsset(assets *Assets, char *FilePath)
{
    asset_info_array *Infos =
        &Assets->TexturesInfo[Assets->DEBUGCurrentAssetType];
    asset_slot *Textures =
        &Assets->Assets[Assets->DEBUGCurrentAssetType];

    Assert(Infos->InfoCount < Infos->AllocatedCount);
    Assert(Textures->ArrayCount < Textures->AllocatedCount);
        
    asset_info *NewInfo = &Infos->Infos[Infos->InfoCount];
    NewInfo->Family = AssetFamily_Audio;
    // TODO(zoubir): make a temporary memory to
    // Push Strings in it
    NewInfo->FilePath = AllocateString(&Assets->Arena, FilePath);

    Infos->InfoCount++;
    Textures->ArrayCount++;
}

inline void
EndAssetType(assets *Assets)
{
    Assert(Assets->DEBUGCurrentAssetType != 0);
    Assets->DEBUGCurrentAssetType = AssetType_Invalid;
}

internal void
LoadAsset(assets *Assets,
          app_state *AppState,
          app_memory *Memory,
          memory_arena *AssetArena,
          asset_id ID)
{
    Assert(ID.Type);
    asset *Asset = GetAsset(Assets, ID);
    if (AtomicCompareExchangeU32((u32 *)&Asset->State, AssetState_Queued, AssetState_Unloaded) ==
        AssetState_Unloaded)
    {
        asset_info *Info = GetAssetInfo(Assets, ID);
        load_asset_work *Work = AllocateStruct(AssetArena, load_asset_work);
        Work->Queue = &AppState->OpenglTextureQueue;
        Work->Memory = Memory;
        Work->Assets = Assets;
        Work->ID = ID;
        Work->Family = Info->Family;
        Work->FilePath = Info->FilePath;
        Work->Tags = Info->Tags;    
    
        PlatformAddWorkEntry(AppState->WorkQueue,
                             LoadAssetWork, Work);
    }
}
