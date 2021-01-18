/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */
#if APP_DEV
#include "windows.h"
#endif

#include "app.h"
#include "stdio.h"
#include "string.h"

#include "random.cpp"
#include "utility.cpp"

#include "assets.cpp"
#include "render.cpp"
#include "ui.cpp"
#include "entity.cpp"
#include "world.cpp"

#include "app_ui.h"


#if 0
void PrintTest(render_context *RenderContext, app_memory *Memory,
               float X, float Y, char *text)
{
   // assume orthographic projection with units = screen pixels, origin at top left
   while (*text) {
      if (*text >= 32 && *text < 128) {
         stbtt_aligned_quad q;
         stbtt_GetBakedQuad((stbtt_bakedchar *)Memory->Font->Glyphs,
                            Memory->FontBitmapWidth,
                            Memory->FontBitmapHeight,
                             *text-32, &X,&Y,&q,1);//1=opengl & d3d10+,0=d3d9
         float QuadWidth = q.x1 - q.x0;
         float QuadHeight = q.y1 - q.y0;
         float UX = q.s0;
         float UY = q.t0;
         float TW = q.s1;
         float TH = q.t1;
         float ImageWidth = 512.f;
         float ImageHeight = 512.f;
         float QuadX = ImageWidth * UX;
         float QuadY = ImageHeight * UY;
         QuadWidth = (q.s1 - q.s0) * 512;
         QuadHeight = (q.t1 - q.t0) * 512;
         #if 0
         RenderQuadTexture(RenderContext, QuadX, QuadY,
                           QuadWidth, QuadHeight,
                           0.f, 0.f, 1.f, 1.f, RGBA8_WHITE, 1.f);
         #endif
         #if 1
         RenderQuadTexture(RenderContext, q.x0, q.y0,
                           QuadWidth, QuadHeight,
                           UX, UY, TW, TH, RGBA8_WHITE, 1.f);
         #endif
      }
      ++text;
   }
}
#endif
// old code 
#if 0
internal void *
Allocate(thread_context *Thread, app_memory *Memory, u32 Bytes)
{
    // TODO(zoubir): add a case where more memory is needed
    // if must be
    void *Result = 0;
    // look for a free block
    app_memory_block_header **FreeBlocks = Memory->PermanentStorageFreeBlocks;
    for(u32 CurrentFreeBlockIndex = 0;
        CurrentFreeBlockIndex < Memory->PermanentStorageFreeBlocksCount;
        CurrentFreeBlockIndex++)
    {
        app_memory_block_header *ThisFreeBlock = FreeBlocks[CurrentFreeBlockIndex];
        if (ThisFreeBlock->Size >= Bytes)
        {
            // Allocate This Block
            Result = ThisFreeBlock + 1;
            u32 RemainingBytes = ThisFreeBlock->Size - Bytes;
            if (RemainingBytes > sizeof(app_memory_block_header))
            {
                // Reduce Remaining Block size
                app_memory_block_header *NewFreeBlock =
                    (app_memory_block_header *)(RemainingBytes - sizeof(app_memory_block_header));
                
                NewFreeBlock->Size = ThisFreeBlock->Size
                    - Bytes - sizeof(app_memory_block_header);
                NewFreeBlock->FreeBlockIndex = CurrentFreeBlockIndex;
                NewFreeBlock->Previous = ThisFreeBlock;
                
                ThisFreeBlock->Size = Bytes;
                ThisFreeBlock->FreeBlockIndex = 0;

                return Result;
            }
            else
            {
                // Take the block and remove the header from the freeblocks array
                for(int FreeBlockIndex = CurrentFreeBlockIndex;
                    FreeBlockIndex >= 0;
                    FreeBlockIndex--)
                {
                    
                }
                // adjust the FreeBlockIndex for the remaining
                // Free Blocks 
                for(int FreeBlockIndex = CurrentFreeBlockIndex + 1;
                    FreeBlockIndex < Memory->PermanentStorageFreeBlocksCount;
                    FreeBlockIndex++)
                {
                    
                }
                
                return Result;
            }
        }
    }
#if 0    
    // see if we have enough memory left through nextBlockHeader
    void *NextFreeBlockHeader = Memory->PermanentStorageNextFreeBlockHeader;
    if (NextFreeBlockHeader && NextFreeBlockHeader->Size >= Bytes)
    {
        // use the current pointer to allocate
    }
    else
    {
        
    }

    
    for(u32 CurrentFreeBlockIndex = 0;
        CurrentFreeBlockIndex < Memory->PermanentStorageFreeBlocksCount;
        CurrentFreeBlockIndex++)
    {
        app_memory_free_block *ThisFreeBlock =
            &Memory->PermanentStorageFreeBlocks[CurrentFreeBlockIndex];
        if (ThisFreeBlock->Size > Bytes)
        {
            // take free block, reduce size and offset Ptr
            Result = ThisFreeBlock->Ptr;

            ThisFreeBlock->Ptr = (void *)((size_t)ThisFreeBlock->Ptr + (size_t)Bytes);
            ThisFreeBlock->Size = ThisFreeBlock->Size - Bytes;

            return Result;
        }
        else if (ThisFreeBlock->Size == Bytes)
        {
            // take this free block, and remove element
            Result = ThisFreeBlock->Ptr;
            
            // move all element By 1 Position
            for(int FreeBlockIndex = CurrentFreeBlockIndex;
                FreeBlockIndex > 0;
                FreeBlockIndex--)
            {
                Memory->PermanentStorageFreeBlocks[FreeBlockIndex] =
                    Memory->PermanentStorageFreeBlocks[FreeBlockIndex - 1];
            }
            Memory->PermanentStorageFreeBlocks++;
            Memory->PermanentStorageFreeBlocksCount--;

            return Result;
        }
    }
#endif
    return Result;
}

internal void
Free(thread_context *Thread, app_memory *Memory, void *Ptr)
{
    
}
#endif

internal void
PlaySound(app_state *AppState, loaded_audio *Audio)
{
    Assert(AppState->PlayingSoundsCount < ArrayCount(AppState->PlayingSounds));
    playing_sound *NewSound =
        &AppState->PlayingSounds[AppState->PlayingSoundsCount++];
    NewSound->Audio = Audio;
    NewSound->Volume[0] = 1.f;
    NewSound->Volume[1] = 1.f;
    NewSound->SamplesPlayed = 0;
}

internal void
AppOutputSound(app_state *AppState,
                app_sound_output_buffer *SoundBuffer)
{
    loaded_audio *ZoubirAudio = &AppState->Assets.ZoubirAudio;
    local_persist i16 *Data = ZoubirAudio->Data;
    local_persist u32 NumSamples = ZoubirAudio->SampleCount;
    local_persist u32 Offset = 0;
    
    i16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        i16 SampleValue = Data[Offset];
        *SampleOut++ = SampleValue;
//        Offset = (Offset + 1) % NumSamples;
//        SampleValue = Data[Offset];
        *SampleOut++ = SampleValue;
        Offset = (Offset + 1) % NumSamples;
    }
}


internal bool32
CompileShader(open_gl *OpenGL,
              memory_arena *Arena,
              void* source, size_t size, char* name, u32 id)
{

    GLint sizeGL = (GLint)size;
    GLchar *sourceGL = (GLchar *)source;
    //tell opengl that we want to use fileContents as the contents of the shader file
    OpenGL->glShaderSource(id, 1, &sourceGL, &sizeGL);
    //compile the shader
    OpenGL->glCompileShader(id);
    //check for errors
    i32 success = 0;
    OpenGL->glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        i32 maxLength = 0;
        OpenGL->glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        temporary_memory TempMem = BeginTemporaryMemory(Arena);
        char* errorLog = AllocateArray(Arena, (u32)maxLength, char);
        OpenGL->glGetShaderInfoLog(id, maxLength, &maxLength, errorLog);

        
        //TODO(zoubir): Add logging
        EndTemporaryMemory(TempMem);
        Assert(0);

        OpenGL->glDeleteShader(id); //Don't leak the shader.
    }
    return success == GL_FALSE;
}

internal void
InitProgram(open_gl *OpenGL, memory_arena *Arena,
            render_program *Program, char *VertexShaderFileName,
            char *FragmentShaderFileName, char **Attributes, u32 NumAttrib)
{
    Program->ID = OpenGL->glCreateProgram();
    u32 VertexShaderID =
        OpenGL->glCreateShader(GL_VERTEX_SHADER);
    if (VertexShaderID == 0) {
        // Error Creating VertexShader
        // TODO(zoubir): Add Logging
        Assert(0);
    }

    u32 FragmentShaderID =
        OpenGL->glCreateShader(GL_FRAGMENT_SHADER);
    if (FragmentShaderID == 0) {
        // Error Creating FragmentShader
        // TODO(zoubir): Add Logging
        Assert(0);
    }
    debug_read_file_result VertexFileResult =
        DEBUGPlatformReadEntireFile(VertexShaderFileName);
    Assert(VertexFileResult.Memory);
    debug_read_file_result FragmentFileResult =
        DEBUGPlatformReadEntireFile(FragmentShaderFileName);
    Assert(FragmentFileResult.Memory);

    // Compile Shaders
    CompileShader(OpenGL, Arena, VertexFileResult.Memory,
                  VertexFileResult.Size, "Vertex Shader", VertexShaderID);
    CompileShader(OpenGL, Arena, FragmentFileResult.Memory,
                  FragmentFileResult.Size, "Fragment Shader", FragmentShaderID);

    Program->NumAttrib = NumAttrib;
    for(u32 CurrentAttribute = 0;
        CurrentAttribute < NumAttrib;
        CurrentAttribute++)
    {
        OpenGL->glBindAttribLocation(Program->ID, Program->NumAttrib,
                             Attributes[CurrentAttribute]);
    }

    //Attach shaders to program
    OpenGL->glAttachShader(Program->ID, VertexShaderID);
    OpenGL->glAttachShader(Program->ID, FragmentShaderID);
        
    //Link program
    OpenGL->glLinkProgram(Program->ID);
        
    //Note the different functions here: glGetProgram* instead of glGetShader*.
    int isLinked = 0;
    OpenGL->glGetProgramiv(Program->ID, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        int32_t maxLength = 0;
        OpenGL->glGetProgramiv(Program->ID, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        temporary_memory TempMem = BeginTemporaryMemory(Arena);
        char* errorLog =
            AllocateArray(Arena, (u32)maxLength, char);
        OpenGL->glGetProgramInfoLog(Program->ID, maxLength, &maxLength, errorLog);

        // TODO(zoubir): Add Logging
        EndTemporaryMemory(TempMem);
        Assert(0);
        //We don't need the program anymore.
        OpenGL->glDeleteProgram(Program->ID);
        //Don't leak shaders either.
        OpenGL->glDeleteShader(VertexShaderID);
        OpenGL->glDeleteShader(FragmentShaderID);
    }

    //Always detach shaders after a successful link.
    OpenGL->glDetachShader(Program->ID, VertexShaderID);
    OpenGL->glDetachShader(Program->ID, FragmentShaderID);
    OpenGL->glDeleteShader(VertexShaderID);
    OpenGL->glDeleteShader(FragmentShaderID);
}

internal void
AppInitOpenGL(memory_arena *TransientArena, app_state *AppState,
              thread_context *Thread, app_memory *Memory)
{
    render_context* RenderContext = &Thread->RenderContext;
    open_gl *OpenGL = RenderContext->OpenGL;
    // generate and bind VBO & VAO
    OpenGL->glGenVertexArrays(1, &RenderContext->VAO);    
    OpenGL->glBindVertexArray(RenderContext->VAO);
    OpenGL->glGenBuffers(1, &RenderContext->VBO);
    OpenGL->glBindBuffer(GL_ARRAY_BUFFER, RenderContext->VBO);

    // enable 3 attributes
    OpenGL->glEnableVertexAttribArray(0);
    OpenGL->glEnableVertexAttribArray(1);
    OpenGL->glEnableVertexAttribArray(2);

    //This is the position attribute pointer
    OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(render_vertex),
                          (void*)OffsetOf(render_vertex, X));
    //This is the color attribute pointer
    OpenGL->glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(render_vertex),
                          (void*)OffsetOf(render_vertex, R));
    //This is the UV attribute pointer
    OpenGL->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(render_vertex),
                          (void*)OffsetOf(render_vertex, U));

    OpenGL->glBindVertexArray(0);
    render_program *TextureProgram = &RenderContext->TextureProgram;
    render_program *LineProgram = &RenderContext->LineProgram;

    char *TextureProgramAttributes[] =
    {
        "VertexPosition",
        "VertexColor",
        "VertexUV",
    };
    char *LineProgramAttributes[] =
    {
        "VertexPosition",
        "VertexColor"
    };
    
    InitProgram(OpenGL, TransientArena, TextureProgram,
                "texture_shading.vert", "texture_shading.frag",
                TextureProgramAttributes,
                ArrayCount(TextureProgramAttributes));
    InitProgram(OpenGL, TransientArena, LineProgram,
                "line_shading.vert", "line_shading.frag",
                LineProgramAttributes,
                ArrayCount(LineProgramAttributes));
    
