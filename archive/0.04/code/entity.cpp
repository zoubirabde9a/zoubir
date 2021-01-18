/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */
#include "entity.h"

inline bool32
IsSet(world_entity *Entity, u32 Flag)
{
    bool32 Result = Entity->Flags & Flag;
    return Result;
}

inline void
AddFlags(world_entity *Entity, u32 Flags)
{
    Entity->Flags |= Flags;
}

inline void
ClearFlag(world_entity *Entity, u32 Flag)
{
    Entity->Flags &= ~Flag;
}

entity_collision_volume_group *
MakeSimpleGroundedCollisionVolume(memory_arena *Arena, v3 HalfDims)
{
    entity_collision_volume_group *Group =
        AllocateStruct(Arena, entity_collision_volume_group);
    Group->VolumesCount = 1;
    Group->Volumes =
        AllocateArray(Arena, 1, entity_collision_volume);
    Group->Volumes[0].Offset.Z = HalfDims.Z;
    Group->Volumes[0].HalfDims = HalfDims;
    Group->TotalVolume = Group->Volumes[0];
    return Group;
}

inline entity_collision_volume
GetTotalVolume(entity_collision_volume *Volumes,
               u32 VolumesCount)
{
    Assert(VolumesCount > 0);
    entity_collision_volume TotalVolume = Volumes[0];

    for(u32 VolumeIndex = 0;
        VolumeIndex < VolumesCount;
        VolumeIndex++)
    {
        entity_collision_volume *Volume =
            &Volumes[VolumeIndex];
        v3 Min = Minimum3(TotalVolume.Offset, Volume->Offset);
        v3 Max = Maximum3(TotalVolume.Offset + TotalVolume.HalfDims,
                Volume->Offset + Volume->HalfDims);
        TotalVolume.Offset = Min;
        TotalVolume.HalfDims = Max - Min;
    }
    
    return TotalVolume;
}

entity_collision_volume_group *
MakeGroundedTreeCollisionVolume(memory_arena *Arena)
{
    entity_collision_volume_group *Group =
        AllocateStruct(Arena, entity_collision_volume_group);

    float LowestHalfZ = 63;
    
    Group->VolumesCount = 2;
    Group->Volumes =
        AllocateArray(Arena, Group->VolumesCount, entity_collision_volume);
    Group->Volumes[0].HalfDims = {18, 10, 63};
    Group->Volumes[0].Offset.Z += LowestHalfZ;
    
    Group->Volumes[1].HalfDims = {43, 10, 28};
    Group->Volumes[1].Offset.Z = 15;
    Group->Volumes[1].Offset.Z += LowestHalfZ;
    //TODO(zoubir): this is not the total volume
    // make a function that returns one
    Group->TotalVolume = GetTotalVolume(Group->Volumes,
                                        Group->VolumesCount);
    return Group;
}
