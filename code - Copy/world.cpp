/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */
#include "world.h"

inline cannonical_position
CannonicalizePosition(v3 AbsolutePosition, u32 TileWidth, u32 TileHeight,
                      u32 TileDepth)
{
    cannonical_position Result;

    Result.TileX = (u32)(AbsolutePosition.X / TileWidth);
    Result.TileRel.X = AbsolutePosition.X -
        (Result.TileX * TileWidth);
    
    Result.TileY = (u32)(AbsolutePosition.Y / TileHeight);
    Result.TileRel.Y = AbsolutePosition.Y -
        (Result.TileY * TileHeight);    
    
    Result.TileZ = (u32)(AbsolutePosition.Z / TileDepth);
    Result.TileRel.Z = AbsolutePosition.Z -
        (Result.TileZ * TileDepth);
    
    return Result;
}

v4 GetTextureUvsFromIndex(u32 TextureWidth, u32 TextureHeight,
                          u32 TileNumX, u32 TileNumY,
                          u32 Index)
{

    v4 Result;
    
    Assert(TileNumX * (TextureWidth / TileNumX) == TextureWidth);
    Assert(TileNumY * (TextureHeight / TileNumY) == TextureHeight);
    
    float UvWidth = 1.f / TileNumX;
    float UvHeight = 1.f / TileNumY;
    
    Result.X = ((UvWidth) * (Index % TileNumX));
    Result.Y = ((UvHeight) * ((Index / TileNumX) % TileNumY));
    Result.Z = Result.X + UvWidth;
    Result.W = Result.Y + UvHeight;
    
    return Result;
}

inline animation_slot *
GetAnimation(animation_set *Set, animation_type Type,
             animation_direction Direction)
{
    animation_slot *Result =
        &Set->Animations[Type][Direction];
    return Result;
}

internal v4
DoAnimation(animation_state *State,
            u32 TextureWidth, u32 TextureHeight,
            u32 TileNumX, u32 TileNumY,
            float DeltaTime,
            float SecondsPerFrame, animation_slot Animation)
{
    v4 Result;
    
    State->DeltaTime += DeltaTime;
    if (State->DeltaTime >= SecondsPerFrame)
    {
        State->DeltaTime -= SecondsPerFrame;
        State->SlotIndex =
            ((State->SlotIndex + 1) % Animation.IndicesCount);
        
    }

    u32 Index = Animation.FirstIndex + State->SlotIndex;
    Result = GetTextureUvsFromIndex(TextureWidth, TextureHeight,
                                    TileNumX, TileNumY, Index);
    return Result;
}

inline v4
DoAnimation(animation_state *State,
            u32 TextureWidth, u32 TextureHeight,
            u32 TileNumX, u32 TileNumY,
            float DeltaTime,
            float SecondsPerFrame, animation_set *Set,
            animation_type AnimationType,
            animation_direction AnimationDirection)
{
    Assert(AnimationType < ANIMATION_TYPE_COUNT);
    Assert(AnimationDirection < ANIMATION_DIRECTION_COUNT);
    Assert(State);
    v4 Result;
    
    animation_slot *Animation =
        GetAnimation(Set, AnimationType, AnimationDirection);
    
    // NOTE(zoubir): Animation Does Not Exit
    Assert(Animation->IndicesCount > 0);
    
    if (Set->LastAnimationDirection != AnimationDirection)
    {
        State->DeltaTime = 0.f;
        State->SlotIndex = 0;
        u32 Index = Animation->FirstIndex;
        Result = GetTextureUvsFromIndex(TextureWidth, TextureHeight,
                                        TileNumX, TileNumY, Index);
    }
    else
    {
        Result = DoAnimation(State, TextureWidth, TextureHeight,
                    TileNumX, TileNumY, DeltaTime, SecondsPerFrame,
                    *Animation);
                    
    }
    Set->LastAnimationDirection = AnimationDirection;
    return Result;
};

inline world_chunk *
GetChunk(world *World, u32 ChunkX, u32 ChunkY, u32 ChunkZ)
{
    Assert(ChunkX < CHUNK_MAX_X);
    Assert(ChunkY < CHUNK_MAX_Y);
    Assert(ChunkZ < CHUNK_MAX_Z);
    world_chunk *Chunk = &World->Chunks[ChunkX][ChunkY][ChunkZ];
    return Chunk;
}

inline world_chunk *
GetChunk(world *World, v3 Position)
{    
    u32 ChunkX = (u32)(Position.X / (World->TilesPerChunkX * World->TileWidth));
    u32 ChunkY = (u32)(Position.Y / (World->TilesPerChunkY * World->TileHeight));
    u32 ChunkZ = (u32)(Position.Z / (World->TilesPerChunkZ * World->TileDepth));
    world_chunk *Chunk = GetChunk(World, ChunkX, ChunkY, ChunkZ);
    return Chunk;
}

