/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */

#include "asset.h"

struct load_asset_work
{
    opengl_texture_queue *Queue;
    assets *Assets;
    asset_id ID;
};

internal void
AddOpenglTextureToQueue(opengl_texture_queue *Queue,
                        assets *Assets,
                        asset_id ID,
                        void *TextureMemory, u32 Width,
                        u32 Height, i32 ImageFormat,
                        u32 Flags)
{
    opengl_texture_queue_entry NewEntry = {};
    NewEntry.ID = ID;
    NewEntry.Assets = Assets;
    NewEntry.TextureMemory = TextureMemory;
    NewEntry.Width = Width;
    NewEntry.Height = Height;
    NewEntry.ImageFormat = ImageFormat;
    NewEntry.Flags = Flags;

    bool32 DoneOperation = 0;
    while(DoneOperation == 0)
    {
        DoneOperation = 
            AtomicCompareExchangeU32(&Queue->AThreadIsAlreadyWritingAnEntry, 1, 0) == 0;
        if (DoneOperation)
        {        
            u32 NextEntryToWrite =
                (Queue->CurrentEntryToWrite + 1) %
                ArrayCount(Queue->Entries);
            opengl_texture_queue_entry *Entry =
                &Queue->Entries[Queue->CurrentEntryToWrite];
            *Entry = NewEntry;
            CompletePreviousWritesBeforeFutureWrites;
            Queue->CurrentEntryToWrite = NextEntryToWrite;
            Queue->AThreadIsAlreadyWritingAnEntry = 0;            
        }
    }
       
}

inline zas_asset_info *
GetAssetInfo(assets *Assets, asset_id ID)
{    
    Assert(ID.Type);
    Assert(ID.Type < AssetType_Count);
    zas_asset_type_slot *AssetType = &Assets->AssetTypes[ID.Type];
    Assert(ID.Index < AssetType->OnePastLastIndex - AssetType->FirstIndex);
    u32 Index = AssetType->FirstIndex + ID.Index;
    zas_asset_info *Result = &Assets->Infos[Index];
    
    return Result;    
}

inline asset *
GetAsset(assets *Assets, asset_id ID)
{    
    Assert(ID.Type);
    Assert(ID.Type < AssetType_Count);
    zas_asset_type_slot *AssetType = &Assets->AssetTypes[ID.Type];
    Assert(ID.Index < AssetType->OnePastLastIndex - AssetType->FirstIndex);
    u32 Index = AssetType->FirstIndex + ID.Index;
    asset *Asset = &Assets->Assets[Index];
    
    return Asset;
}

internal void
LoadTexture(opengl_texture_queue *Queue,
            assets *Assets,
            asset_id ID, zas_asset_info *AssetInfo)
{
    zas_texture_info *TextureInfo = &AssetInfo->Texture;
    Assert(TextureInfo->Channels == 4 || TextureInfo->Channels == 3);    
    int ImageFormat = (TextureInfo->Channels == 4) ? GL_RGBA : GL_RGB;
    
    u32 TextureSize = TextureInfo->Channels * TextureInfo->Width * TextureInfo->Height;
    //IMPORTANT memory leak
    void *Data = AllocateSize(&Assets->Arena, TextureSize);
    
    Platform.ReadDataFromFile(Assets->FileHandle,
                              AssetInfo->DataOffset,
                              TextureSize, Data);
                              
    AddOpenglTextureToQueue(Queue, Assets,
                            ID, Data,
                            TextureInfo->Width,
                            TextureInfo->Height,
                            ImageFormat,
                            TextureInfo->Tags);
}

inline void
UploadAudio(assets *Assets, loaded_audio Audio,
              asset_id ID)
{
    asset *Asset = GetAsset(Assets, ID);
//    asset *Asset = &Assets->Assets[ID.Type].Array[ID.Index];
    Asset->Audio = Audio;

    CompletePreviousWritesBeforeFutureWrites;
    Asset->State = AssetState_Loaded;
}

