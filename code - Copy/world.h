#if !defined(WORLD_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define WORLD_H

enum animation_type
{
    ANIMATION_TYPE_MOVE,
    ANIMATION_TYPE_STAND,
    ANIMATION_TYPE_COUNT
};

enum animation_direction
{
    ANIMATION_DIRECTION_UP,
    ANIMATION_DIRECTION_DOWN,
    ANIMATION_DIRECTION_RIGHT,
    ANIMATION_DIRECTION_LEFT,
    ANIMATION_DIRECTION_COUNT
};

struct animation_slot
{    
    u32 FirstIndex;
    u32 IndicesCount;
};

struct animation_state
{
    float DeltaTime;
    u32 SlotIndex;
};

struct animation_set
{
    //TODO(zoubir): think about making this
    // a ptr that is shared between
    animation_slot Animations[ANIMATION_TYPE_COUNT][ANIMATION_DIRECTION_COUNT];
    
    animation_direction LastAnimationDirection;
};

struct cannonical_position
{
    u32 TileX;
    u32 TileY;
    u32 TileZ;

    v3 TileRel;
};

enum entity_type
{
    EntityType_Invalid,
    EntityType_Player,
    EntityType_StaticObject,
    EntityType_Monster,
    EntityType_Familiar,
    EntityType_FireBall,
    EntityType_Count
};

enum entity_flag
{
    EntityFlag_NonCollidable = (1 << 1),
    EntityFlag_NonSpatial = (1 << 2)
};

struct world_entity
{
    entity_type Type;
    u32 Flags;
    
    v3 Position;
    v3 Velocity;
    v3 Dimensions;
    animation_state AnimationState;
    animation_set AnimationSet;
    gl_texture *Texture;
    gl_texture *ShadowTexture;
    u32 TextureNumTilesX;
    u32 TextureNumTilesY;
    v2 TextureOrigin;
    v3 CollisionHalfDims;
    float Scale;

    // WALL
    v4 Uvs;

    // Familliar
    world_entity *FollowingEntity;
    float tFlying;
    //FireBall
    float DistanceRemaining;
    float AnimationSpeed;
    animation_type AnimationType;
    animation_direction AnimationDirection;


    bool32 IsPresent;
};

inline bool32
IsSet(world_entity *Entity, u32 Flag)
{
    bool32 Result = Entity->Flags & Flag;
    return Result;
}

inline void
AddFlag(world_entity *Entity, u32 Flag)
{
    Entity->Flags |= Flag;
}

inline void
ClearFlag(world_entity *Entity, u32 Flag)
{
    Entity->Flags &= ~Flag;
}

struct tile
{
    u32 Index;
};

struct tile_map
{
    gl_texture Texture;
    tile *Tiles;
};

#if 1
#endif

struct world_entity_chunk
{
    u32 EntityCount;
    world_entity *Entities[16];
    world_entity_chunk *Next;
};

struct world_chunk
{
    tile Tiles;
    world_entity_chunk FirstEntityChunk;
};

#define CHUNK_MAX_X 12
#define CHUNK_MAX_Y 12
#define CHUNK_MAX_Z 4

struct world
{
    tile_map TileMap;
    u32 NumTilesX;
    u32 NumTilesY;
    u32 NumTilesZ;
    
    u32 TileWidth;
    u32 TileHeight;
    u32 TileDepth;

    bool32 *CollisionMap;
    u32 NumCollisionX;
    u32 NumCollisionY;
    u32 CollisionWidth;
    u32 CollisionHeight;
    u32 CollisionDepth;

    u32 TilesPerChunkX;
    u32 TilesPerChunkY;
    u32 TilesPerChunkZ;
    
    float TileDepth_;
    world_chunk Chunks[CHUNK_MAX_X][CHUNK_MAX_Y][CHUNK_MAX_Z];
    world_entity Entities[4096];
    u32 EntityCount;
    world_entity_chunk *FirstFreeChunk;
};

#endif