internal void
InsertEntity(world *World,
             memory_arena *Arena,
             world_entity_chunk *FirstEntityChunk,
             world_entity *NewEntity)
{
    FirstEntityChunk->Entities[FirstEntityChunk->EntityCount++] =
        NewEntity;
    // If the FirstChunk is Full copy it to the next
    if (FirstEntityChunk->EntityCount >= ArrayCount(FirstEntityChunk->Entities))
    {
        world_entity_chunk *OldEntityChunk;

        if (World->FirstFreeChunk)
        {
            // grap a free chunk
            OldEntityChunk = World->FirstFreeChunk;
            World->FirstFreeChunk = OldEntityChunk->Next;
        }
        else
        {
            // No free chunk was found allocate a new one            
            OldEntityChunk =
                AllocateStruct(Arena, world_entity_chunk);
        }
        *OldEntityChunk = *FirstEntityChunk;
        FirstEntityChunk->EntityCount = 0;
        FirstEntityChunk->Next = OldEntityChunk;
    }   
}
internal bool32
RemoveEntity(world *World,
             world_chunk *Chunk,
             world_entity *Entity)
{
    bool32 Result = false;
    world_entity_chunk *FirstEntityChunk = &Chunk->FirstEntityChunk;
    world_entity_chunk *EntityChunk = FirstEntityChunk;
    do {
        for(u32 EntityIndex = 0;
            EntityIndex < EntityChunk->EntityCount;
            EntityIndex++)
        {
            world_entity *ThisEntity = EntityChunk->Entities[EntityIndex];
            if (ThisEntity == Entity)
            {
                Result = true;
                // swap with last element
                if (FirstEntityChunk->EntityCount > 0)
                {
                    u32 LastIndex = FirstEntityChunk->EntityCount - 1;
                    EntityChunk->Entities[EntityIndex] =
                        FirstEntityChunk->Entities[LastIndex];
                    FirstEntityChunk->EntityCount--;
                }
                else
                {
                    // remove first chunk
                    u32 LastIndex = EntityChunk->EntityCount - 1;
                    EntityChunk->Entities[EntityIndex] =
                        EntityChunk->Entities[LastIndex];
                    EntityChunk->EntityCount--;
                    *FirstEntityChunk = *EntityChunk;
                    // move it to the free list
                    EntityChunk->EntityCount = 0;
                    EntityChunk->Next = World->FirstFreeChunk;
                    World->FirstFreeChunk = EntityChunk;
                }
                break;
            }
        }
        EntityChunk = EntityChunk->Next;
    } while(EntityChunk);
    
    return Result;
}

internal world_entity *
AddEntity(world *World, memory_arena *Arena,
          v3 Position)
{
    Assert(ArrayCount(World->Entities) > World->EntityCount);
    world_entity *NewEntity = 
        World->Entities + World->EntityCount++;
    
    NewEntity->Position = Position;
    world_chunk *Chunk = GetChunk(World, Position);

    // Add Entity Pointer Into The Chunk
    world_entity_chunk *FirstEntityChunk = &Chunk->FirstEntityChunk;
    
    InsertEntity(World, Arena, FirstEntityChunk, NewEntity);
    
    return NewEntity;
}

//TODO(zoubir): Decrement Entity Count 
inline bool32
RemoveEntity(world *World, world_entity *Entity)
{
    world_chunk *Chunk =
        GetChunk(World, Entity->Position);
    bool32 Result = RemoveEntity(World, Chunk, Entity);
    Assert(Result);
    Entity->IsPresent = false;
    return Result;
}

internal void
CheckAndChangeEntityChunk(world *World, memory_arena *Arena,
                          v3 OldPosition,
                          world_entity *Entity)
{
    world_chunk *TargetChunk =
        GetChunk(World, Entity->Position);
    world_chunk *OldChunk =
        GetChunk(World, OldPosition);
    bool32 Removed = 0;
    
    if (TargetChunk != OldChunk)
    {
        Removed = RemoveEntity(World, OldChunk, Entity);
    #if 0
        // remove the entity from the Old chunk
        world_entity_chunk *FirstEntityChunk = &OldChunk->FirstEntityChunk;
        world_entity_chunk *EntityChunk = FirstEntityChunk;
        do {
            for(u32 EntityIndex = 0;
                EntityIndex < EntityChunk->EntityCount;
                EntityIndex++)
            {
                world_entity *ThisEntity = EntityChunk->Entities[EntityIndex];
                if (ThisEntity == Entity)
                {
                    FoundEntity = Entity;
                    // swap with last element
                    if (FirstEntityChunk->EntityCount > 0)
                    {
                        u32 LastIndex = FirstEntityChunk->EntityCount - 1;
                        EntityChunk->Entities[EntityIndex] =
                            FirstEntityChunk->Entities[LastIndex];
                        FirstEntityChunk->EntityCount--;
                    }
                    else
                    {
                        // remove first chunk
                        u32 LastIndex = EntityChunk->EntityCount - 1;
                        EntityChunk->Entities[EntityIndex] =
                            EntityChunk->Entities[LastIndex];
                        EntityChunk->EntityCount--;
                        *FirstEntityChunk = *EntityChunk;
                        // move it to the free list
                        EntityChunk->EntityCount = 0;
                        EntityChunk->Next = World->FirstFreeChunk;
                        World->FirstFreeChunk = EntityChunk;
                    }
                    break;
                }
            }
            EntityChunk = EntityChunk->Next;
        } while(EntityChunk);
        #endif
        
        Assert(Removed);
        // add the entity into the target chunk
        InsertEntity(World, Arena,
                     &TargetChunk->FirstEntityChunk, Entity);
    }    
}
