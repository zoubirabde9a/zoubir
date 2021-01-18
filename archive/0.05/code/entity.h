#if !defined(ENTITY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */
#define ENTITY_H

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
    EntityType_Tiled,
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

struct entity_collision_volume
{
    v3 Offset;
    v3 HalfDims;
};

struct entity_collision_volume_group
{
    entity_collision_volume TotalVolume;
    u32 VolumesCount;
    entity_collision_volume *Volumes;
};

struct world_entity
{
    u32 ID;
    entity_type Type;
    u32 Flags;
    
    v3 Position;
    float GroundZ;
    v3 Velocity;
    v2 Dimensions;
    animation_state AnimationState;
    animation_set AnimationSet;
    
    asset_id Texture;
    asset_id ShadowTexture;
    
    u32 TextureNumTilesX;
    u32 TextureNumTilesY;
    v2 TextureOrigin;
    
    entity_collision_volume_group *Collision;
//    v3 CollisionHalfDims;
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

    //Monster && Player
    float Hp;
    
    u32 UpdateID;
    
    bool32 IsPresent;

    //NOTE(zoubir): Tiled Object    
    u32 NumTilesX;
    u32 NumTilesY;
    u32 NumTilesZ;
    // 6 * 6
    u32 TileIndices[36];
};

#endif