#if 0    
    u32 TextureProgram = RenderContext->TextureProgram;
    OpenGL->glUseProgram(TextureProgram);
    OpenGL->glEnableVertexAttribArray(0);
    OpenGL->glEnableVertexAttribArray(1);
    OpenGL->glEnableVertexAttribArray(2);
        
    GLint textureUniform = OpenGL->glGetUniformLocation(TextureProgram, "textureSampler");
    OpenGL->glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
#endif
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

inline bool32
GetCollisionValue(world *World, u32 TileX, u32 TileY)
{
    Assert(TileX < World->NumCollisionX);
    Assert(TileY < World->NumCollisionY);
    
    u32 Index = TileX + TileY * World->NumCollisionX;
    bool32 Result = World->CollisionMap[Index];
    return Result;
}

inline bool32
IsPositionColliding(world *World, cannonical_position Position)
{
    bool32 Result = GetCollisionValue(World, Position.TileX,
                                      Position.TileY);
    return Result;
}

internal void
ClearCollisionRulesFor(app_state *AppState, u32 Entity)
{
    for(u32 BucketIndex = 0;
        BucketIndex < ArrayCount(AppState->CollisionRuleHash);
        BucketIndex++)
    {
        for(pairwise_collision_rule **Rule = &AppState->CollisionRuleHash[BucketIndex];
            *Rule;
            )
        {
            if ((*Rule)->EntityA == Entity ||
                (*Rule)->EntityB == Entity)
            {
                pairwise_collision_rule *RemovedRule = *Rule;                
                *Rule = (*Rule)->Next;
                RemovedRule->Next = AppState->FirstFreeCollisionRule;
                AppState->FirstFreeCollisionRule =
                    RemovedRule;
            }
            else
            {
                Rule = &((*Rule)->Next);
            }
        }
    }
}

internal void
AddCollisionRule(app_state *AppState, memory_arena *Arena,
                 u32 EntityA, u32 EntityB,
                 bool32 Tag)
{   
    if (EntityA > EntityB)
    {
        u32 Tmp = EntityA;
        EntityA = EntityB;
        EntityB = Tmp;
    }
    
    // Hash on Entity A
    // TODO(zoubir): Better Hash Function 02
    u32 HashValue =
        EntityA & (ArrayCount(AppState->CollisionRuleHash) - 1);

    
    pairwise_collision_rule *FoundRule = 0;
    for(pairwise_collision_rule *Rule = AppState->CollisionRuleHash[HashValue];
        Rule;
        Rule = Rule->Next)
    {
        if (Rule->EntityA == EntityA &&
            Rule->EntityB == EntityB)
        {
            FoundRule = Rule;
            break;
        }
    }

    if (FoundRule)
    {
        FoundRule->Tag = Tag;
    }
    else
    {
        pairwise_collision_rule *NewRule;
        
        if (AppState->FirstFreeCollisionRule)
        {
            NewRule = AppState->FirstFreeCollisionRule;
            AppState->FirstFreeCollisionRule =
                AppState->FirstFreeCollisionRule->Next;
        }
        else
        {
            NewRule = AllocateStruct(Arena, pairwise_collision_rule);
        }
        
        NewRule->EntityA = EntityA;
        NewRule->EntityB = EntityB;
        NewRule->Tag = Tag;
        NewRule->Next = AppState->CollisionRuleHash[HashValue];
        AppState->CollisionRuleHash[HashValue] = NewRule;
    }
}

internal bool32
CanCollide(app_state *AppState, world_entity *A,
              world_entity *B)
{
    bool32 Result = false;

    if(A != B)
    {
        if(A->ID > B->ID)
        {
            world_entity *Tmp = A;
            A = B;
            B = Tmp;
        }

        // TODO(zoubir): Property-based logic goes here
        Result = true;
            
        // TODO(zoubir): Better Hash Function 02        
        u32 HashBucket = A->ID & (ArrayCount(AppState->CollisionRuleHash) - 1);
        for(pairwise_collision_rule *Rule = AppState->CollisionRuleHash[HashBucket];
            Rule;
            Rule = Rule->Next)
        {
            if((Rule->EntityA == A->ID) &&
               (Rule->EntityB == B->ID))
            {
                Result = Rule->Tag;
                break;
            }
        }
    }
    
    return(Result);
}

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

internal bool32
HandleCollision(world *World,
                world_entity *A,
                world_entity *B)
{
    bool32 Result = true;

    if (A->Type == EntityType_FireBall &&
        B->Type == EntityType_Monster)
    {
        B->Hp -= 25;
        if (B->Hp <= 0.f)
        {
            RemoveEntity(World, B);
        }
        Result = false;
    }
    
    if (B->Type == EntityType_FireBall &&
        A->Type == EntityType_Monster)
    {
        A->Hp -= 25;
        if (A->Hp <= 0.f)
        {
            RemoveEntity(World, B);            
        }
        Result = false;        
    }    
    
    return Result;
}

struct test_wall
{
    float X;
    v3 Rel;
    v3 Delta;
    float MinY;
    float MaxY;
    float MinZ;
    float MaxZ;
    v3 Normal;
};

internal bool32
TestWall(float WallX, float RelX, float RelY, float RelZ,
         float PlayerDeltaX, float PlayerDeltaY, float PlayerDeltaZ,
         float *tMin,
         float MinY, float MaxY,
         float MinZ, float MaxZ)
{
    bool32 Hit = false;
    float tEpsilon = 0.01f;
    if(PlayerDeltaX != 0.0f)
    {
        float tResult = (WallX - RelX) / PlayerDeltaX;
        float Y = RelY + tResult*PlayerDeltaY;
        float Z = RelZ + tResult*PlayerDeltaZ;
        if((tResult >= 0.0f) && (*tMin > tResult))
        {
            if((Y >= MinY) && (Y <= MaxY) &&
               (Z >= MinZ) && (Z <= MaxZ))
            {
                *tMin = Maximum(0.0f, tResult - tEpsilon);
                Hit = true;
            }
        }
    }
    
    return Hit;
}

internal bool32
CanOverlap(world_entity *Entity, world_entity *Region)
{
    bool32 Result = false;
    if (Entity->Type == EntityType_Player &&
        Region->Type == EntityType_Familiar)
    {
        Result = true;
    }
    
    return Result;
}

internal bool32
EntityOverlap(world_entity *Entity, world_entity *Region)
{
    bool32 Result = false;
    
    for(u32 VolumeIndex = 0;
        VolumeIndex < Entity->Collision->VolumesCount;
        VolumeIndex++)
    {
        entity_collision_volume *Volume =
            &Entity->Collision->Volumes[VolumeIndex];
        
        rectangle3 EntityRect =
            RectCenterHalfDims(Entity->Position +
                               Volume->Offset, Volume->HalfDims);
                                                                     
        for(u32 RegionVolumeIndex = 0;
            RegionVolumeIndex < Region->Collision->VolumesCount;
            RegionVolumeIndex++)
        {
            entity_collision_volume *RegionVolume =
                &Region->Collision->Volumes[RegionVolumeIndex];
            rectangle3 RegionRect =
                RectCenterHalfDims(Region->Position +
                                   RegionVolume->Offset, RegionVolume->HalfDims);
            if (RectanglesIntersect(EntityRect, RegionRect))
            {
                Result = true;
            }
        }
    }
    return Result;
}

internal void
HandleOverlap(app_state *AppState, world_entity *Entity, world_entity *Region)
{
//    PlaySound(AppState, &AppState->Assets.ZoubirAudio);
}

