/* date = October 12th 2022 0:13 pm */

#ifndef WIN32_OPENGL_H

#define GLDefineGlobalFunction(Name) type_##Name * Name

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

typedef GLuint WINAPI type_glCreateShader(GLenum type);
typedef void WINAPI type_glShaderSource(GLuint Shader, 
                                        GLsizei count, 
                                        GLchar** string, const GLint *Length);
typedef void WINAPI type_glCompileShader(GLuint Shader);
typedef void WINAPI type_glGetShaderiv(GLuint shader,
                                       GLenum pname,
                                       GLint *params);
typedef void WINAPI type_glGetShaderInfoLog(GLuint shader,
                                            GLsizei maxLength,
                                            GLsizei *length,
                                            GLchar *infoLog);
typedef GLuint WINAPI type_glCreateProgram();
typedef void WINAPI type_glAttachShader(GLuint program,
                                        GLuint shader);
typedef void WINAPI type_glLinkProgram(GLuint program);
typedef void WINAPI type_glGetProgramiv(GLuint program,
                                        GLenum pname,
                                        GLint *params);
typedef void WINAPI type_glGetProgramInfoLog(GLuint program,
                                             GLsizei maxLength,
                                             GLsizei *length,
                                             GLchar *infoLog);
typedef void WINAPI type_glGenVertexArrays(GLsizei n,
                                           GLuint *arrays);
typedef void WINAPI type_glBindVertexArray(GLuint array);
typedef void WINAPI type_glGenBuffers(GLsizei n,
                                      GLuint *buffers);
typedef GLint WINAPI type_glGetAttribLocation(GLuint program,
                                              const GLchar *name);
typedef void WINAPI type_glEnableVertexAttribArray(GLuint index);
typedef void WINAPI type_glDisableVertexAttribArray(GLuint index);
typedef void WINAPI type_glVertexAttribPointer(GLuint index,
                                               GLint size,
                                               GLenum type,
                                               GLboolean normalized,
                                               GLsizei stride,
                                               const GLvoid *pointer);

typedef void WINAPI type_glVertexAttribIPointer(GLuint index,
                                                GLint size,
                                                GLenum type,
                                                GLsizei stride,
                                                const GLvoid * pointer);
typedef void WINAPI type_glUseProgram(GLuint program);
typedef GLint WINAPI type_glGetUniformLocation(GLuint program,
                                               const GLchar *name);
typedef void WINAPI type_glTexImage3D(GLenum target,
                                      GLint level,
                                      GLint internalFormat,
                                      GLsizei width,
                                      GLsizei height,
                                      GLsizei depth,
                                      GLint border,
                                      GLenum format,
                                      GLenum type,
                                      const GLvoid * data);
typedef void WINAPI type_glTexSubImage3D(GLenum target,
                                         GLint level,
                                         GLint xoffset,
                                         GLint yoffset,
                                         GLint zoffset,
                                         GLsizei width,
                                         GLsizei height,
                                         GLsizei depth,
                                         GLenum format,
                                         GLenum type,
                                         const GLvoid * data);
typedef void WINAPI type_glBindBuffer(GLenum target,
                                      GLuint buffer);
typedef void WINAPI type_glBufferData(GLenum target,
                                      GLsizeiptr size,
                                      const GLvoid *data,
                                      GLenum usage);
typedef void WINAPI type_glUniformMatrix4fv(GLint location,
                                            GLsizei count,
                                            GLboolean transpose,
                                            const GLfloat *value);
typedef void WINAPI type_glUniform1i(GLint location, GLint v0);
typedef void WINAPI type_glUniform3f(GLint location,
                                     GLfloat v0,
                                     GLfloat v1,
                                     GLfloat v2);


typedef BOOL WINAPI wgl_swap_interval_ext(int interval);
typedef const char * WINAPI wgl_get_extensions_string_ext(void);

global_variable wgl_swap_interval_ext *wglSwapIntervalEXT;
global_variable wgl_get_extensions_string_ext *wglGetExtensionsStringEXT;

GLDefineGlobalFunction(glCreateShader);
GLDefineGlobalFunction(glShaderSource);
GLDefineGlobalFunction(glCompileShader);
GLDefineGlobalFunction(glGetShaderiv);
GLDefineGlobalFunction(glGetShaderInfoLog);
GLDefineGlobalFunction(glCreateProgram);
GLDefineGlobalFunction(glAttachShader);
GLDefineGlobalFunction(glLinkProgram);
GLDefineGlobalFunction(glGetProgramiv);
GLDefineGlobalFunction(glGetProgramInfoLog);
GLDefineGlobalFunction(glGenVertexArrays);
GLDefineGlobalFunction(glBindVertexArray);
GLDefineGlobalFunction(glGenBuffers);
GLDefineGlobalFunction(glGetAttribLocation);
GLDefineGlobalFunction(glEnableVertexAttribArray);
GLDefineGlobalFunction(glVertexAttribPointer);
GLDefineGlobalFunction(glVertexAttribIPointer);
GLDefineGlobalFunction(glUseProgram);
GLDefineGlobalFunction(glGetUniformLocation);
GLDefineGlobalFunction(glTexImage3D);
GLDefineGlobalFunction(glTexSubImage3D);
GLDefineGlobalFunction(glBindBuffer);
GLDefineGlobalFunction(glBufferData);
GLDefineGlobalFunction(glUniformMatrix4fv);
GLDefineGlobalFunction(glUniform1i);
GLDefineGlobalFunction(glUniform3f);

#define WIN32_OPENGL_H

#endif //WIN32_OPENGL_H
