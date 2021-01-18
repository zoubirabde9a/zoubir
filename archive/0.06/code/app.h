#if !defined(APP_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */
/*
  //NOTE(soubir):
  APP_SLOW:
  1- slow code allowed
  0- fast code only
  APP_DEV:
  1- build for developer only
  0- build for public release
*/
// TODO(zoubir):
// make a dash ability and a new DASHINg animation type
// Fix Weird bug in soda.wav not clean sound at the end.
// Next Todo Is Wraping Get Asset Calls
// Z Pos as v3 mb
//TODO(zoubir): more Todos
// remove entity does not work
   // make it so that the entity has a boolean IsValid or smth
//make a better MoveEntityFunction with delta time
// by making it able to move at high speed and not
  // fail
// make a familliar
// make pair wise collision rules for familliar
// make a flowing
// tile lighting
// layers and map levels(Z)
// audio mixer
// CameraShake

#include "app_platform.h"

#define Pi32 3.14159265359f
// NOTE(zoubir): Services that the app provides to the platform layer

struct font
{    
    u32 Texture;
    u32 BitmapWidth;
    u32 BitmapHeight;
    float UpperLimit;
    float LowerLimit;
    void *Glyphs;
    u32 FirstGlyph;
    u32 GlyphsSize;
};

typedef size_t memory_index;

struct memory_arena
{
    u8 *Base;
    memory_index Size;
    memory_index Used;
    u32 TempCount;
};

struct temporary_memory
{
    memory_arena *Arena;
    memory_index Used;
};

inline void
InitializeArena(memory_arena *Arena,
                      memory_index *Base, memory_index Size)
{
    Arena->Base = (u8 *)Base;
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->TempCount = 0;
}

inline memory_index
GetAlignmentOffset(memory_arena *Arena, memory_index Alignment)
{
    memory_index Offset = 0;
    memory_index Ptr = (memory_index)Arena->Base + Arena->Used;
    memory_index Mask = Alignment - 1;
    if (Ptr & Mask)
    {
        Offset = Alignment - (Ptr & Mask);
    }
    
    return Offset;
}

inline memory_index
GetArenaSizeRemaining(memory_arena *Arena, memory_index Alignment)
{
    memory_index SizeRemaining;
    SizeRemaining = Arena->Size - (Arena->Used + GetAlignmentOffset(Arena, Alignment));
    return SizeRemaining;
}

inline void *
AllocateSize(memory_arena *Arena, memory_index PrefSize, memory_index Alignment = 4)
{
    memory_index Size = PrefSize;

    memory_index AlignmentOffset = GetAlignmentOffset(Arena, Alignment);
    Size += AlignmentOffset;

    Assert(Arena->Size >=(Arena->Used + Size));
    void *Result = Arena->Base + Arena->Used + AlignmentOffset;
    Arena->Used += Size;

    return Result;
}

#define AllocateStruct(Arena, Type, ...) (Type *)AllocateSize(Arena, sizeof(Type), ##__VA_ARGS__)
#define AllocateArray(Arena, Count, Type, ...) (Type *)AllocateSize(Arena, (Count) * sizeof(Type), ##__VA_ARGS__)

inline char *
AllocateString(memory_arena *Arena, char *Source)
{
    memory_index StringSize = strlen(Source) + 1;
    char *Dest = AllocateArray(Arena, StringSize, char);
    memcpy(Dest, Source, StringSize);
    return Dest;
}


inline temporary_memory
BeginTemporaryMemory(memory_arena *Arena)
{
    temporary_memory Result;

    Result.Arena = Arena;
    Result.Used = Arena->Used;
    Arena->TempCount++;

    return Result;
}

inline void
ZeroSize(void *Ptr, memory_index Size)
{
    #if 0
    u8 *BytePtr = (u8 *)Ptr;
    while(Size--)
    {
        *BytePtr++ = 0;
    }
    #endif
    memset(Ptr, 0, Size);
}
#define ZeroArray(Array, Count, Type) ZeroSize(Array, (Count) * sizeof(Type))

inline void
CopyString(char *Dest, memory_index DestSize, char *Src)
{
    while(*Src)
    {        
        *Dest = *Src++;
        DestSize--;
        if (DestSize == 0)
        {
            break;
        }
    }
}

inline void
EndTemporaryMemory(temporary_memory TempMemory)
{
    memory_arena *Arena = TempMemory.Arena;
    Assert(Arena->Used >= TempMemory.Used);
    Assert(Arena->TempCount > 0);

    memory_index MemoryFreed = Arena->Used - TempMemory.Used;
    Arena->Used = TempMemory.Used;    
    Arena->TempCount--;
    ZeroSize(Arena->Base + Arena->Used, MemoryFreed);
}

inline void
SubArena(memory_arena *Dest, memory_arena *Src,
         memory_index Size, memory_index Alignment = 16)
{
    Dest->Size = Size;
    Dest->Base = (u8 *)AllocateSize(Src, Size, Alignment);
    Dest->Used = 0;
    Dest->TempCount = 0;
}
// TODO(zoubir): Remove Those
#include "stdlib.h"
#include "string.h"
#include "stdio.h"