internal void
MoveEntity(world_entity *Entity, world *World,
           memory_arena *Arena,
           app_input *Input, app_state *AppState,
           v3 DDEntity, float *MaxDistance)
{
    float FrameDelta = 1.f / 60.f;
    float AllDeltaTime = Input->DeltaTime;
    while (AllDeltaTime > 0.f)
    {
        float DeltaTime;
        if (AllDeltaTime >= FrameDelta)
        {
            DeltaTime = FrameDelta;
        }
        else
        {
            DeltaTime = AllDeltaTime;
        }
        AllDeltaTime -=DeltaTime;
    // NOTE(zoubir): jumping code
    
//    DDEntityZ -= (10.f * Entity->VelocityZ);
    
    // NOTE(zoubir): New Position
    v3 EntityDelta = 0.5f * DDEntity * Square(DeltaTime) +
        Entity->Velocity * DeltaTime;
    
    Entity->Velocity = DDEntity * DeltaTime +
        Entity->Velocity;

    int NumIterations = 4;
    for(int Iteration = 0;
        Iteration < NumIterations;
        Iteration++)
    {
        //TODO(zoubir): optimise this shit show
        
        v3 EntityOldPosition = Entity->Position;
        v3 EntityNewPosition = EntityOldPosition + EntityDelta;
        EntityNewPosition.Z = Maximum(0.f, EntityNewPosition.Z);

        float Distance = Length(EntityDelta);
        float EpsilonDistance = 0.000001f;
        if (Distance <= EpsilonDistance)
        {
            // Nothing to do here
            break;
        }

        if (Distance > *MaxDistance)
        {
            
            float Ratio = *MaxDistance / Distance;
            EntityDelta = Ratio * EntityDelta;
            *MaxDistance = 0.f;
        }
        else
        {
            *MaxDistance -= Distance;
        }

        u32 CollisionTileWidth = World->CollisionWidth;
        u32 CollisionTileHeight = World->CollisionHeight;
        u32 CollisionTileDepth = World->CollisionDepth;
    
        cannonical_position NewCollisionP =
            CannonicalizePosition(EntityNewPosition,
                                  World->CollisionWidth,
                                  World->CollisionHeight,
                                  World->CollisionDepth);
        cannonical_position OldCollisionP =
            CannonicalizePosition(EntityOldPosition,
                                  World->CollisionWidth,
                                  World->CollisionHeight,
                                  World->CollisionDepth);
    
    
        entity_collision_volume *EntityCollisionTotal =
            &Entity->Collision->TotalVolume;
        
        u32 MinChunkX;
        u32 MinChunkY;
        u32 MinChunkZ;
        u32 MaxChunkX;
        u32 MaxChunkY;
        u32 MaxChunkZ;        
        
        v3 MinPos = Minimum3(EntityOldPosition, EntityNewPosition);
        v3 MaxPos = Maximum3(EntityOldPosition, EntityNewPosition);
        MinPos += EntityCollisionTotal->Offset;
        MaxPos += EntityCollisionTotal->Offset;
        
        MinPos -= EntityCollisionTotal->HalfDims;
        MaxPos += EntityCollisionTotal->HalfDims;        
        rectangle3 Box = RectMinMax(MinPos,
                                    MaxPos);
        GetChunksFromBox(World, Box,
                               &MinChunkX, &MinChunkY, &MinChunkZ,
                               &MaxChunkX, &MaxChunkY, &MaxChunkZ);

        #if 0
        u32 MinTileX = Minimum(OldCollisionP.TileX,
                               NewCollisionP.TileX);
        u32 MinTileY = Minimum(OldCollisionP.TileY,
                               NewCollisionP.TileY);
        u32 MinTileZ = Minimum(OldCollisionP.TileZ,
                               NewCollisionP.TileZ);
        
        u32 MaxTileX = Maximum(OldCollisionP.TileX,
                               NewCollisionP.TileX);
        u32 MaxTileY = Maximum(OldCollisionP.TileY,
                               NewCollisionP.TileY);
        u32 MaxTileZ = Maximum(OldCollisionP.TileZ,
                               NewCollisionP.TileZ);
        
        u32 EntityTileWidth = CeilFloatToUInt32(Entity->CollisionHalfDims.X /
                                                CollisionTileWidth);
        u32 EntityTileHeight = CeilFloatToUInt32(Entity->CollisionHalfDims.Y /
                                                 CollisionTileHeight);    
        u32 EntityTileDepth = CeilFloatToUInt32(Entity->CollisionHalfDims.Z /
                                                 CollisionTileDepth);
        MinTileX -= EntityTileWidth;
        MinTileY -= EntityTileHeight;
        MinTileZ -= EntityTileDepth;
        
        MaxTileX += EntityTileWidth;
        MaxTileY += EntityTileHeight;
        MaxTileZ += EntityTileDepth;
#endif
        world_entity *CollidedEntity = 0;
        v3 WallNormal = {};
        float tMin = 1.f;

        //TODO(zoubir): make this spot collision
        // with different objects
        // and do something about it
        for(u32 ChunkY = MinChunkY;
            ChunkY <= MaxChunkY;
            ChunkY++)
        {
            for(u32 ChunkX = MinChunkX;
                ChunkX <= MaxChunkX;
                ChunkX++)
            {
                for(u32 ChunkZ = MinChunkZ;
                    ChunkZ <= MaxChunkZ;
                    ChunkZ++)
                {
                    world_chunk *ThisChunk = GetChunk(World, ChunkX,
                                                      ChunkY, ChunkZ);
                    for (world_entity_chunk *EntityChunk = &ThisChunk->FirstEntityChunk;
                         EntityChunk;
                         EntityChunk = EntityChunk->Next)
                    {
                        for(u32 EntityIndex = 0;
                            EntityIndex < EntityChunk->EntityCount;
                            EntityIndex++)
                        {
                            world_entity *ThisEntity = EntityChunk->Entities[EntityIndex];
                            if ((ThisEntity != Entity) &&
                                CanCollide(AppState, Entity, ThisEntity) &&
                                CanCollide(AppState, Entity->Type,
                                       ThisEntity->Type))
                            {
                                
                                for(u32 VolumeIndex = 0;
                                    VolumeIndex < Entity->Collision->VolumesCount;
                                    VolumeIndex++)
                                {
                                    entity_collision_volume *Volume =
                                        Entity->Collision->Volumes +
                                        VolumeIndex;
                                    for(u32 TestVolumeIndex = 0;
                                        TestVolumeIndex < ThisEntity->Collision->VolumesCount;
                                        TestVolumeIndex++)
                                    {
                                        entity_collision_volume *TestVolume =
                                            ThisEntity->Collision->Volumes +
                                            TestVolumeIndex;
                                        v3 MinkowskiDiameter = Volume->HalfDims
                                            + TestVolume->HalfDims;
                                        v3 MinCorner = -MinkowskiDiameter;
                                        v3 MaxCorner = MinkowskiDiameter;
                                        
                                        v3 Rel = (EntityOldPosition + Volume->Offset) -
                                            (ThisEntity->Position + TestVolume->Offset);

                                        test_wall Walls[] =
                                        {
                                            {MinCorner.X, Rel.X, Rel.Y, Rel.Z, EntityDelta.X, EntityDelta.Y, EntityDelta.Z, MinCorner.Y, MaxCorner.Y, MinCorner.Z, MaxCorner.Z, {-1, 0, 0}},
                                            {MaxCorner.X, Rel.X, Rel.Y, Rel.Z, EntityDelta.X, EntityDelta.Y, EntityDelta.Z, MinCorner.Y, MaxCorner.Y, MinCorner.Z, MaxCorner.Z, {1, 0, 0}},
                                            {MinCorner.Y, Rel.Y, Rel.X, Rel.Z, EntityDelta.Y, EntityDelta.X, EntityDelta.Z, MinCorner.X, MaxCorner.X, MinCorner.Z, MaxCorner.Z, {0, 1, 0}},
                                            {MaxCorner.Y, Rel.Y, Rel.X, Rel.Z, EntityDelta.Y, EntityDelta.X, EntityDelta.Z, MinCorner.X, MaxCorner.X, MinCorner.Z, MaxCorner.Z, {0, -1, 0}},
                                            {MinCorner.Z, Rel.Z, Rel.Y, Rel.X, EntityDelta.Z, EntityDelta.Y, EntityDelta.X, MinCorner.Y, MaxCorner.Y, MinCorner.X, MaxCorner.X, {0, 0, -1}},
                                            {MaxCorner.Z, Rel.Z, Rel.Y, Rel.X, EntityDelta.Z, EntityDelta.Y, EntityDelta.X, MinCorner.Y, MaxCorner.Y, MinCorner.X, MaxCorner.X, {0, 0, 1}},
                                            {MinCorner.Z, Rel.Z, Rel.X, Rel.Y, EntityDelta.Z, EntityDelta.X, EntityDelta.Y, MinCorner.X, MaxCorner.X, MinCorner.Y, MaxCorner.Y, {0, 0, 1}},
                                            {MaxCorner.Z, Rel.Z, Rel.X, Rel.Y, EntityDelta.Z, EntityDelta.X, EntityDelta.Y, MinCorner.X, MaxCorner.X, MinCorner.Y, MaxCorner.Y, {0, 0, -1}}
                                        };
                                        
                                        for(u32 WallIndex = 0;
                                            WallIndex < ArrayCount(Walls);
                                            WallIndex++)
                                        {
                                            test_wall *Wall = &Walls[WallIndex];
                                            if (TestWall(Wall->X,
                                                         Wall->Rel.X, Wall->Rel.Y, Wall->Rel.Z,
                                                         Wall->Delta.X, Wall->Delta.Y, Wall->Delta.Z,
                                                         &tMin,
                                                         Wall->MinY, Wall->MaxY,
                                                         Wall->MinZ, Wall->MaxZ))
                                            {
                                                WallNormal = Wall->Normal;
                                                CollidedEntity = ThisEntity;                                                
                                            }
                                        }
                                    }                    
                                }                    
                            }
                        }
                    }
                }
            }
        }

        v3 AllowedDelta = tMin * EntityDelta;
        Entity->Position = EntityOldPosition + AllowedDelta;
        
        // NOTE(zoubir): Cannot Go Lower Than The Ground Level
        if (Entity->Position.Z < 0.f)
        {
            Entity->Position.Z = 0.f;
            Entity->Velocity.Z = 0.f;
        }

        // TODO(zoubir): Do This Once Per MoveEntityCall RatherThan
        // Every Iteration        
        CheckAndChangeEntityChunk(World, Arena, EntityOldPosition, Entity);

        if (CollidedEntity)
        {
            bool32 StopsOnCollision =
                HandleCollision(World, Entity,
                                CollidedEntity);
            if (StopsOnCollision)
            {
                Entity->Velocity = Entity->Velocity - 1.f * DotProduct(Entity->Velocity, WallNormal) * WallNormal;
                v3 DeltaLeft = EntityDelta - AllowedDelta;
                EntityDelta = DeltaLeft - 1.f * DotProduct(DeltaLeft, WallNormal) * WallNormal;
            }
            else
            {
                AddCollisionRule(AppState, Arena,
                                 Entity->ID, CollidedEntity->ID,
                                 false);
            }
        }
        else
        {
            break;
        }
    }

    // NOTE(zoubir): New Velocity

    
    // Gravity / friction
//    AppState->DEntity *= 1.f;
    //TODO(zoubir): fix this false velocity
#if 0
    v2 MaxVelocity = {300, 300};
    if (AppState->DEntity.X > MaxVelocity.X)
    {
        AppState->DEntity.X = MaxVelocity.X;
    }
    if (AppState->DEntity.X < -MaxVelocity.X)
    {
        AppState->DEntity.X = -MaxVelocity.X;
    }
    if (AppState->DEntity.Y > MaxVelocity.Y)
    {
        AppState->DEntity.Y = MaxVelocity.Y;
    }
    if (AppState->DEntity.Y < -MaxVelocity.Y)
    {
        AppState->DEntity.Y = -MaxVelocity.Y;
    }
#endif
    }

    // TODO(zoubir): Spatial partition here!
    for(u32 EntityIndex = 0;
        EntityIndex < World->EntityCount;
        EntityIndex++)
    {        
        world_entity *TestEntity = &World->Entities[EntityIndex];
        if (Entity != TestEntity &&
            CanOverlap(Entity, TestEntity) &&
            EntityOverlap(Entity, TestEntity))
        {
            HandleOverlap(AppState, Entity, TestEntity);
        }                    
    }

    float NearestDistance = 100000.f;
    entity_collision_volume *NearestVolume = 0;
    world_entity *NearestEntity = 0;
    // TODO(zoubir): Spatial partition here!
    for(u32 EntityIndex = 0;
        EntityIndex < World->EntityCount;
        EntityIndex++)
    {
        world_entity *TestEntity = &World->Entities[EntityIndex];
        if (TestEntity != Entity &&
            CanCollide(AppState, Entity->Type, TestEntity->Type) &&
            CanCollide(AppState, Entity, TestEntity))
        {
            
            for(u32 VolumeIndex = 0;
                VolumeIndex < Entity->Collision->VolumesCount;
                VolumeIndex++)
            {                
                entity_collision_volume *Volume =
                    &Entity->Collision->Volumes[VolumeIndex];
                
                rectangle2 EntityRect =
                    RectCenterHalfDims(Entity->Position.XY + Volume->Offset.XY,
                                       Volume->HalfDims.XY);
                for(u32 TestVolumeIndex = 0;
                    TestVolumeIndex < TestEntity->Collision->VolumesCount;
                    TestVolumeIndex++)
                {                    
                    entity_collision_volume *TestVolume =
                        &TestEntity->Collision->Volumes[TestVolumeIndex];
                    
                    rectangle2 TestEntityRect =
                        RectCenterHalfDims(TestEntity->Position.XY +
                                           TestVolume->Offset.XY,
                                           TestVolume->HalfDims.XY);
                    if (RectanglesIntersect(EntityRect, TestEntityRect))
                    {
                        float Distance =
                            (Entity->Position.Z -
                             (TestEntity->Position.Z +
                              TestVolume->Offset.Z +
                              TestVolume->HalfDims.Z));
                        Assert(Distance < 100000.f);
                        if (Distance > 0.f &&
                            Distance < NearestDistance)
                        {
                            NearestDistance = Distance;
                            NearestVolume = TestVolume;
                            NearestEntity = TestEntity;
                        }
                    }
                }
            }
        }
    }

    if (NearestVolume)
    {
        Entity->GroundZ = NearestEntity->Position.Z +
            NearestVolume->Offset.Z + NearestVolume->HalfDims.Z;        
    }
    else
    {
        Entity->GroundZ = 0.f;
    }
}
    