internal void
LoadAudio(assets *Assets,
            asset_id ID,
            zas_asset_info *Info)
{
    loaded_audio Audio;
    zas_audio_info *AudioInfo = &Info->Audio;
    u32 AudioSize = AudioInfo->Channels * AudioInfo->SampleCount * sizeof(i16);
    void *Data = AllocateSize(&Assets->Arena, AudioSize);
    
    Platform.ReadDataFromFile(Assets->FileHandle,
                              Info->DataOffset,
                              AudioSize, Data);
    
    Audio.Channels = AudioInfo->Channels;
    Audio.SampleCount = AudioInfo->SampleCount;
    
    for(u32 ChannelIndex = 0;
        ChannelIndex < Audio.Channels;
        ChannelIndex++)
    {
       Audio.Samples[ChannelIndex] =
           &((i16 *)Data)[ChannelIndex * AudioInfo->SampleCount];
    }
    
    
    UploadAudio(Assets, Audio, ID);
}

inline void
UploadTexture(assets *Assets, loaded_texture Texture,
              asset_id ID)
{
    asset *Asset = GetAsset(Assets, ID);
    Asset->Texture = Texture;
    
    CompletePreviousWritesBeforeFutureWrites;
    Asset->State = AssetState_Loaded;
}

internal PLATFORM_WORK_QUEUE_CALLBACK(LoadAssetWork)
{
    load_asset_work *Work = (load_asset_work *)Data;

    zas_asset_info *Info = GetAssetInfo(Work->Assets, Work->ID);
        
    switch(Info->Family)
    {
        case AssetFamily_Texture:
        {
            LoadTexture(Work->Queue, Work->Assets,
                        Work->ID,
                        Info);
            break;
        }
        case AssetFamily_Audio:
        {
            LoadAudio(Work->Assets, Work->ID, Info);
            break;
        }
        default:
        {
            InvalidCodePath;
        }
    };
}

internal bool32
LoadAsset(assets *Assets, app_state *AppState, asset_id ID)
{
    Assert(ID.Type);
    asset *Asset = GetAsset(Assets, ID);
    bool32 Result = 0;
    if (AtomicCompareExchangeU32((u32 volatile *)&Asset->State, AssetState_Queued, AssetState_Unloaded) ==
        AssetState_Unloaded)

    {
        //IMPORTANT(zoubir): memory leak
        //TODO(zoubir): this load_asset_work is never getting
        // freed
        load_asset_work *Work = AllocateStruct(&Assets->Arena, load_asset_work);
        
        Work->Queue = &AppState->OpenglTextureQueue;
        Work->Assets = Assets;
        Work->ID = ID;
    
        Platform.AddWorkEntry(AppState->WorkQueue,
                              LoadAssetWork, Work);
        Result = true;
    }
    
    return Result;
}

inline loaded_texture *
GetTexture(assets *Assets, app_state *AppState, asset_id ID)
{
    loaded_texture *Result = 0;
    
    asset *Asset = GetAsset(Assets, ID);
    
    if (Asset->State == AssetState_Loaded)
    {
        Result = &Asset->Texture;
    }
    else if (Asset->State == AssetState_Unloaded)
    {
        LoadAsset(Assets, AppState, ID);
    }

    return Result;
}
inline loaded_audio *
GetAudio(assets *Assets, app_state *AppState, asset_id ID)
{
    loaded_audio *Result = 0;
    asset *Asset = GetAsset(Assets, ID);
    
    if (Asset->State == AssetState_Loaded)
    {
        Result = &Asset->Audio;
    }
    else if (Asset->State == AssetState_Unloaded)
    {
        LoadAsset(Assets, AppState, ID);
    }

    return Result;
}

internal void
LoadOpenglTextureFromEntry(open_gl *OpenGL,
                           opengl_texture_queue_entry *Entry)
{
    loaded_texture Texture;
    Texture.Width = Entry->Width;
    Texture.Height = Entry->Height;
    
    glGenTextures(1, &Texture.ID);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);

    glTexImage2D(GL_TEXTURE_2D, 0, Entry->ImageFormat,
                 Entry->Width, Entry->Height, 0,
                 Entry->ImageFormat, GL_UNSIGNED_BYTE, Entry->TextureMemory);

