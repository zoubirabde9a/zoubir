#if !defined(OPENGL_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define OPENGL_H
#include "gl/gl.h"
#include <stddef.h>

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_COMPILE_STATUS                 0x8B81
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
 
typedef GLuint type_glDrawArrays(GLenum Mode,
                                 GLint First,
                                 GLsizei Count);
typedef GLuint type_glCreateProgram(void);
typedef void type_glGenerateMipmap(GLenum Target);
typedef void type_glGenVertexArrays(GLsizei N,
                                    GLuint *Arrays);
typedef void type_glGenBuffers(GLsizei N,
                               GLuint *Buffers);
typedef void type_glVertexAttribPointer(GLuint Index,
                                        GLint Size,
                                        GLenum Type,
                                        GLboolean Normalized,
                                        GLsizei Stride,
                                        void *Pointer);
typedef void type_glUseProgram(GLuint Program);
typedef void type_glEnableVertexAttribArray(GLuint Index);
typedef GLint type_glGetUniformLocation(GLuint Program,
                                   GLchar *Name);
typedef void type_glUniformMatrix4fv(GLint Location,
                                GLsizei Count,
                                GLboolean transpose,
                                GLfloat *Value);
typedef void type_glDisableVertexAttribArray(GLuint Index);
typedef void type_glBindBuffer(GLenum Target,
                               GLuint Buffer);
typedef void type_glBufferData(GLenum Target,
                               GLsizeiptr Size,
                               void *Data,
                               GLenum Usage);

typedef void type_glBufferSubData(GLenum Target,
                               GLintptr Offset,
                               GLsizeiptr Size,
                               void *Data);

typedef void type_glBindVertexArray(GLuint Array);
typedef void type_glShaderSource(GLuint Shader,
                                 GLsizei Count,
                                 GLchar **String,
                                 GLint *Length);
typedef void type_glCompileShader(GLuint Shader);
typedef void type_glGetShaderiv(GLuint Shader,
                                GLenum PName,
                                GLint *Params);
typedef void type_glGetShaderInfoLog(GLuint Shader,
                                     GLsizei MaxLength,
                                     GLsizei *Length,
                                     GLchar *InfoLog);
typedef void type_glDeleteShader(GLuint Shader);
typedef void type_glUniform1i(GLint Location,
                              GLint V0);

typedef GLuint type_glCreateShader(GLenum ShaderType);
typedef void type_glBindAttribLocation(GLuint Program,
                                       GLuint Index,
                                       GLchar *Name);
typedef void type_glAttachShader(GLuint Program,
                                 GLuint Shader);
typedef void type_glLinkProgram(GLuint Program);
typedef void type_glGetProgramiv(GLuint Program,
                                 GLenum PName,
                                 GLint *Params);
typedef void type_glGetProgramInfoLog(GLuint Program,
                                      GLsizei MaxLength,
                                      GLsizei *Length,
                                      GLchar *InfoLog);
typedef void type_glDeleteProgram(GLuint Program);
typedef void type_glDetachShader(GLuint Program,
                                 GLuint Shader);

#define OpenGLFunction(Name) type_##Name *Name
struct open_gl
{
    
    OpenGLFunction(glDrawArrays);
    OpenGLFunction(glCreateProgram);
    OpenGLFunction(glGenerateMipmap);
    OpenGLFunction(glGenVertexArrays);
    OpenGLFunction(glBindVertexArray);
    OpenGLFunction(glGenBuffers);
    OpenGLFunction(glVertexAttribPointer); 
    OpenGLFunction(glUseProgram);
    OpenGLFunction(glEnableVertexAttribArray);
    OpenGLFunction(glGetUniformLocation);
    OpenGLFunction(glUniformMatrix4fv);    
    OpenGLFunction(glDisableVertexAttribArray);    
    OpenGLFunction(glBindBuffer);
    OpenGLFunction(glBufferData);
    OpenGLFunction(glBufferSubData);
    OpenGLFunction(glShaderSource);
    OpenGLFunction(glCompileShader);
    OpenGLFunction(glGetShaderiv);
    OpenGLFunction(glGetShaderInfoLog);
    OpenGLFunction(glDeleteShader);
    OpenGLFunction(glUniform1i);
    OpenGLFunction(glCreateShader);
    OpenGLFunction(glBindAttribLocation);
    OpenGLFunction(glAttachShader);
    OpenGLFunction(glLinkProgram);
    OpenGLFunction(glGetProgramiv);
    OpenGLFunction(glGetProgramInfoLog);
    OpenGLFunction(glDeleteProgram);
    OpenGLFunction(glDetachShader);
       
};
#endif