internal world_entity *
AddPlayer(app_state *AppState,
          world *World, memory_arena *Arena, v3 Position)
{
    app_assets *Assets = &AppState->Assets;
    world_entity *Player =
        AddEntity(World, Arena, Position,
                  AppState->PlayerCollision);
    
    Player->Type = EntityType_Player;
    Player->Position = Position;
    Player->Dimensions = {32, 48};
    Player->AnimationState = {};
    Player->Texture = AAI_Zoubir;
    Player->ShadowTexture = AAI_Shadow;
    Player->TextureNumTilesX = 4;
    Player->TextureNumTilesY = 4;
    Player->TextureOrigin = {16, 42};
    
    animation_set *PlayerAnimationSet =
        &Player->AnimationSet;
        
    animation_slot *MoveUpAnimation =
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_UP);
    MoveUpAnimation->FirstIndex = 0;
    MoveUpAnimation->IndicesCount = 4;
        
    animation_slot *MoveDownAnimation =
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_DOWN);
    MoveDownAnimation->FirstIndex = 12;
    MoveDownAnimation->IndicesCount = 4;
        
    animation_slot *MoveRightAnimation =
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_RIGHT);
    MoveRightAnimation->FirstIndex = 4;
    MoveRightAnimation->IndicesCount = 4;
        
    animation_slot *MoveLeftAnimation = 
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_LEFT);
    MoveLeftAnimation->FirstIndex = 8;
    MoveLeftAnimation->IndicesCount = 4;
        
    animation_slot *StandUpAnimation = 
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_UP);
    StandUpAnimation->FirstIndex = 0;
    StandUpAnimation->IndicesCount = 1;
        
    animation_slot *StandDownAnimation =
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_DOWN);
    StandDownAnimation->FirstIndex = 12;
    StandDownAnimation->IndicesCount = 1;
        
    animation_slot *StandRightAnimation =
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_RIGHT);
    StandRightAnimation->FirstIndex = 4;
    StandRightAnimation->IndicesCount = 1;
        
    animation_slot *StandLeftAnimation =
        GetAnimation(PlayerAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_LEFT);
    StandLeftAnimation->FirstIndex = 8;
    StandLeftAnimation->IndicesCount = 1;

    return Player;
}

internal world_entity *
AddWall(app_state *AppState,
          world *World, memory_arena *Arena, v3 Position)
{
    app_assets *Assets = &AppState->Assets;
    
    float WallWidth = (float)World->TileWidth;
    float WallHeight = (float)World->TileHeight;
    float WallDepth = (float)World->TileDepth;
    
    world_entity *Wall =
        AddEntity(World, Arena, Position,
                  AppState->WallCollision);
    
    Wall->Type = EntityType_StaticObject;
    Wall->Dimensions = {WallWidth, WallHeight};
    Wall->TextureNumTilesX = 1;
    Wall->TextureNumTilesY = 1;
    Wall->TextureOrigin = {0.5f*WallWidth, 0.5f*WallHeight};
//    Wall->Uvs = GetTextureUvsFromIndex(Wall->Texture->Width,
//                                       Wall->Texture->Height,
//                                       3, 4, 4);
    
    return Wall;
}

internal world_entity *
AddTree(app_state *AppState,
          world *World, memory_arena *Arena, v3 Position)
{
    app_assets *Assets = &AppState->Assets;
    
    float Width = 122.f;
    float Height = 159.f;
    
    world_entity *Entity =
        AddEntity(World, Arena, Position,
                  AppState->TreeCollision);

    Entity->Type = EntityType_StaticObject;
    Entity->Dimensions = {Width, Height};
    Entity->Texture = AAI_Tree;
    Entity->TextureNumTilesX = 1;
    Entity->TextureNumTilesY = 1;
    Entity->TextureOrigin = {61, 148};
    Entity->Uvs = {0.f, 0.f, 1.f, 1.f};
    
    return Entity;
}

internal world_entity *
AddTileEntity(app_state *AppState,
              world *World, memory_arena *Arena,
              u32 TileX, u32 TileY, u32 TileZ)
{
    app_assets *Assets = &AppState->Assets;

    u32 NumTilesX = 3;
    u32 NumTilesY = 3;
    u32 NumTilesZ = 2;

    v2 TextureDims = V2((NumTilesX * World->TileWidth),
                        ((NumTilesZ + NumTilesY) * World->TileHeight));

    v3 HalfDims = V3((World->TileWidth * NumTilesX / 2),
                     (World->TileHeight * NumTilesY / 2),
                     (World->TileDepth * NumTilesZ / 2));

    v3 Position = V3(TileX * World->TileWidth + HalfDims.X,
                     TileY * World->TileHeight + HalfDims.Y,
                     (float)(TileZ * World->TileWidth));
    
    world_entity *Entity =
        AddEntity(World, Arena, Position,
                  AppState->TileObjectCollision);

    Entity->Type = EntityType_Tiled;
    Entity->Dimensions = {TextureDims.X, TextureDims.Y};
    Entity->Texture = AAI_TileMap;
    //
    Entity->TextureOrigin =
        V2(HalfDims.X,
           World->TileHeight *
           (NumTilesY + NumTilesZ) * 0.5f +
           5);
    
    Entity->NumTilesX = NumTilesX;
    Entity->NumTilesY = NumTilesY;
    Entity->NumTilesZ = NumTilesZ;

    u32 TileMapOffsetX = 5;
    u32 TileMapOffsetY = 178;
    u32 TileMapNumTilesX = 8;
    for(u32 IndexY = 0;
        IndexY <  NumTilesY + NumTilesZ - 1;
        IndexY++)
    {
        u32 TileMapIndexY = TileMapOffsetY - IndexY;
        for(u32 IndexX = 0;
            IndexX < NumTilesX;
            IndexX++)
        {
            u32 TileMapIndexX = TileMapOffsetX + IndexX;
            u32 Index = TileMapIndexX +
                (TileMapIndexY * TileMapNumTilesX);
            Entity->TileIndices[IndexX + IndexY * NumTilesX] =
                Index;
        }
    }
    
    return Entity;
}

internal world_entity *
AddMonster(app_state *AppState,
          world *World, memory_arena *Arena, v3 Position)
{
    app_assets *Assets = &AppState->Assets;
    world_entity *Entity =
        AddEntity(World, Arena, Position, AppState->PlayerCollision);
    
    Entity->Type = EntityType_Monster;
    Entity->Dimensions = {32, 48};
    Entity->AnimationState = {};
    Entity->Texture = AAI_Zoubir;
    Entity->ShadowTexture = AAI_Shadow;
    Entity->TextureNumTilesX = 4;
    Entity->TextureNumTilesY = 4;
    Entity->TextureOrigin = {16, 42};
    Entity->Hp = 100.f;
    
    animation_set *EntityAnimationSet =
        &Entity->AnimationSet;
        
    animation_slot *MoveUpAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_UP);
    MoveUpAnimation->FirstIndex = 0;
    MoveUpAnimation->IndicesCount = 4;
        
    animation_slot *MoveDownAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_DOWN);
    MoveDownAnimation->FirstIndex = 12;
    MoveDownAnimation->IndicesCount = 4;
        
    animation_slot *MoveRightAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_RIGHT);
    MoveRightAnimation->FirstIndex = 4;
    MoveRightAnimation->IndicesCount = 4;
        
    animation_slot *MoveLeftAnimation = 
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_LEFT);
    MoveLeftAnimation->FirstIndex = 8;
    MoveLeftAnimation->IndicesCount = 4;
        
    animation_slot *StandUpAnimation = 
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_UP);
    StandUpAnimation->FirstIndex = 0;
    StandUpAnimation->IndicesCount = 1;
        
    animation_slot *StandDownAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_DOWN);
    StandDownAnimation->FirstIndex = 12;
    StandDownAnimation->IndicesCount = 1;
        
    animation_slot *StandRightAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_RIGHT);
    StandRightAnimation->FirstIndex = 4;
    StandRightAnimation->IndicesCount = 1;
        
    animation_slot *StandLeftAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_LEFT);
    StandLeftAnimation->FirstIndex = 8;
    StandLeftAnimation->IndicesCount = 1;

    return Entity;
}

internal world_entity *
AddFamiliar(app_state *AppState,
          world *World, memory_arena *Arena,
            world_entity *Player)
{
    v3 Position = Player->Position + v3{40, 40, 0};
    
    app_assets *Assets = &AppState->Assets;
    world_entity *Entity =
        AddEntity(World, Arena, Position,
                  AppState->FamiliarCollision);
    
    Entity->Type = EntityType_Familiar;
    Entity->Dimensions = {45 * 0.5f, 25 * 0.5f};
    Entity->AnimationState = {};
    Entity->Texture = AAI_Familiar;
    Entity->ShadowTexture = AAI_Shadow;
    Entity->TextureNumTilesX = 2;
    Entity->TextureNumTilesY = 1;
    Entity->TextureOrigin = {22 * 0.5f, 12 * 0.5f};
    Entity->FollowingEntity = Player;
    
    animation_set *EntityAnimationSet =
        &Entity->AnimationSet;
        
    animation_slot *MoveRightAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_RIGHT);
    MoveRightAnimation->FirstIndex = 0;
    MoveRightAnimation->IndicesCount = 1;
        
    animation_slot *MoveLeftAnimation = 
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_LEFT);
    MoveLeftAnimation->FirstIndex = 1;
    MoveLeftAnimation->IndicesCount = 1;
        
    animation_slot *StandRightAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_RIGHT);
    StandRightAnimation->FirstIndex = 0;
    StandRightAnimation->IndicesCount = 1;
        
    animation_slot *StandLeftAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_STAND,
                     ANIMATION_DIRECTION_LEFT);
    StandLeftAnimation->FirstIndex = 1;
    StandLeftAnimation->IndicesCount = 1;

    return Entity;
}

internal world_entity *
AddFireBall(app_state *AppState,
          world *World, memory_arena *Arena,
            world_entity *Owner, v3 CastPosition,
            v3 Velocity)
{
    app_assets *Assets = &AppState->Assets;

    float Width = 18.f;
    float Height = 18.f;
    
    world_entity *Entity =
        AddEntity(World, Arena, CastPosition,
                  AppState->FireBallCollision);
    
    Entity->Type = EntityType_FireBall;
    Entity->Velocity = Velocity;
    Entity->Dimensions = {Width, Height};
    Entity->AnimationState = {};
    Entity->Texture = AAI_FireBall;
    Entity->ShadowTexture = AAI_Shadow;
    Entity->TextureNumTilesX = 4;
    Entity->TextureNumTilesY = 4;
    Entity->TextureOrigin = {18 * 0.5f, 18 * 0.5f};
    Entity->DistanceRemaining = 300.f;
    
    animation_set *EntityAnimationSet =
        &Entity->AnimationSet;
        
    animation_slot *MoveRightAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_RIGHT);
    MoveRightAnimation->FirstIndex = 4;
    MoveRightAnimation->IndicesCount = 4;
        
    animation_slot *MoveLeftAnimation = 
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_LEFT);    
    MoveLeftAnimation->FirstIndex = 8;
    MoveLeftAnimation->IndicesCount = 4;
        
    animation_slot *MoveUpAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_UP);
    MoveUpAnimation->FirstIndex = 0;
    MoveUpAnimation->IndicesCount = 4;
        
    animation_slot *MoveDownAnimation =
        GetAnimation(EntityAnimationSet,
                     ANIMATION_TYPE_MOVE,
                     ANIMATION_DIRECTION_DOWN);
    MoveDownAnimation->FirstIndex = 12;
    MoveDownAnimation->IndicesCount = 4;
        
    return Entity;
}

