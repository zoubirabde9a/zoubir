#if !defined(APP_PLATFORM_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define APP_PLATFORM_H
#include <stdint.h>
#include "GL\glew.h"

#define PI32 3.14159265359f
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t bool32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define internal static
#define local_persist static
#define global_variable static

#include "math.h"
#include "render.h"

#if APP_DEV
#define Assert(expression) if (!(expression)) {*(int*)0 = 0;}
#define InvalidCodePath Assert(!"InvalidCodePath")
#else
#define Assert(expression)
#endif

#define Kilobytes(value) ((value) * 1024ll)
#define Megabytes(value) (Kilobytes(value) * 1024ll)
#define Gigabytes(value) (Megabytes(value) * 1024ll)
#define Terabytes(value) (Gigabytes(value) * 1024ll)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define OffsetOf(Type, Element) ((size_t)&(((Type *)0)->Element))

inline u32
safeTruncateU32(u64 value)
{
    Assert(value <= 0xFFFFFFFF);
    return (u32)value;
}

// NOTE(zoubir): Services that the platform layer provides to the app
struct thread_context
{
    int placeHolder;
    render_context RenderContext;
};

struct read_file_result
{
    void *Memory;
    u32 Size;
};

//#define PLATFORM_READ_ENTIRE_FILE(name) read_file_result name(thread_context* Thread, app_memory* Memory, char* FileName)
//typedef PLATFORM_READ_ENTIRE_FILE(platform_read_entire_file);

#if APP_DEV
struct debug_read_file_result
{
    void *Memory;
    u32 Size;
};

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context* thread, char* fileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context* thread, void *memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context* thread, char *fileName, u32 memorySize, void *memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#else
#endif

struct platform_work_queue;
#define PLATFORM_WORK_QUEUE_CALLBACK(Name) void Name(void *Data)
typedef PLATFORM_WORK_QUEUE_CALLBACK(platform_work_queue_callback);

#define PLATFORM_ADD_WORK_ENTRY(Name) void Name(platform_work_queue *Queue, platform_work_queue_callback *Callback, void *Data)
typedef PLATFORM_ADD_WORK_ENTRY(platform_add_work_entry);

struct app_window
{
    int Width;
    int Height;
};

struct app_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    i16* Samples;    
};

struct app_button_state
{
    int halfTransitionCount;
    bool32 Pressed;
    bool32 EndedDown;
    bool32 Released;
};

struct app_controller_input
{
    bool32 isConnected;
    bool32 isAnalog;

    float stickAverageX;
    float stickAverageY;

    union
    {
        app_button_state buttons[12];
        struct
        {
            app_button_state stickUp;
            app_button_state stickDown;
            app_button_state stickLeft;
            app_button_state stickRight;

            app_button_state buttonUp;
            app_button_state buttonDown;
            app_button_state buttonLeft;
            app_button_state buttonRight;

            app_button_state leftShoulder;
            app_button_state rightShoulder;

            app_button_state start;
            app_button_state back;

            // NOTE(zoubir): all buttons hould be inserted above this
            app_button_state terminator;
        };
    };
};

//TODO(zoubir): mb to big for stack
struct app_input
{
    float DeltaTime;
    union
    {   
        app_button_state mouseButtons[5];
        struct
        {
            app_button_state LeftButton;
            app_button_state midButton;
            app_button_state rightButton;
            app_button_state xButton1;
            app_button_state xButton2;
        };
    };
    i32 MouseX, MouseY, MouseZ;
    
    app_controller_input controllers[5];

    app_button_state ArrowUp;
    app_button_state ArrowDown;
    app_button_state ArrowRight;
    app_button_state ArrowLeft;
    
    app_button_state CapsButton;
    app_button_state BackspaceButton;
    app_button_state SpaceButton;
    app_button_state ShiftButton;
    app_button_state AltButton;
    union
    {
        app_button_state FButtons[12];
        struct
        {
            app_button_state ButtonF1;
            app_button_state ButtonF2;
            app_button_state ButtonF3;
            app_button_state ButtonF4;
            app_button_state ButtonF5;
            app_button_state ButtonF6;
            app_button_state ButtonF7;
            app_button_state ButtonF8;
            app_button_state ButtonF9;
            app_button_state ButtonF10;
            app_button_state ButtonF11;
            app_button_state ButtonF12;
        };
    };
    union
    {
        app_button_state NumbersButtons[10];
        struct
        {
            app_button_state Button0;
            app_button_state Button1;
            app_button_state Button2;
            app_button_state Button3;
            app_button_state Button4;
            app_button_state Button5;
            app_button_state Button6;
            app_button_state Button7;
            app_button_state Button8;
            app_button_state Button9;
        };
    };
    union
    {
        app_button_state AlphaButtons[26];
        struct
        {
            app_button_state ButtonA;
            app_button_state ButtonB;
            app_button_state ButtonC;
            app_button_state ButtonD;
            app_button_state ButtonE;
            app_button_state ButtonF;
            app_button_state ButtonG;
            app_button_state ButtonH;
            app_button_state ButtonI;
            app_button_state ButtonJ;
            app_button_state ButtonK;
            app_button_state ButtonL;
            app_button_state ButtonM;
            app_button_state ButtonN;
            app_button_state ButtonO;
            app_button_state ButtonP;
            app_button_state ButtonQ;
            app_button_state ButtonR;
            app_button_state ButtonS;
            app_button_state ButtonT;
            app_button_state ButtonU;
            app_button_state ButtonV;
            app_button_state ButtonW;
            app_button_state ButtonX;
            app_button_state ButtonY;
            app_button_state ButtonZ;
        };
    };

    bool32 CapsOn;
    bool32 TextErase;
    char TextInput[64];
    u32 TextInputCount;
    u32 TimeToTextInput;
    u32 TimeToTextErase;
};

inline app_controller_input *GetController(app_input* input, int controllerIndex)
{
    Assert(controllerIndex < ArrayCount(input->controllers));

    app_controller_input *result = &input->controllers[0];
    return result;
}

#if 0
struct app_memory_block_header
{
    u32 Size;
    u32 FreeBlockIndex;
    void *Previous; 
};
inline app_memory_block_header *
AppMemoryGetBlockHeader(void *Block)
{
    return (app_memory_block_header *)Block - 1;
}
#endif

struct app_memory
{
    bool32 isInitialized;
    u32 PermanentStorageSize;
    void *PermanentStorage; // NOTE(zoubir): Required to be set to 0 at startup
    u32 PermanentStorageOffset;
#if 0
    app_memory_block_header **PermanentStorageFreeBlocks;
    u32 PermanentStorageFreeBlocksCount;
    app_memory_block_header *PermanentStorageNextFreeBlockHeader;
#endif
    
    size_t TransientStorageSize;
    size_t TransientStorageOffset;
    void *TransientStorage;  // NOTE(zoubir): Required to be set to 0 at startup

    debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;

    platform_add_work_entry *PlatformAddWorkEntry;
};

#define APP_UPDATE_AND_RENDER(name)void name(thread_context *Thread, app_memory *Memory, app_input *Input, app_window *Window)
typedef APP_UPDATE_AND_RENDER(app_update_and_render);
internal APP_UPDATE_AND_RENDER(AppUpdateAndRenderStub)
{
}

//NOTE has to be a very fast function
#define APP_GET_SOUND_SAMPLES(name) void name(thread_context *Thread, app_memory *Memory, app_sound_output_buffer *SoundBuffer)
typedef APP_GET_SOUND_SAMPLES(app_get_sound_samples);
internal APP_GET_SOUND_SAMPLES(AppGetSoundSamplesStub)
{
}


#endif