#include "utility.h"
#include "asset.h"
#include "ui.h"
#include "world.h"
#include "audio.h"

// asset_id
struct pairwise_collision_rule
{
    u32 EntityA;
    u32 EntityB;

    bool32 Tag;
    pairwise_collision_rule *Next;
};

global_variable platform_add_work_entry *PlatformAddWorkEntry;
global_variable debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
global_variable debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
global_variable debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;

enum player_delayed_input_type
{
    PDI_Move,
    PDI_Attack,
    PDI_Cast,
    PDI_Count
};

struct player_delayed_input
{
    player_delayed_input_type Type;
    float TimeRemaining;
    v2 Dir;
};

struct app_state
{
    bool32 IsInitialized;
    memory_arena MemoryArena;
    memory_arena ConstantsArena;

    player_delayed_input PlayerDelayedInput[32];
    u32 PlayerDelayedInputCount;
    
    font *DefaultFont;    
    texture_cache *TextureCache;
    ui_context *UIContext;

    world World;
    u32 UpdateID;
    
    world_entity *Player;

    v3 TargetCamera;
    v3 CameraOffset;
    
    assets Assets;

    audio_state AudioState;

    ui_state TilePickerWidget;
    bool32 TileEditing;

    bool32 EntityCollisionTable[EntityType_Count][EntityType_Count];

    //NOTE(zoubir): Power Of 2
    pairwise_collision_rule *CollisionRuleHash[256];
    pairwise_collision_rule *FirstFreeCollisionRule;


    entity_collision_volume_group *TileObjectCollision;
    entity_collision_volume_group *TreeCollision;
    entity_collision_volume_group *WallCollision;
    entity_collision_volume_group *PlayerCollision;
    entity_collision_volume_group *FamiliarCollision;
    entity_collision_volume_group *FireBallCollision;
    entity_collision_volume_group *SwordCollision;

    animation_set ZoubirAnimationSet;
    animation_set FamiliarAnimationSet;
    animation_set FireballAnimationSet;
    animation_set SwordAnimationSet;
    

    platform_work_queue *WorkQueue;

    opengl_texture_queue OpenglTextureQueue;
    open_gl *OpenGL;
};

inline void
AddPlayerDelayedAttack(app_state *AppState,
                      float TimeRemaining, v2 Dir)
{
    Assert(AppState->PlayerDelayedInputCount < ArrayCount(AppState->PlayerDelayedInput));
    if (AppState->PlayerDelayedInputCount < ArrayCount(AppState->PlayerDelayedInput))
    {
        player_delayed_input *NewInput =
            &AppState->PlayerDelayedInput[AppState->PlayerDelayedInputCount++];
        NewInput->Type = PDI_Attack;
        NewInput->TimeRemaining = TimeRemaining;
        NewInput->Dir = Dir;
    }
}

inline void
AddPlayerDelayedMove(app_state *AppState,
                     float TimeRemaining,
                      v2 Dir)
{
    Assert(AppState->PlayerDelayedInputCount < ArrayCount(AppState->PlayerDelayedInput));
    if (AppState->PlayerDelayedInputCount < ArrayCount(AppState->PlayerDelayedInput))
    {
        player_delayed_input *NewInput =
            &AppState->PlayerDelayedInput[AppState->PlayerDelayedInputCount++];
        NewInput->Type = PDI_Move;
        NewInput->TimeRemaining = TimeRemaining;
        NewInput->Dir = Dir;
    }
}

inline void
AddPlayerDelayedCast(app_state *AppState,
                     float TimeRemaining,
                      v2 Dir)
{
    Assert(AppState->PlayerDelayedInputCount < ArrayCount(AppState->PlayerDelayedInput));
    if (AppState->PlayerDelayedInputCount < ArrayCount(AppState->PlayerDelayedInput))
    {
        player_delayed_input *NewInput =
            &AppState->PlayerDelayedInput[AppState->PlayerDelayedInputCount++];
        NewInput->Type = PDI_Cast;
        NewInput->TimeRemaining = TimeRemaining;
        NewInput->Dir = Dir;
    }
}

struct transient_state
{
    bool32 IsInitialized;
    memory_arena MemoryArena;
};

internal void
AddCollisionRule(app_state *AppState, memory_arena *Arena,
                 u32 EntityA, u32 EntityB,
                 bool32 Tag);

internal bool32
CanCollide(app_state *AppState, world_entity *A,
           world_entity *B);
inline void
SetCollision(app_state *AppState, entity_type A,
             entity_type B, bool32 Collides)
{
    AppState->EntityCollisionTable[A][B] = Collides;
    AppState->EntityCollisionTable[B][A] = Collides;
}

inline bool32
CanCollide(app_state *AppState, entity_type A,
             entity_type B)
{
    bool32 Result = AppState->EntityCollisionTable[A][B];
    return Result;
}

#define APP_H
#endif