internal void
UpdatePlayer(world_entity *Player, world *World,
             memory_arena *Arena,
             app_input *Input, app_state *AppState,
             float *AnimationSpeed,
             animation_type *AnimationType,
             animation_direction *AnimationDirection)
{
    float DeltaTime = Input->DeltaTime;
    v3 DDPlayer = {};
    float PlayerAcceleration = 56000.f;
    *AnimationType = ANIMATION_TYPE_STAND;
    *AnimationDirection =
        Player->AnimationSet.LastAnimationDirection;
    *AnimationSpeed = 1.f;
            
    if (Input->ButtonZ.EndedDown)
    {
        DDPlayer.Y = -1.f;
        *AnimationType = ANIMATION_TYPE_MOVE;
        *AnimationDirection = ANIMATION_DIRECTION_UP;
    }
    if (Input->ButtonS.EndedDown)
    {
        DDPlayer.Y = 1.f;
        *AnimationType = ANIMATION_TYPE_MOVE;
        *AnimationDirection = ANIMATION_DIRECTION_DOWN;
    }
    if (Input->ButtonD.EndedDown)
    {
        DDPlayer.X = 1.f;
        *AnimationType = ANIMATION_TYPE_MOVE;
        *AnimationDirection = ANIMATION_DIRECTION_RIGHT;
    }
    if (Input->ButtonQ.EndedDown)
    {
        DDPlayer.X = -1.f;
        *AnimationType = ANIMATION_TYPE_MOVE;
        *AnimationDirection = ANIMATION_DIRECTION_LEFT;
    }
    if (Input->SpaceButton.Pressed)
    {
        Player->Velocity.Z = 230.f;
        PlaySound(AppState, &AppState->Assets.ZoubirAudio);
    }
    
    if (Input->AltButton.Pressed)
    {
        PlayerAcceleration *= 10;
        PlaySound(AppState, &AppState->Assets.Dash);
    }

    float FireBallVelocity = 450.f;
    float HandOffset = 30;
    if (Input->ArrowRight.Pressed)
    {
        
        world_entity *Entity = AddFireBall(AppState, World, Arena,
                                           Player, Player->Position + v3{32.f, 0.f, HandOffset},
                                           v3{FireBallVelocity, 0.f, 0.f});
        Entity->AnimationSpeed = 0.15f;
        Entity->AnimationType = ANIMATION_TYPE_MOVE;
        Entity->AnimationDirection = ANIMATION_DIRECTION_RIGHT;
        PlaySound(AppState, &AppState->Assets.FireCast);
    }
    
    if (Input->ArrowLeft.Pressed)
    {
        
        world_entity *Entity = AddFireBall(AppState, World, Arena,
                                           Player, Player->Position + v3{-32.f, 0.f, HandOffset},
                                           v3{-FireBallVelocity, 0.f, 0.f});
        Entity->AnimationSpeed = 1.f;
        Entity->AnimationType = ANIMATION_TYPE_MOVE;
        Entity->AnimationDirection = ANIMATION_DIRECTION_LEFT;
        PlaySound(AppState, &AppState->Assets.FireCast);
    }
    if (Input->ArrowDown.Pressed)
    {
        
        world_entity *Entity = AddFireBall(AppState, World, Arena,
                                           Player, Player->Position + v3{0.f, 32.f, HandOffset},
                    v3{0.f, FireBallVelocity});
        Entity->AnimationSpeed = 1.f;
        Entity->AnimationType = ANIMATION_TYPE_MOVE;
        Entity->AnimationDirection = ANIMATION_DIRECTION_DOWN;
        PlaySound(AppState, &AppState->Assets.FireCast);
    }
    if (Input->ArrowUp.Pressed)
    {
        
        world_entity *Entity = AddFireBall(AppState, World, Arena,
                                           Player, Player->Position + v3{0.f, -32.f, HandOffset},
                                           v3{0.f, -FireBallVelocity, 0.f});
        Entity->AnimationSpeed = 1.f;
        Entity->AnimationType = ANIMATION_TYPE_MOVE;
        Entity->AnimationDirection = ANIMATION_DIRECTION_UP;
        PlaySound(AppState, &AppState->Assets.FireCast);
    }
    
    #if 1
    float DDPlayerLengthSq = LengthSq(DDPlayer);
    if (DDPlayerLengthSq > 1.f)
    {
        DDPlayer *= 1.f / SquareRoot(DDPlayerLengthSq);
    }
    #else
    if (DDPlayer.X != 0 && DDPlayer.Y != 0)
    {
        DDPlayer *= .707106781187f;
    }
    #endif

    DDPlayer *= PlayerAcceleration * DeltaTime;
    // Drag
    DDPlayer -= (10.f * Player->Velocity);
    //Gravity
    DDPlayer.Z = -1000.f;
    
    
//    float DeltaZ = 0.5f * DDPlayer.Z * Square(DeltaTime) +
//        Player->Velocity.Z * DeltaTime;    
//    Player->Velocity.Z = DDPlayer.Z * DeltaTime +
//        Player->Velocity.Z;    
//    Player->Position.Z += DeltaZ * DeltaTime;
//    Player->Position.Z = Maximum(0.f, Player->Position.Z);
    
    float MaxDistance = 10000.f;
    MoveEntity(Player, World, Arena, Input, AppState,
               DDPlayer, &MaxDistance);
}

internal void
UpdateFamiliar(world_entity *Entity, world *World,
             memory_arena *Arena,
             app_input *Input, app_state *AppState,
             float *AnimationSpeed,
             animation_type *AnimationType,
             animation_direction *AnimationDirection)
{
    float DeltaTime = Input->DeltaTime;
    v3 DDEntity = {};
    float EntityAcceleration = 56000.f;
    *AnimationType = ANIMATION_TYPE_STAND;
    *AnimationDirection =
        Entity->AnimationSet.LastAnimationDirection;
    *AnimationSpeed = 1.f;
    
    Assert(Entity->FollowingEntity);
    v3 FollowingPos = Entity->FollowingEntity->Position;
    v3 EntityPos = Entity->Position;
    
    DDEntity = FollowingPos - EntityPos;
    
    float DDEntityAbsoluteX = Absolute(DDEntity.X);
    float DDEntityAbsoluteY = Absolute(DDEntity.Y);

        if (DDEntity.X > 0)
        {
            *AnimationDirection = ANIMATION_DIRECTION_RIGHT;
        }
        else
        {
            *AnimationDirection = ANIMATION_DIRECTION_LEFT;
        }
    
    float DDEntityLength = Length(DDEntity);
    if (DDEntityLength > 1.f)
    {
        DDEntity *= 1.f / DDEntityLength;
    }

    DDEntity *= EntityAcceleration * DeltaTime;
    DDEntity -= (10.f * Entity->Velocity);

    Entity->tFlying += DeltaTime * 5;
    float TwoPi = 2.f * Pi32;
    if (Entity->tFlying > TwoPi)
    {
        Entity->tFlying -= TwoPi;
    }
    Entity->Position.Z = 30 + 4.f * Sin(Entity->tFlying);
    
#if 0        
    float DeltaZ = 0.5f * DDEntityZ * Square(DeltaTime) +
        Entity->VelocityZ * DeltaTime;
    Entity->VelocityZ = DDEntityZ * DeltaTime +
        Entity->VelocityZ;
    Entity->Z += DeltaZ * DeltaTime;
    Entity->Z = Maximum(0.f, Entity->Z);
#endif
    
    float MaxDistanceFromFollowingEntity = 45.f;
    if (DDEntityLength > MaxDistanceFromFollowingEntity)
    {        
        float MaxDistance =  DDEntityLength -
            MaxDistanceFromFollowingEntity;
        MoveEntity(Entity, World, Arena, Input, AppState,
                   DDEntity, &MaxDistance);
    }
}

internal void
UpdateFireBall(world_entity *Entity, world *World,
               memory_arena *Arena,
               app_input *Input, app_state *AppState)
{
    float DeltaTime = Input->DeltaTime;
    v3 DDEntity = {};
    float DeltaZ = 0.5f * DDEntity.Z * Square(DeltaTime) +
        Entity->Velocity.Z * DeltaTime;    
    Entity->Velocity.Z = DDEntity.Z * DeltaTime +
        Entity->Velocity.Z;
    
    Entity->Position.Z += DeltaZ * DeltaTime;
    
    DDEntity.X -= (0.75f * Entity->Velocity.X);
    DDEntity.Y -= (0.75f * Entity->Velocity.Y);
    
    if (Entity->Position.Z <= 0.f || Entity->DistanceRemaining <= 0.f)
    {
        // TODO(zoubir): Remove Entity
        RemoveEntity(World, Entity);
    }
    else
    {
        MoveEntity(Entity, World, Arena, Input, AppState,
                   DDEntity, &Entity->DistanceRemaining);
    }
}

//TODO(zoubir): check if its 3D compatible
inline v3
CenterCamera(v3 Position, float MinX, float MinY,
             float MaxX, float MaxY, u32 WindowWidth,
             u32 WindowHeight)
{
    v3 Result = Position;
    
    Result.X -= (float)(WindowWidth / 2);
    Result.Y -= (float)(WindowHeight / 2);
    
    Result.X = Minimum(Result.X, MaxX - WindowWidth);
    Result.Y = Minimum(Result.Y, MaxY - WindowHeight);
    
    Result.X = Maximum(Result.X, MinX);
    Result.Y = Maximum(Result.Y, MinY);
    
    return Result;
}


