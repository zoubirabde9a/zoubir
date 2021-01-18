#if !defined(UTILITY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define UTILITY_H

enum texture_cache_element_type
{
    TEXTURE_CACHE_ELEMENT_EMPTY,
    TEXTURE_CACHE_ELEMENT_TOMB,
    TEXTURE_CACHE_ELEMENT_VALUE
};

struct texture_cache_element
{
    gl_texture Texture;
    char *Key;
    u32 KeyLength;
    int Type;
};

struct texture_cache
{
    u32 AllocatedTextureCount;
    u32 TextureCount;
    char *CurrentCharacterBlockPointer;
    texture_cache_element *Textures;
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
    i16 *Data;
    u32 SampleCount;
    u32 Channels;    
};

#endif
