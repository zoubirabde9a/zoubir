/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */

#include <SDL\SDL.h>
//#undef main
#include <emscripten\emscripten.h>
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>

#include "app_platform.h"
#include "app.cpp"


DEBUG_PLATFORM_READ_ENTIRE_FILE(WebReadEntireFile)
{
    debug_read_file_result Result = {};

    FILE *File = fopen(fileName, "rb");
    if (File)
    {
        fseek(File, 0l, SEEK_END);
        u32 Size = ftell(File);
        fseek(File, 0l, SEEK_SET);
        void *Memory = malloc(Size);
        fread(Memory, Size, 1, File);

        Result.Memory = Memory;
        Result.Size = Size;        
    }
    
    return Result;
}
DEBUG_PLATFORM_FREE_FILE_MEMORY(WebFreeFileMemory)
{
    free(memory);
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(WebWriteEntireFile)
{    
    bool32 Result = 0;

    FILE *File = fopen(fileName, "wb+");
    if (File)
    {
        Result = (fwrite(memory, memorySize, 1, File) == memorySize);
    }
    
    return Result;
}

    #if 1


global_variable thread_context *Thread;
global_variable app_memory AppMemory;
global_variable app_input NewInput;
global_variable app_input OldInput;
global_variable int WindowWidth;
global_variable int WindowHeight;

internal void
MainLoop()
{
    app_window AppWindow;
    AppWindow.Width = WindowWidth;
    AppWindow.Height = WindowHeight;
    AppUpdateAndRender(Thread, &AppMemory, &NewInput, &AppWindow);
}
    #endif

int main()
{
    int Width, Height;
    SDL_Window *Window;
    SDL_GLContext Context;
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    Window = SDL_CreateWindow("randomWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 768, SDL_WINDOW_OPENGL);
    Context = SDL_GL_CreateContext(Window);
    if (!Context) {
    }
    
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    SDL_GetWindowSize(Window, &WindowWidth, &WindowHeight);

    thread_context ThreadContext = {};
    thread_context *Thread = &ThreadContext;
#if 0
    AppMemory.PermanentStorageSize = Megabytes(64);
    AppMemory.TransientStorageSize = Megabytes(8);
    
    AppMemory.PermanentStorageSize;
    AppMemory.PermanentStorage; // NOTE(zoubir): Required to be set to 0 at startup
    AppMemory.PermanentStorageOffset;
    
    AppMemory.TransientStorageSize;
    AppMemory.TransientStorageOffset;
    AppMemory.TransientStorage;  // NOTE(zoubir): Required to be set to 0 at startup

    AppMemory = ;
    NewInput = ;
#endif

//    emscripten_set_main_loop(MainLopp, 60, true);
}
         