internal void
DrawTileEntity(render_context *RenderContext,
               render_program TextureProgram,
               app_assets *Assets,
               world *World,
               world_entity *Entity, v3 CameraOffset)
{
    v2 EntityCameraPosition = Entity->Position.XY - CameraOffset.XY;
    v2 EntityTexturePosition = EntityCameraPosition - Entity->TextureOrigin;
    EntityTexturePosition.Y -= Entity->Position.Z;
    
    float SortingValue =
        Entity->Position.Y - Entity->Collision->TotalVolume.HalfDims.Y;
    gl_texture *Texture = GetTexture(Assets, Entity->Texture);
    if (Texture)
    {        
        ColorRGBA8 Color;
        Color.ColorU32 = RGBA8_WHITE;
        Color.A = 255;
        BeginBatch(RenderContext, Texture->ID,
                   SortingValue, TextureProgram);
        
        for(u32 TileY = 0;
            TileY < (Entity->NumTilesY + Entity->NumTilesZ - 1);
            TileY++)
        {
            for(u32 TileX = 0;
                TileX < Entity->NumTilesX;
                TileX++)
            {
                u32 TileValue =
                    Entity->TileIndices[TileX + TileY *
                                        (Entity->NumTilesX)];
                //TODO(zoubir): remove TextureTileNum X/Y
                // and replace them with the world
                // tileWidth and TileHeight
                u32 TextureTileNumX = Texture->Width /
                    World->TileWidth;
                u32 TextureTileNumY = Texture->Height /
                    World->TileHeight;
                v4 Uvs =
                    GetTextureUvsFromIndex(Texture->Width,
                                           Texture->Height,
                                           TextureTileNumX,
                                           TextureTileNumY,
                                           TileValue);

                v2 Offset = V2(TileX * World->TileWidth,
                               TileY * World->TileHeight);
                
                RenderQuadTexture(RenderContext,
                                  EntityTexturePosition.X + Offset.X,
                                  EntityTexturePosition.Y + Offset.Y,
                                  (float)World->TileWidth,
                                  (float)World->TileHeight,
                                  Uvs,
                                  Color.ColorU32, Entity->Position.Z);
            }
        }
        
        EndBatch(RenderContext);
        
#if 0
        for(u32 VolumeIndex = 0;
            VolumeIndex < Entity->Collision->VolumesCount;
            VolumeIndex++)
        {
            entity_collision_volume *Volume =
                Entity->Collision->Volumes + VolumeIndex;
            
        v2 CollisionRectPosition = EntityCameraPosition -
            Volume->HalfDims.XY;
        CollisionRectPosition.Y -= Entity->Position.Z -
            Volume->HalfDims.Z + Volume->Offset.Z;
    
        DrawRectangle3D(RenderContext,
                        CollisionRectPosition.X,
                        CollisionRectPosition.Y,
                        Volume->HalfDims.X * 2,
                        Volume->HalfDims.Y * 2,
                        Volume->HalfDims.Z * 2,
                        RGBA8_RED, SortingValue);
        }
    #endif
    }
}
internal void
DrawEntity(render_context *RenderContext,
           render_program TextureProgram,
           app_assets *Assets,
           world_entity *Entity, v3 CameraOffset)
{
    v2 EntityCameraPosition = Entity->Position.XY - CameraOffset.XY;
    v2 EntityTexturePosition = EntityCameraPosition - Entity->TextureOrigin;
    EntityTexturePosition.Y -= Entity->Position.Z;
    
    v2 ShadowOrigin = {7, 4};
    v2 ShadowDims = {14, 7};
    v2 ShadowPosition = EntityCameraPosition - ShadowOrigin;
    ShadowPosition.Y -= Entity->GroundZ;
    
    float SortingValue =
        Entity->Position.Y;
    gl_texture *Texture = GetTexture(Assets, Entity->Texture);
    
    if (Texture)
    {
        BeginBatch(RenderContext, Texture->ID,
                   SortingValue, TextureProgram);

        ColorRGBA8 Color;
        Color.ColorU32 = RGBA8_WHITE;
        Color.A = 255;
        RenderQuadTexture(RenderContext,
                          EntityTexturePosition.X,
                          EntityTexturePosition.Y,
                          Entity->Dimensions.X,
                          Entity->Dimensions.Y,
                          Entity->Uvs,
                          Color.ColorU32, Entity->Position.Z);
        EndBatch(RenderContext);
    }
    
    gl_texture *ShadowTexture = GetTexture(Assets, Entity->ShadowTexture);
    
    if (ShadowTexture)
    {
        BeginBatch(RenderContext, ShadowTexture->ID,
                   Entity->Position.Y, TextureProgram);

        ColorRGBA8 ShadowColor;
        ShadowColor.ColorU32 = RGBA8_WHITE;
        if (Entity->Position.Z < 200)
        {
            ShadowColor.A = (u8)(255 - (Entity->Position.Z - Entity->GroundZ) - 55);
        }
        else
        {
            ShadowColor.A = 0;
        }
                            
        RenderQuadTexture(RenderContext,
                          ShadowPosition.X,
                          ShadowPosition.Y,
                          ShadowDims.X,
                          ShadowDims.Y,
                          {0.f, 0.f, 1.f, 1.f},
                          ShadowColor.ColorU32, 0.f);
        EndBatch(RenderContext);
    }
    
    {
        // Hp
        if (Entity->Hp > 0.f && Entity->Hp <= 100.f)
        {
        DrawRectangle(RenderContext,
                      EntityTexturePosition.X +
                      Entity->Dimensions.X * 0.25f,
                      EntityTexturePosition.Y -
                      10,
                      (Entity->Hp / 100.f) *
                      (Entity->Dimensions.X * 0.5f),
                      1,
                      RGBA8_WHITE, SortingValue);
        }
#if 0
        DrawRectangle(RenderContext,
                      EntityTexturePosition.X,
                      EntityTexturePosition.Y,
                      Entity->Dimensions.X,
                      Entity->Dimensions.Y,
                      RGBA8_WHITE, SortingValue);
#endif

#if 0        
        for(u32 VolumeIndex = 0;
            VolumeIndex < Entity->Collision->VolumesCount;
            VolumeIndex++)
        {
            entity_collision_volume *Volume =
                Entity->Collision->Volumes + VolumeIndex;
            
        v2 CollisionRectPosition = EntityCameraPosition -
            Volume->HalfDims.XY;
        CollisionRectPosition.Y -= Entity->Position.Z -
            Volume->HalfDims.Z +
            Volume->Offset.Z;
    
        DrawRectangle3D(RenderContext,
                        CollisionRectPosition.X,
                        CollisionRectPosition.Y,
                      Volume->HalfDims.X * 2,
                      Volume->HalfDims.Y * 2,
                      Volume->HalfDims.Z * 2,
                      RGBA8_RED, SortingValue);
        }
        
#endif
        
#if 0        
        entity_collision_volume *Volume =
            &Entity->Collision->TotalVolume;
            
        v2 CollisionRectPosition = EntityCameraPosition -
            Volume->HalfDims.XY;
        CollisionRectPosition.Y -= Entity->Position.Z -
            Volume->HalfDims.Z +
            Volume->Offset.Z;
    
        DrawRectangle3D(RenderContext,
                        CollisionRectPosition.X,
                        CollisionRectPosition.Y,
                        Volume->HalfDims.X * 2,
                        Volume->HalfDims.Y * 2,
                        Volume->HalfDims.Z * 2,
                        RGBA8_RED, SortingValue);
#endif
    }

}