//    DEBUGPlatformFreeFileMemory(Entry->MemoryToFree);
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

internal void
InitializeAssets(assets *Assets, app_state *AppState,
                 memory_arena *MemoryArena)
{
    memory_arena *AssetArena = &Assets->Arena;
    SubArena(AssetArena, MemoryArena, Megabytes(32));

    platform_file_group *FileGroup =
        Platform.GetAllFilesOfTypeBegin("zas");

    for(u32 FileIndex = 0;
        FileIndex < FileGroup->FileCount;
        FileIndex++)
    {
        if (FileIndex == 0)
        {
            platform_file_handle *FileHandle =
                Platform.OpenNextFile(FileGroup);

            Assets->FileHandle = FileHandle;
            
            zas_header ZASHeader;
            Platform.ReadDataFromFile(FileHandle, 0, sizeof(zas_header), &ZASHeader);
            Assert(ZASHeader.MagicValue == ZAS_MAGIC_VALUE);
            Assert(ZASHeader.Version == ZAS_VERSION);

            u32 AssetCount = ZASHeader.AssetCount;
            u64 AssetTypesOffset = ZASHeader.AssetTypesOffset;
            u64 AssetsInfosOffset = ZASHeader.AssetsInfosOffset;
        
            zas_asset_type_slot *FileAssetTypes =
                AllocateArray(AssetArena, AssetType_Count, zas_asset_type_slot);
        
            Platform.ReadDataFromFile(FileHandle, AssetTypesOffset,
                                      AssetType_Count * sizeof(zas_asset_type_slot),
                                      FileAssetTypes);
            Assets->AssetTypes = FileAssetTypes;
        
            zas_asset_info *FileAssetInfos =
                AllocateArray(AssetArena, AssetCount + 1, zas_asset_info);
            Platform.ReadDataFromFile(FileHandle, AssetsInfosOffset,
                                      (AssetCount + 1) * sizeof(zas_asset_info),
                                      FileAssetInfos);
            Assets->Infos = FileAssetInfos;
        
            Assets->Assets =
                AllocateArray(AssetArena, AssetCount, asset);
            Assets->AssetCount = AssetCount;
        }
    }

    Platform.GetAllFilesOfTypeEnd(FileGroup);

    #if 0
    for(u32 AssetTypeIndex = 1;
        AssetTypeIndex < AssetType_Count;
        AssetTypeIndex++)
    {
        LoadAsset(Assets, AppState, {(asset_type_id)AssetTypeIndex});
    }
    #endif

            

#if 0        
    u8 *AssetFileMemory = (u8 *)AssetFileResult.Memory;
    Assets->ZASBase = AssetFileMemory;
    if (AssetFileResult.Size > 0)
    {
        zas_header *ZASHeader = (zas_header *)AssetFileMemory;        
        Assert(ZASHeader->MagicValue == ZAS_MAGIC_VALUE);
        Assert(ZASHeader->Version == ZAS_VERSION);

        zas_asset_type_slot *FileAssetTypes =
            (zas_asset_type_slot *)(AssetFileMemory +
                                    sizeof(zas_header));

        Assets->AssetTypes = FileAssetTypes;
        zas_asset_info *FileAssetInfos =
            (zas_asset_info *)(AssetFileMemory +
                               ZASHeader->AssetsInfosOffset);
        Assets->AssetCount = ZASHeader->AssetCount;

        Assets->Assets =
            AllocateArray(AssetArena, Assets->AssetCount, asset);

        Assets->Infos = FileAssetInfos;        
    }

    for(u32 AssetTypeIndex = 1;
        AssetTypeIndex < AssetType_Count;
        AssetTypeIndex++)
    {
        LoadAsset(Assets, AppState, {(asset_type_id)AssetTypeIndex});
    }
    #endif
}

