/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */
#include "utility.h"

inline bool32
IsMouseOnRectangle(i32 MouseX, i32 MouseY,
                   float X, float Y, float Width, float Height)
{
    return MouseX >= X && MouseX <= X + Width &&
        MouseY >= Y && MouseY <= Y + Height;
}

internal bool32
CompareStrings(char *StringA, u32 StringALength,
               char *StringB, u32 StringBLength)
{
    bool32 Result = false;
    if (StringALength != StringBLength)
    {
        return Result;
    }
    Result = memcmp(StringA, StringB, StringALength) == 0;
    return Result;
}

struct hash_string_result
{
    u32 KeyHash;
    u32 KeyLength;
};

hash_string_result HashString(char *String)
{
    hash_string_result Result = {};
    while(*String)
    {
        Result.KeyHash ^= *String;
        Result.KeyHash *= 16777619;        
        Result.KeyLength++;
        String++;
    }
    return Result;
}

internal texture_cache *
TextureCacheCreate(memory_arena *Arena,
                     u32 AllocatedTextureCount, u32 AllocatedCharacterCount)
{
    //TODO(zoubir): check if the memory is zeroed
    texture_cache *Cache = AllocateStruct(Arena, texture_cache);
    
    Cache->AllocatedTextureCount = AllocatedTextureCount;
    Cache->TextureCount = 0;
    Cache->CurrentCharacterBlockPointer =
        AllocateArray(Arena, AllocatedCharacterCount, char);
    Cache->Textures = AllocateArray(Arena, AllocatedTextureCount, texture_cache_element);
    
    return Cache;
}

inline void
TextureCacheInsertElement(char **AllocatedCharacterBlock,
                          texture_cache_element *Element,
                          char *Key, u32 KeyLength, gl_texture Texture)
{
    Element->Texture = Texture;
    memcpy(*AllocatedCharacterBlock, Key, KeyLength);
    Element->Key = *AllocatedCharacterBlock;
    *AllocatedCharacterBlock+= KeyLength;
    Element->KeyLength = KeyLength;
    Element->Type = TEXTURE_CACHE_ELEMENT_VALUE;            
}

internal void
TextureCacheInsert(texture_cache *Cache,
                   char *Key, gl_texture Texture)
{
    Assert(Cache->TextureCount < Cache->AllocatedTextureCount);
    hash_string_result KeyHash = HashString(Key);
    u32 Index = KeyHash.KeyHash % Cache->AllocatedTextureCount;

    // walk the array until an empty slot or a tomb is found
    texture_cache_element *Elements = Cache->Textures;
    for(u32 CurrentElementIndex = Index;
        CurrentElementIndex < Cache->AllocatedTextureCount;
        CurrentElementIndex++)
    {
        texture_cache_element *ThisElement = &Elements[CurrentElementIndex];
        // if an empty slot is found
        if (ThisElement->Type == TEXTURE_CACHE_ELEMENT_EMPTY)
        {
            TextureCacheInsertElement(&Cache->CurrentCharacterBlockPointer,
                                      ThisElement, Key, KeyHash.KeyLength,
                                      Texture);
            return;
        }
        // if a tomb is found
        if (ThisElement->Type == TEXTURE_CACHE_ELEMENT_TOMB)
        {
            TextureCacheInsertElement(&Cache->CurrentCharacterBlockPointer,
                                      ThisElement, Key, KeyHash.KeyLength,
                                      Texture);
            return;
        }
    }
    for(u32 CurrentElementIndex = 0;
        CurrentElementIndex < Index;
        CurrentElementIndex++)
    {        
        texture_cache_element *ThisElement = &Elements[CurrentElementIndex];
        // if an empty slot is found
        if (ThisElement->Type == TEXTURE_CACHE_ELEMENT_EMPTY)
        {
            TextureCacheInsertElement(&Cache->CurrentCharacterBlockPointer,
                                      ThisElement, Key, KeyHash.KeyLength,
                                      Texture);
            return;
        }
        // if a tomb is found
        if (ThisElement->Type == TEXTURE_CACHE_ELEMENT_TOMB)
        {
            TextureCacheInsertElement(&Cache->CurrentCharacterBlockPointer,
                                      ThisElement, Key, KeyHash.KeyLength,
                                      Texture);
            return;
        }
    }
}

internal gl_texture
TextureCacheGet(texture_cache *Cache, char *Key)
{
    gl_texture Result = {};
    
    hash_string_result KeyHash = HashString(Key);
    u32 Index = KeyHash.KeyHash % Cache->AllocatedTextureCount;
    
    texture_cache_element *Elements = Cache->Textures;
    for(u32 CurrentElementIndex = Index;
        CurrentElementIndex < Cache->AllocatedTextureCount;
        CurrentElementIndex++)
    {
        texture_cache_element *ThisElement = &Elements[CurrentElementIndex];
        // if a slot is found
        if (ThisElement->Type == TEXTURE_CACHE_ELEMENT_VALUE
            && CompareStrings(ThisElement->Key, ThisElement->KeyLength,
                              Key, KeyHash.KeyLength))
        {
            Result = ThisElement->Texture;
            return Result;
        }
    }
    for(u32 CurrentElementIndex = 0;
        CurrentElementIndex < Index;
        CurrentElementIndex++)
    {        
        texture_cache_element *ThisElement = &Elements[CurrentElementIndex];
        // if a slot is found
        if (ThisElement->Type == TEXTURE_CACHE_ELEMENT_VALUE
            && CompareStrings(ThisElement->Key, ThisElement->KeyLength,
                              Key, KeyHash.KeyLength))
        {
            Result = ThisElement->Texture;
            return Result;
        }
    }
    return Result;
}

inline gl_texture
AcquireTexture(app_state *AppState, char *TextureName)
{
    return TextureCacheGet(AppState->TextureCache, TextureName);
}

//NOTE(zoubir): Cannot exceed 32 bit File Size
loaded_audio LoadWav(thread_context *Thread, app_memory *Memory,
                 char* FilePath) {
    loaded_audio Result = {};
    
    debug_read_file_result ReadResult
        = Memory->DEBUGPlatformReadEntireFile(Thread, FilePath);

    if (ReadResult.Memory == 0) {
        InvalidCodePath;
        return Result;
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
        Result.Data = (i16 *)(Header + 1);
        Result.SampleCount = Header->DataSize / Header->BytesPerSample;
        Result.Channels = Header->Channels;
    }
    
    return Result;
}


