/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: zoubir $
   ======================================================================== */
#include "opengl.h"

void *GetGLFuncAddress(HMODULE OpenglDLL, char *name)
{
  void *Result = (void *)wglGetProcAddress(name);
  
  if(OpenglDLL &&
     (Result == 0 ||
     (Result == (void*)0x1) ||
     (Result == (void*)0x2) ||
     (Result == (void*)0x3) ||
      (Result == (void*)-1)))
  {
//      Result = (void *)GetProcAddress(OpenglDLL, name);
  }

  return Result;
}

void LoadOpenglFunctions(open_gl *OpenGL)
{
    HMODULE OpenglDLL = LoadLibraryA("opengl32.dll");

#define GetOpenglFunction(Name) OpenGL->Name = (type_##Name *)GetGLFuncAddress(OpenglDLL, #Name)
    
    GetOpenglFunction(glCreateProgram);
    GetOpenglFunction(glGenerateMipmap);
    GetOpenglFunction(glGenVertexArrays);
    GetOpenglFunction(glGenBuffers);
    GetOpenglFunction(glUseProgram);
    GetOpenglFunction(glVertexAttribPointer); 
    GetOpenglFunction(glEnableVertexAttribArray);
    GetOpenglFunction(glGetUniformLocation);
    GetOpenglFunction(glUniformMatrix4fv);
    GetOpenglFunction(glBindBuffer);
    GetOpenglFunction(glBufferData);
    GetOpenglFunction(glBufferSubData);
    GetOpenglFunction(glBindVertexArray);
    GetOpenglFunction(glShaderSource);
    GetOpenglFunction(glCompileShader);
    GetOpenglFunction(glGetShaderiv);
    GetOpenglFunction(glGetShaderInfoLog);
    GetOpenglFunction(glDeleteShader);
    GetOpenglFunction(glUniform1i);
    GetOpenglFunction(glCreateShader);
    GetOpenglFunction(glBindAttribLocation);
    GetOpenglFunction(glAttachShader);
    GetOpenglFunction(glLinkProgram);
    GetOpenglFunction(glGetProgramiv);
    GetOpenglFunction(glGetProgramInfoLog);
    GetOpenglFunction(glDeleteProgram);
    GetOpenglFunction(glDetachShader);

    if (OpenglDLL)
    {
        FreeLibrary(OpenglDLL);
    }
}