//TODO(zoubir): remove the collision and tiles in
// world_position
extern "C" APP_UPDATE_AND_RENDER(AppUpdateAndRender)
{
    app_state *AppState = (app_state *)Memory->PermanentStorage;
    transient_state *TransientState = (transient_state *)Memory->TransientStorage;
    memory_arena *MemoryArena = &AppState->MemoryArena;
    memory_arena *TransientArena = &TransientState->MemoryArena;
    memory_arena *ConstantsArena = &AppState->ConstantsArena;
    
    app_assets *Assets = &AppState->Assets;

    if (!AppState->IsInitialized)
    {
        PlatformAddWorkEntry = Memory->PlatformAddWorkEntry;
        DEBUGPlatformReadEntireFile = Memory->DEBUGPlatformReadEntireFile;
        DEBUGPlatformFreeFileMemory = Memory->DEBUGPlatformFreeFileMemory;
        DEBUGPlatformWriteEntireFile = Memory->DEBUGPlatformWriteEntireFile;
        
        InitializeArena(MemoryArena,
                        (memory_index *)(AppState + 1),
                        Memory->PermanentStorageSize - sizeof(app_state));
        
        InitializeArena(TransientArena,
                        (memory_index *)(TransientState + 1),
                        Memory->TransientStorageSize - sizeof(transient_state));
        SubArena(ConstantsArena, MemoryArena, Kilobytes(64));
        
        AppState->WorkQueue = Memory->WorkQueue;
        
        temporary_memory TempMem = BeginTemporaryMemory(TransientArena);
        AppInitOpenGL(TransientArena, AppState, Thread, Memory);

        // Texture Loading        
        AppState->TextureCache = TextureCacheCreate(MemoryArena, 8, 20 * 8);

        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_Dragon);
        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_Zoubir);
        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_Shadow);
        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_TileMap);
        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_Tree);
        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_Familiar);
        LoadAsset(Assets, AppState, Memory, MemoryArena, AAI_FireBall);
            
        #if 0
        Assets->DragonTexture =
            LoadTexture(Thread, Memory, "a.zi", TEXTURE_SOFT_FILTER);
        
        TextureCacheInsert(AppState->TextureCache, "a", Assets->DragonTexture);
        Assets->ZoubirTexture =
            LoadTexture(Thread, Memory, "zoubir.zi", TEXTURE_SOFT_FILTER);
        TextureCacheInsert(AppState->TextureCache, "zoubir", Assets->ZoubirTexture);
        
        Assets->ShadowTexture =
            LoadTexture(Thread, Memory, "player_shadow.zi", TEXTURE_SOFT_FILTER);
        
        Assets->TileMap1 =
            LoadTexture(Thread, Memory, "forest_pallet_01.zi", TEXTURE_NO_FILTER);
        
        Assets->Tree =
            LoadTexture(Thread, Memory, "tree_summer_01.zi", TEXTURE_SOFT_FILTER);
        
        Assets->FamiliarTexture =
            LoadTexture(Thread, Memory, "familiar.zi", TEXTURE_SOFT_FILTER);
        
        Assets->FireBallTexture =
            LoadTexture(Thread, Memory, "fireball.zi", TEXTURE_SOFT_FILTER);
        #endif
        
        AppState->DefaultFont =
            CreateFont(MemoryArena,
                       24.f, 512, 512,
                       "c:/windows/fonts/times.ttf");

        AppState->UIContext =
            UIContextCreate(MemoryArena, UI_COUNT);
        // TODO(zoubir): make this struct a pointer
        // to a struct
        Assets->ZoubirAudio = LoadWav("something.wav");
        Assets->Dash = LoadWav("Dash.wav");
        Assets->FireCast = LoadWav("fire_cast.wav");        

        u32 const TileWidth = 32;
        u32 const TileHeight = 32;
        u32 const TileDepth = 32 * 1;

        u32 const CollisionWidth = TileWidth;
        u32 const CollisionHeight = TileHeight;
        u32 const CollisionDepth = TileDepth;
        
        u32 const DesiredTilesX = 80;
        u32 const DesiredTilesY = 40;
        u32 const DesiredTilesZ = 12;
        
        u32 const CollisionToTilesX =
            TileWidth / CollisionWidth;
        u32 const CollisionToTilesY =
            TileHeight / CollisionHeight;

        AppState->TileObjectCollision =
            MakeSimpleGroundedCollisionVolume(ConstantsArena, {96.f*0.5f, 76.f*0.5f, 24.f});
        AppState->TreeCollision =
            MakeGroundedTreeCollisionVolume(ConstantsArena);
        AppState->WallCollision = 
            MakeSimpleGroundedCollisionVolume(ConstantsArena, {TileWidth*.5f, TileHeight*.5f, TileDepth *.5f});
        AppState->PlayerCollision = 
            MakeSimpleGroundedCollisionVolume(ConstantsArena, {10, 4, 18.f});
        AppState->FamiliarCollision = 
            MakeSimpleGroundedCollisionVolume(ConstantsArena, {11, 6, 0.f});
        AppState->FireBallCollision = 
            MakeSimpleGroundedCollisionVolume(ConstantsArena, {9, 9, 0.f});
        
        world *World = &AppState->World;
        World->TileWidth = TileWidth;
        World->TileHeight = TileHeight;
        World->TileDepth = TileDepth;
        
        World->CollisionWidth = World->TileWidth;
        World->CollisionHeight = World->TileHeight;
        World->CollisionDepth = World->TileDepth;
        
        World->TilesPerChunkX = 16;
        World->TilesPerChunkY = 16;
        World->TilesPerChunkZ = 4;

        World->MaxEntityVelocity = {1.f, 1.f, 1.f};
        
        
        Assert(World->CollisionWidth <= World->TileWidth);
        Assert(World->CollisionHeight <= World->TileHeight);
        
        u32 const CollisionNumX = DesiredTilesX * CollisionToTilesX;
        u32 const CollisionNumY = DesiredTilesY * CollisionToTilesY;
        bool32 *CollisionTiles =
            AllocateArray(MemoryArena,
                          CollisionNumX * CollisionNumY,
                          bool32);
        World->CollisionMap = CollisionTiles;
        World->NumCollisionX = CollisionNumX;
        World->NumCollisionY = CollisionNumY;
    
        u32 const TileNumX = DesiredTilesX;
        u32 const TileNumY = DesiredTilesY;
        u32 const TileNumZ = DesiredTilesZ;
        
        asset_id TMT = AAI_TileMap;
        tile_map *TileMap = &World->TileMap;
        TileMap->Texture = TMT;
        World->NumTilesX = TileNumX;
        World->NumTilesY = TileNumY;
        World->NumTilesZ = TileNumZ;
        tile *Tiles = AllocateArray(MemoryArena,
                                    TileNumX * TileNumY,
                                    tile);

        for(u32 CurrentTile = 0;
            CurrentTile < (World->NumCollisionX *
                           World->NumCollisionY);
            CurrentTile++)
        {
            CollisionTiles[CurrentTile] = CurrentTile % (17) == 2;
            if ((CurrentTile < World->NumCollisionX) ||
                (CurrentTile > (World->NumCollisionX *
                                 World->NumCollisionY) - World->NumCollisionX) ||
                (CurrentTile % World->NumCollisionX) == 0 ||
                ((CurrentTile + 1) % World->NumCollisionX) == 0)
            {
                CollisionTiles[CurrentTile] = true;
                #if 1
                u32 X = CurrentTile % World->NumCollisionX;
                u32 Y = CurrentTile / World->NumCollisionX;
                v3 Pos;
                Pos.X = X * (float)World->TileWidth +
                    (float)World->TileWidth / 2.f;
                Pos.Y = Y * (float)World->TileHeight +
                    (float)World->TileHeight / 2.f;
                Pos.Z = 0.f;
                AddWall(AppState, World, MemoryArena, Pos);
                #endif
            }            
        }
    
        u32 TileIndex = 1592;
        //u32 TileIndex = 10;
        for(u32 CurrentTile = 0;
            CurrentTile < (World->NumTilesX * World->NumTilesY);
            CurrentTile++)
        {
            if (CollisionTiles[CurrentTile])
            {
//                Tiles[CurrentTile].Index = 4;
                Tiles[CurrentTile].Index = TileIndex;
            }
            else
            {            
                Tiles[CurrentTile].Index = TileIndex;
                u32 CurrentTileY = CurrentTile / World->NumTilesX;
                u32 CurrentTileX = CurrentTile % World->NumTilesX;
                if (CurrentTileX % World->TilesPerChunkX == 0 ||
                    CurrentTileY % World->TilesPerChunkY == 0)
                {
                    Tiles[CurrentTile].Index = 1567;
                    //Tiles[CurrentTile].Index = 6;
                }
                if (CurrentTile % 70 == 0)
                {
                    u32 X = CurrentTile % World->NumTilesX;
                    u32 Y = CurrentTile / World->NumTilesX;
                    v3 Pos;
                    Pos.X = X * (float)World->TileWidth +
                        (float)World->TileWidth / 2.f;
                    Pos.Y = Y * (float)World->TileHeight +
                        (float)World->TileHeight / 2.f;
                    Pos.Z = 0.f;
                    AddTree(AppState, World, MemoryArena, Pos);
                }
            }
        }            

        TileMap->Tiles = Tiles;

        world_entity *Player = AddPlayer(AppState, World,
                                         MemoryArena, {350, 300});
        AppState->Player = Player;
        
        #if 0
        world_entity *Familiar =
            AddFamiliar(AppState, World, MemoryArena, Player);
        #endif
        
        for(u32 MonsterIndex = 0;
            MonsterIndex < 10;
            MonsterIndex++)
        {
            AddMonster(AppState, World,
                       MemoryArena,
                       {((MonsterIndex % 3) * 32.f + MonsterIndex) * 32.f,
                               600 +
                               (float)((MonsterIndex % 2) * 5 + MonsterIndex) * 4,
                               0.f});
        }

        random_series Series = Seed(67);
        u32 PosX = 12;
        u32 PosY = 6;
        for(u32 EntityIndex = 0;
            EntityIndex < 10;
            EntityIndex++)
        {
            u32 DirX = RandomChoice(&Series, 2);
            u32 DirY = RandomChoice(&Series, 2);
            PosX += DirX * 5;
            PosY += DirY * 5;
            AddTileEntity(AppState, World, MemoryArena, PosX, PosY, 0);
        }
        
        
        SetCollision(AppState, EntityType_Familiar,
                     EntityType_StaticObject, true);
        
        SetCollision(AppState, EntityType_Familiar,
                     EntityType_Monster, true);
        
        SetCollision(AppState, EntityType_Player,
                     EntityType_Player, true);
        
        SetCollision(AppState, EntityType_Player,
                     EntityType_StaticObject, true);
        
        SetCollision(AppState, EntityType_Player,
                     EntityType_Tiled, true);
        
        SetCollision(AppState, EntityType_Player,
                     EntityType_Monster, true);

        SetCollision(AppState, EntityType_Monster,
                     EntityType_StaticObject, true);

        SetCollision(AppState, EntityType_Monster,
                     EntityType_FireBall, true);
        
        SetCollision(AppState, EntityType_Monster,
                     EntityType_Monster, true);

        SetCollision(AppState, EntityType_StaticObject,
                     EntityType_StaticObject, true);

        EndTemporaryMemory(TempMem);
        AppState->IsInitialized = true;
    }

    render_context *RenderContext = &Thread->RenderContext;
    open_gl *OpenGL = RenderContext->OpenGL;
    render_program *TextureProgram = &RenderContext->TextureProgram;
    render_program *LineProgram = &RenderContext->LineProgram;
    
    LoadOpenglTexturesFromQueue(OpenGL, &AppState->OpenglTextureQueue);
    
    ui_context *UIContext = AppState->UIContext;
    
    temporary_memory FrameTemporaryMemory =
        BeginTemporaryMemory(TransientArena);
    
    texture_cache *TextureCache = AppState->TextureCache;
    gl_texture *DragonTexture = GetTexture(Assets, AAI_Dragon);
    gl_texture *ZoubirTexture = GetTexture(Assets, AAI_Zoubir);
    font *DefaultFont = AppState->DefaultFont;

    float Right = (float)Window->Width;
    float Left = 0.f;
    float Top = 0.f;
    float Bottom = (float)Window->Height;
    float Far = 100000.f;
    float Near = 0.f;
        
    mat4 ProjectionMatrix
        = OrthoMatrix(Left, Right, Bottom, Top, Near, Far);

    TextureProgram->ProjectionMatrix = &ProjectionMatrix;
    LineProgram->ProjectionMatrix = &ProjectionMatrix;
    
    ArrayCount(Input->controllers[0].buttons) * sizeof(app_button_state);
    Assert(&Input->controllers[0].terminator - &Input->controllers[0].buttons[0] ==
           ArrayCount(Input->controllers[0].buttons));
    Assert(Memory->PermanentStorageSize > sizeof(app_state));

    for(int controllerIndex = 0;
        controllerIndex < ArrayCount(Input->controllers);
        controllerIndex++)
    {
        app_controller_input *thisController = GetController(Input, controllerIndex);

    }
    glViewport(0, 0, Window->Width, Window->Height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

//  gl_texture ATexture = AcquireTexture("a");
    world *World = &AppState->World;
    tile_map *TileMap = &World->TileMap;
    world_entity *Player = AppState->Player;

    float TileMapWidth = (float)(World->NumTilesX * World->TileWidth);
    float TileMapHeight = (float)(World->NumTilesY * World->TileHeight);
    u32 WindowHalfWidth = Window->Width / 2;
    u32 WindowHalfHeight = Window->Height / 2;

    AppState->TargetCamera =
        CenterCamera(Player->Position,
                     0.f, 0.f,
                     TileMapWidth,
                     TileMapHeight,
                     Window->Width,
                     Window->Height);

    #if 0
    float LerpValue = 0.95f;
    float DeltaTime = Input->DeltaTime;
    v3 CameraOffset = AppState->CameraOffset =
        AppState->CameraOffset +
        (AppState->TargetCamera - AppState->CameraOffset) *
        LerpValue * DeltaTime;
    #else
    v3 CameraOffset = AppState->CameraOffset =
        AppState->TargetCamera;
    #endif
    
    CameraOffset.X = (float)((u32)CameraOffset.X);
    CameraOffset.Y = (float)((u32)CameraOffset.Y);
    
    v3 CameraDims = {(float)Window->Width,
                     (float)Window->Height,
                     1.f};
    v3 CameraGrabEntitySafetyMargin = {32, 32};
    v3 CameraMinPos = CameraOffset;
    v3 CameraMaxPos = CameraOffset + CameraDims;
    CameraMinPos -= CameraGrabEntitySafetyMargin;
    CameraMaxPos += CameraGrabEntitySafetyMargin;
    
    u32 MinChunkX;
    u32 MinChunkY;
    u32 MinChunkZ;
    
    u32 MaxChunkX;
    u32 MaxChunkY;
    u32 MaxChunkZ;
    
    rectangle3 Box = RectMinMax(CameraMinPos,
                                CameraMaxPos);
    GetChunksFromBox(World,
                     Box,
                     &MinChunkX, &MinChunkY, &MinChunkZ,
                     &MaxChunkX, &MaxChunkY, &MaxChunkZ);
    
    
    #if 1
    {
    u32 BatchesCount =(World->NumTilesX *
                       World->NumTilesY + World->EntityCount * 2) ;
    u32 VerticesCount = 6 * BatchesCount;
    SetupBatchRenderer(RenderContext, TransientArena, BatchesCount);
    RenderBegin(RenderContext, VerticesCount, RENDER_ORDER_BACK_TO_FRONT);
    
    gl_texture *TileMapTexture = GetTexture(Assets, TileMap->Texture);
    // Drawing Tile Map
#if 1
    if (TileMapTexture)
    {
    BeginBatch(RenderContext, TileMapTexture->ID, 0.f, *TextureProgram);    
    u32 TextureTileNumX = TileMapTexture->Width / 32;
    u32 TextureTileNumY = TileMapTexture->Height / 32;
    for(u32 TileYIndex = 0;
        TileYIndex < World->NumTilesY;
        TileYIndex++)
    {
        for(u32 TileXIndex = 0;
            TileXIndex < World->NumTilesX;
            TileXIndex++)
        {
            tile *ThisTile =
                &TileMap->Tiles[TileXIndex +
                              TileYIndex * World->NumTilesX];
            
            v4 TileUvs = GetTextureUvsFromIndex(TileMapTexture->Width,
                                            TileMapTexture->Height,
                                            TextureTileNumX,
                                            TextureTileNumY,
                                            ThisTile->Index);
            RenderQuadTexture(RenderContext,
                              (float)TileXIndex * World->TileWidth -
                              CameraOffset.X,
                              (float)TileYIndex * World->TileHeight -
                              CameraOffset.Y,
                              (float)World->TileWidth,
                              (float)World->TileHeight,
                              TileUvs,
                              RGBA8_WHITE,
                              0.f);
        }
    }    
    #endif
            EndBatch(RenderContext);
    }
    }
    #endif
    
#if 1
    // TODO(zoubir): IMPORTANT
    // this shit is not updating entities
    // that are not in camera bounds
    u32 UpdateID = AppState->UpdateID++;
    
    for(u32 EntityIndex = 0;
        EntityIndex < World->EntityCount;
        EntityIndex++)
    {
        world_entity *ThisEntity =
            &World->Entities[EntityIndex];
        if (ThisEntity->IsPresent)
        {
                        
//                        Assert(ThisEntity->CollisionHalfDims.Z * 2.f <
//                               World->MaxEntityHalfDims.Z);

//                        Assert(ThisEntity->CollisionHalfDims.Z * 2.f >
//                               World->MaxEntityVelocity.Z);

                        
        if (ThisEntity->UpdateID != UpdateID)
        {
            ThisEntity->UpdateID = UpdateID;
        switch(ThisEntity->Type)
        {
            case EntityType_Player:
            {
                // NOTE(zoubir): player code    
                float PlayerAnimationSpeed;
                animation_type PlayerAnimationType;
                animation_direction PlayerAnimationDirection;
    
                UpdatePlayer(ThisEntity, World, MemoryArena, Input, AppState,
                             &PlayerAnimationSpeed,
                             &PlayerAnimationType,
                             &PlayerAnimationDirection);

                gl_texture *Texture = GetTexture(Assets, ThisEntity->Texture);
                if (Texture)
                {
                    ThisEntity->Uvs = DoAnimation(&ThisEntity->AnimationState, 
                                                  Texture->Width,
                                                  Texture->Height,
                                                  ThisEntity->TextureNumTilesX,
                                                  ThisEntity->TextureNumTilesY,
                                                  Input->DeltaTime, 0.15f * PlayerAnimationSpeed,
                                                  &ThisEntity->AnimationSet,
                                                  PlayerAnimationType,
                                                  PlayerAnimationDirection);
                }
                
                DrawEntity(RenderContext, *TextureProgram,
                           Assets,
                           ThisEntity, CameraOffset);

                break;
            }                    
            case EntityType_Familiar:
            {
                // NOTE(zoubir): player code    
                float EntityAnimationSpeed;
                animation_type EntityAnimationType;
                animation_direction EntityAnimationDirection;
    
                UpdateFamiliar(ThisEntity, World, MemoryArena, Input, AppState,
                               &EntityAnimationSpeed,
                               &EntityAnimationType,
                               &EntityAnimationDirection);

                gl_texture *Texture = GetTexture(Assets, ThisEntity->Texture);
                if (Texture)
                {
                ThisEntity->Uvs = DoAnimation(&ThisEntity->AnimationState, 
                                              Texture->Width,
                                              Texture->Height,
                                              ThisEntity->TextureNumTilesX, ThisEntity->TextureNumTilesY,
                                              Input->DeltaTime, 0.15f * EntityAnimationSpeed,
                                              &ThisEntity->AnimationSet,
                                              EntityAnimationType,
                                              EntityAnimationDirection);
                }

                DrawEntity(RenderContext, *TextureProgram,
                           Assets,
                           ThisEntity, CameraOffset);
                break;
            }                    
            case EntityType_FireBall:
            {
                // NOTE(zoubir): player code    
                float EntityAnimationSpeed = ThisEntity->AnimationSpeed;
                animation_type EntityAnimationType = ThisEntity->AnimationType;
                animation_direction EntityAnimationDirection = ThisEntity->AnimationDirection;
    
                UpdateFireBall(ThisEntity, World, MemoryArena, Input, AppState);

                gl_texture *Texture = GetTexture(Assets, ThisEntity->Texture);
                if (Texture)
                {
                ThisEntity->Uvs = DoAnimation(&ThisEntity->AnimationState, 
                                              Texture->Width,
                                              Texture->Height,
                                              ThisEntity->TextureNumTilesX, ThisEntity->TextureNumTilesY,
                                              Input->DeltaTime, 0.15f * EntityAnimationSpeed,
                                              &ThisEntity->AnimationSet,
                                              EntityAnimationType,
                                              EntityAnimationDirection);

                }
                
                DrawEntity(RenderContext, *TextureProgram,
                           Assets,
                           ThisEntity, CameraOffset);
                break;
            }                    
            case EntityType_StaticObject:
            {
                                
                DrawEntity(RenderContext, *TextureProgram,
                           Assets,
                           ThisEntity, CameraOffset);
                break;
            }                    
            case EntityType_Tiled:
            {
                                
                DrawTileEntity(RenderContext, *TextureProgram,
                               Assets,
                               World, ThisEntity, CameraOffset);
                break;
            }
            case EntityType_Monster:
            {
                //Gravity
                float Gravity = -1000.f;
                float MaxDistance = 10000;
                MoveEntity(ThisEntity, World, MemoryArena, Input, AppState,
                           {0.f, 0.f, Gravity}, &MaxDistance);
                
                float EntityAnimationSpeed = 1.f;
                animation_type EntityAnimationType = ANIMATION_TYPE_STAND;
                animation_direction EntityAnimationDirection =
                    ANIMATION_DIRECTION_DOWN;

                gl_texture *Texture = GetTexture(Assets, ThisEntity->Texture);
                if (Texture)
                {
                ThisEntity->Uvs = DoAnimation(&ThisEntity->AnimationState, 
                                              Texture->Width,
                                              Texture->Height,
                                              ThisEntity->TextureNumTilesX, ThisEntity->TextureNumTilesY,
                                              Input->DeltaTime, 0.15f * EntityAnimationSpeed,
                                              &ThisEntity->AnimationSet,
                                              EntityAnimationType,
                                              EntityAnimationDirection);
                }
                
                DrawEntity(RenderContext, *TextureProgram,
                           Assets,
                           ThisEntity, CameraOffset);
                break;
            }                    
        }
        }
        }
    }
    RenderFlush(RenderContext);

#endif



//    EndTemporaryMemory(FrameTemporaryMemory);
//    FrameTemporaryMemory = BeginTemporaryMemory(TransientArena);
    
    if (Input->ButtonF3.Pressed)
    {
        AppState->TileEditing = !AppState->TileEditing;
    }
#if 1
    u32 NumberOfElements = 200;
    UIBegin(RenderContext, TransientArena, Input, AppState,
            UIContext, NumberOfElements, 4);
    if (AppState->TileEditing)
    {
        float ContainerWidth = 300;
        float ContainerHeight = 600;
        float ContainerX = Window->Width - ContainerWidth - 60;
        float ContainerY = (Window->Height - ContainerHeight) / 2;
        
        BeginContainer(UIContext, ContainerX, ContainerY,
                       ContainerWidth, ContainerHeight);

        DrawRectangle(RenderContext, ContainerX, ContainerY,
                      ContainerWidth, ContainerHeight,
                      RGBA8_YELLOW, 0.f);

        ui_state *TilePickerWidget = &AppState->TilePickerWidget;
        DoTilePickerWidget(&AppState->TilePickerWidget, AppState,
                           UIContext, 0.f, 0.f, 200.f, 200.f,
                           GetTexture(Assets, AAI_TileMap), 32, 32,
                           8, 200);
        float MouseXF = (float)Input->MouseX;
        float MouseYF = (float)Input->MouseY;
        
        float TileWidthF = (float)World->TileWidth;
        float TileHeightF = (float)World->TileHeight;

        float X = (float)((u32)((MouseXF + CameraOffset.X) /
                                TileWidthF));
        float Y = (float)((u32)((MouseYF + CameraOffset.Y) /
                                TileHeightF));
        X *= TileWidthF;
        Y *= TileHeightF;
        X -= CameraOffset.X;
        Y -= CameraOffset.Y;

        gl_texture *TileMapTexture = AppState->TilePickerWidget.TileMap;
        if (TileMapTexture)
        {
        for(u32 IndexY = 0;
            IndexY < TilePickerWidget->SelectedTileCountY;
            IndexY++)
        {
            for(u32 IndexX = 0;
                IndexX < TilePickerWidget->SelectedTileCountX;
                IndexX++)
            {
                u32 IndexInTexture =
                    AppState->TilePickerWidget.SelectedTileIndices[IndexX]
                    [IndexY];
                u32 NumTilesX = AppState->TilePickerWidget.TileMapNumTilesX;
                u32 NumTilesY = AppState->TilePickerWidget.TileMapNumTilesY;
        
                v4 Uvs = GetTextureUvsFromIndex(TileMapTexture->Width,
                                                TileMapTexture->Height,
                                                NumTilesX, NumTilesY,
                                                IndexInTexture);

                u32 TileOffsetX = IndexX * World->TileWidth;
                u32 TileOffsetY = IndexY * World->TileHeight;
                
                BeginBatch(RenderContext, TileMapTexture->ID, 0.f, *TextureProgram);        
                RenderQuadTexture(RenderContext,
                                  X + IndexX * TileWidthF,
                                  Y + IndexY * TileHeightF,
                                  TileWidthF,
                                  TileHeightF,
                                  Uvs, RGBA8_WHITE, 0.f);
                EndBatch(RenderContext);
                if (Input->LeftButton.EndedDown &&
                    !IsMouseOnRectangle(Input->MouseX, Input->MouseY,
                                        ContainerX, ContainerY,
                                        ContainerWidth,
                                        ContainerHeight))
                {
                    u32 TileXIndex =
                        ((u32)(CameraOffset.X + Input->MouseX + TileOffsetX) /
                         World->TileWidth);
                    u32 TileYIndex =
                        ((u32)(CameraOffset.Y + Input->MouseY + TileOffsetY) /
                         World->TileHeight);
                    TileMap->Tiles[TileXIndex +
                                   (TileYIndex * World->NumTilesX)].Index =
                        IndexInTexture;
                }
            }
        }
        }
        float TilesRectangleWidth =
            (float)(TilePickerWidget->SelectedTileCountX
                    * World->TileWidth);
        float TilesRectangleHeight =
            (float)(TilePickerWidget->SelectedTileCountY *
                    World->TileHeight);
        DrawRectangle(RenderContext, X, Y,
                      TilesRectangleWidth,
                      TilesRectangleHeight,
                      RGBA8_YELLOW, 0.f);
        EndContainer(UIContext);
        
    }
    #if 0
    for(int Number = 0;
        Number < 9;
        Number++)
    {
        if (Input->NumbersButtons[Number].EndedDown)
        {
            ui_state *Button1 = UIContextGetState(UIContext, UI_BUTTON1);
            if (DoButton(Button1, AppState, UIContext, -100 + Number * 50.f, 10.f, 200.f,
                         30.f, "Hjglo"))
            {
        
            }
        }
    }
    if (DoButton(UI_BUTTON2, AppState, UIContext, 400.f, 0.f, 200.f,
                       30.f, "Hjglo"))
    {
        if (DoButton(UI_BUTTON3, AppState, UIContext, 200.f, 200.f,
                           33.f, 33.f, "Helgo"))
        {
            
        }        
    }

    DoEditBox(UI_EDITBOX1, AppState, UIContext, 200.f, 300.f,
              100.f, 33.f);

    DoEditBox(UI_EDITBOX2, AppState, UIContext, 240.f, 300.f,
              100.f, 33.f);
    //TODO(zoubir): remove all local_persist
    //bad for hot loading
    local_persist ui_state EditBox3 = {};
    DoEditBox(&EditBox3, AppState, UIContext, 240.f, 200.f,
              100.f, 33.f);
    
    local_persist ui_state TextLabel = {};
    DoTextLabel(&TextLabel, AppState, UIContext, 240.f, 400.f,
                100.f, 33.f, "Hi Sir", TEXT_JUSTIFICATION_MIDDLE);
    #endif
    
    UIEnd(UIContext);
    
    for(int buttonIndex = 0;
        buttonIndex < ArrayCount(Input->mouseButtons);
        buttonIndex++)
    {
        if (Input->mouseButtons[buttonIndex].EndedDown)
        {
//            RenderPlayer(Buffer, 100 * buttonIndex, 100);
        }
    }

#endif
    EndTemporaryMemory(FrameTemporaryMemory);
}

extern "C" APP_GET_SOUND_SAMPLES(AppGetSoundSamples)
{
    app_state *AppState = (app_state *)Memory->PermanentStorage;
    transient_state *TransientState = (transient_state *)Memory->TransientStorage;
    memory_arena *Arena = &TransientState->MemoryArena;
    int SampleCount = SoundBuffer->SampleCount;
    
    temporary_memory TempMem = BeginTemporaryMemory(Arena);

    float *Channel0 = AllocateArray(Arena, SampleCount, float);
    float *Channel1 = AllocateArray(Arena, SampleCount, float);
    
    ZeroArray(Channel0, SampleCount, float);
    ZeroArray(Channel1, SampleCount, float);

    for(u32 PlayingIndex = 0;
        PlayingIndex < AppState->PlayingSoundsCount;
        )
    {
        bool32 SoundFinished = false;
        playing_sound *ThisSound = &AppState->PlayingSounds[PlayingIndex];
        loaded_audio *LoadedAudio = ThisSound->Audio;
        
        i16 *Data = LoadedAudio->Data;
        i16 *Src = Data + ThisSound->SamplesPlayed;
        u32 ThisSoundSampleCount = LoadedAudio->SampleCount;

        Assert(Data);
        float Volume0 = ThisSound->Volume[0];
        float Volume1 = ThisSound->Volume[1];

        float *Dest0 = Channel0;
        float *Dest1 = Channel1;

        Assert(ThisSound->SamplesPlayed >= 0);

        u32 SamplesToMix = SampleCount;
        u32 SamplesRemainingInSound = ThisSoundSampleCount -
            ThisSound->SamplesPlayed;
        if (SamplesToMix >= SamplesRemainingInSound)
        {
            SamplesToMix = SamplesRemainingInSound;
            SoundFinished = true;
        }
        ThisSound->SamplesPlayed += SamplesToMix;

        for(u32 SampleIndex = 0;
            SampleIndex < SamplesToMix;
            SampleIndex++)
        {                
            float SampleValue0 = *Src++;
//                float SampleValue1 = *Src++;

            *Dest0++ += Volume0 * SampleValue0;
            *Dest1++ += Volume1 * SampleValue0;
        }

        if (SoundFinished)
        {
            AppState->PlayingSounds[PlayingIndex] =
                AppState->PlayingSounds[(AppState->PlayingSoundsCount--) - 1];
        }
        else
        {
            PlayingIndex++;
        }
    }

    float *Source0 = Channel0;
    float *Source1 = Channel1;

    i16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        SampleIndex++)
    {
        *SampleOut++ = (i16)((*Source0++) + 0.5f);
        *SampleOut++ = (i16)((*Source1++) + 0.5f);
    }
    EndTemporaryMemory(TempMem);
#if 0
    AppOutputSound(AppState, SoundBuffer);
#endif
}
