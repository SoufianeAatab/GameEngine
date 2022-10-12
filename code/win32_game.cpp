#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "platform.h"

#include <gl/gl.h>
#include "win32_game.h"

#include "game.cpp"

void DebugPrint(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char szBuffer[128]; // get rid of this hard-coded buffer
    int nBuf = vsnprintf_s(szBuffer, 127, fmt, args);
    OutputDebugStringA(szBuffer);
    va_end(args);
}


#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0X2092
#define WGL_CONTEXT_FLAGS_ARB 0X2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_RED_BITS_ARB 0x2015
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023

typedef HGLRC WINAPI wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext, const int *attribList);

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
                                                 const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);



// OPENGL FUNCTIONS
#include "win32_opengl.h"
#include "renderer_opengl.cpp"


// END OPENGL FUNCTIONS






global_variable wgl_create_context_attribs_arb *wglCreateContextAttribsARB;
global_variable wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

global_variable u64 GlobalPerfCounterFrequency;

global_variable platform_api Platform;
global_variable bool GlobalRunning = true;


// TODO CHANGE THIS
internal void Win32ProcessMessages(game_keyboard *Input)
{
    MSG Message = {};
    
    while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch (Message.message)
        {
            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                u32 VKCode = (u32)Message.wParam;
                bool WasDown = (Message.lParam & (1 << 30)) != 0;
                bool IsDown = (Message.lParam & (1 << 31)) == 0;
                
                if (WasDown != IsDown)
                {
                    switch (VKCode)
                    {
                        case VK_F4:
                        {
                            bool IsAltDown = Message.lParam & (1 << 29);
                            if (IsAltDown)
                            {
                                GlobalRunning = false;
                            }
                        }
                        break;
                        case 'W':
                        {
                            if (Input->W.IsDown != IsDown)
                            {
                                Input->W.IsDown = IsDown;
                            }
                        }
                        break;
                        case 'S':
                        {
                            if (Input->S.IsDown != IsDown)
                            {
                                Input->S.IsDown = IsDown;
                            }
                        }
                        break;
                        
                        case 'A':
                        {
                            if (Input->A.IsDown != IsDown)
                            {
                                Input->A.IsDown = IsDown;
                            }
                        }
                        break;
                        
                        case 'D':
                        {
                            if (Input->D.IsDown != IsDown)
                            {
                                Input->D.IsDown = IsDown;
                            }
                        }
                        break;
                        
                        case 'Y':
                        {
                            if (Input->Y.IsDown != IsDown)
                            {
                                Input->Y.IsDown = IsDown;
                            }
                        }
                        break;
                        
                        case 'U':
                        {
                            if (Input->U.IsDown != IsDown)
                            {
                                Input->U.IsDown = IsDown;
                            }
                        }
                        break;
                        
                        case 'O':
                        {
                            if (Input->O.IsDown != IsDown)
                            {
                                Input->O.IsDown = IsDown;
                            }
                        }
                        break;
                        case 'P':
                        {
                            if (Input->P.IsDown != IsDown)
                            {
                                Input->P.IsDown = IsDown;
                            }
                        }
                        break;
                    }
                }
            }
            break;
            case WM_MOUSEWHEEL:
            {
                // fwKeys = GET_KEYSTATE_WPARAM(wParam);
                // Input->MouseZ = GET_WHEEL_DELTA_WPARAM(Message.wParam);
            }
            break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            break;
        }
    }
}

internal win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return Result;
}

internal void GLLoadFunctions()
{
#define Win32GetOpenGLFunction(Name) Name = (type_##Name*) wglGetProcAddress(#Name);
    
    Win32GetOpenGLFunction(glCreateShader);
    Win32GetOpenGLFunction(glShaderSource);
    Win32GetOpenGLFunction(glCompileShader);
    Win32GetOpenGLFunction(glGetShaderiv);
    Win32GetOpenGLFunction(glGetShaderInfoLog);
    Win32GetOpenGLFunction(glCreateProgram);
    Win32GetOpenGLFunction(glAttachShader);
    Win32GetOpenGLFunction(glLinkProgram);
    Win32GetOpenGLFunction(glGetProgramiv);
    Win32GetOpenGLFunction(glGetProgramInfoLog);
    Win32GetOpenGLFunction(glGenVertexArrays);
    Win32GetOpenGLFunction(glBindVertexArray);
    Win32GetOpenGLFunction(glGenBuffers);
    Win32GetOpenGLFunction(glGetAttribLocation);
    Win32GetOpenGLFunction(glEnableVertexAttribArray);
    Win32GetOpenGLFunction(glVertexAttribPointer);
    Win32GetOpenGLFunction(glVertexAttribIPointer);
    Win32GetOpenGLFunction(glUseProgram);
    Win32GetOpenGLFunction(glGetUniformLocation);
    Win32GetOpenGLFunction(glTexImage3D);
    Win32GetOpenGLFunction(glTexSubImage3D);
    Win32GetOpenGLFunction(glBindBuffer);
    Win32GetOpenGLFunction(glBufferData);
    Win32GetOpenGLFunction(glUniformMatrix4fv);
    Win32GetOpenGLFunction(glUniform1i);
    Win32GetOpenGLFunction(glUniform3f);
    
    bool VSyncEnabled = true;
    if(!wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT = (wgl_swap_interval_ext *)wglGetProcAddress("wglSwapIntervalEXT");
    }
    
    if(wglSwapIntervalEXT)
    {
        OutputDebugStringA("Enabling SWAP INTERVAL\n");
        wglSwapIntervalEXT(VSyncEnabled ? 1 : 0);
    }
    
}

internal bool Win32CreateGLContext(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);
    
    // this set the whole struct to 0
    PIXELFORMATDESCRIPTOR pfd = {};
    
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cStencilBits = 8;
    
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    
    HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);
    
    wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)wglGetProcAddress("wglCreateContextAttribsARB");
    
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type *)wglGetProcAddress("wglChoosePixelFormatARB");
    
    const int attribList[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        3,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        3,
        WGL_CONTEXT_FLAGS_ARB,
        0,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
        
    };
    int nPixelFormat2;
    
    UINT nMaxFormats = 1;
    UINT nNumFormats;
    float pfAttribFList[] = {0, 0};
    
    int piAttribIList[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_COLOR_BITS_ARB, 32,
        WGL_RED_BITS_ARB, 8,
        WGL_GREEN_BITS_ARB, 8,
        WGL_BLUE_BITS_ARB, 8,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 16,
        WGL_STENCIL_BITS_ARB, 0,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 16,
        0, 0};
    BOOL bValidPixFormat;
    
    bValidPixFormat = wglChoosePixelFormatARB(hdc, piAttribIList, pfAttribFList, nMaxFormats, &nPixelFormat2, &nNumFormats);
    
    HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
    wglMakeCurrent(hdc, hglrc);
    
    
    GLLoadFunctions();
    
    return true;
}

internal LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_CREATE:
        {
            if (!Win32CreateGLContext(Window))
            {
                // Or better yet, use the GL3 way to get the version number
                MessageBox(Window, "CANT CREATE CONTEXT", "YO Q SE", MB_OKCANCEL);
            }
        }
        break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }
        break;
        
        case WM_CLOSE:
        {
            GlobalRunning = false;
            OutputDebugStringA("WM_CLOSE\n");
        }
        break;
        
        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        }
        break;
        
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            Assert(!"no keyboard message here")
        }
        break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct = {};
            HDC DeviceContext = BeginPaint(Window, &PaintStruct);
            EndPaint(Window, &PaintStruct);
        }
        break;
        
        case WM_SIZE:
        {
            glViewport(0, 0, LOWORD(LParam), HIWORD(LParam));
            OutputDebugStringA("WM_SIZE\n");
        }
        break;
        
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        }
        break;
    }
    
    return Result;
}

// API starts here
read_file_result ReadFile(char *FileName)
{
    read_file_result Result = {};
    
    HANDLE File = CreateFileA(
                              FileName,
                              GENERIC_READ,
                              0,
                              0,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              0);
    
    LARGE_INTEGER FileSize;
    
    if (File != INVALID_HANDLE_VALUE)
    {
        if (GetFileSizeEx(File, &FileSize))
        {
            i32 FileSize32 = FileSize.QuadPart;
            void *Memory = VirtualAlloc(0, FileSize32, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (Memory)
            {
                DWORD BytesRead;
                if (ReadFile(File, Memory, FileSize32, &BytesRead, 0) && BytesRead == FileSize32)
                {
                    Result.Size = FileSize32;
                    Result.Data = Memory;
                }
                else
                {
                    // TODO: logging
                }
            }
            else
            {
                // TODO: logging
            }
        }
        else
        {
            // TODO: logging
        }
        
        CloseHandle(File);
    }
    else
    {
        // TODO: logging
        DebugPrint("Can`t Open File: %s\n", FileName);
    }
    
    return Result;
}

void FreeFile(read_file_result File)
{
    VirtualFree(File.Data, File.Size, MEM_RELEASE);
}

void* AllocMem(u32 Size)
{
    return VirtualAlloc(0,Size,MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void FreeMem(void* Data, u32 Size)
{
    VirtualFree(Data, Size, MEM_RELEASE);
}

void CopyMem(void* Dest, void* Source, u32 Size)
{
    CopyMemory(Dest, Source, Size);
}

/// API ENDS HERE 

inline u64 Win32GetPerfCounter()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result.QuadPart;
}

inline f32 Win32GetSecondsElapsed(u64 Start, u64 End)
{
    f32 Result = 0;
    Result = (f32)(End - Start) / (f32)GlobalPerfCounterFrequency;
    return Result;
}

internal void Win32InitGameMemory(game_memory* Memory, platform_api* Api)
{
    LPVOID BaseAddress = 0;
    Memory->PermanentStorageSize = Megabytes(16);
    Memory->TransientStorageSize = Megabytes(16);
    Memory->PermanentStorage = VirtualAlloc(BaseAddress,
                                            (size_t)(Memory->PermanentStorageSize + Memory->TransientStorageSize),
                                            MEM_COMMIT | MEM_RESERVE,
                                            PAGE_READWRITE);
    Memory->TransientStorage = (u8 *)Memory->PermanentStorage + Memory->PermanentStorageSize;
    
    Memory->Platform = Api;
    
}

internal void Win32InitPlatformApi(platform_api* Api)
{
    Api->ReadFile = ReadFile;
    Api->FreeFile = FreeFile;
    Api->AllocMem = AllocMem;
    Api->FreeMem = FreeMem;
    Api->CopyMem = CopyMem;
    Api->DebugPrint = DebugPrint;
}

int CALLBACK
WinMain(
        HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CmdLine,
        int ShowCmd)
{
#if 1
    // Register the window class.
    WNDCLASS WindowClass = {};
    
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    
    if (RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(
                                     0,
                                     "HandmadeHeroWindowClass",
                                     "Handmade Hero",
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     0,
                                     0,
                                     Instance,
                                     0);
        if (Window)
        {
            HDC HandleDc = GetDC(Window);
            
            Api = {};
            Win32InitPlatformApi(&Api);
            
            game_memory Memory = {};
            Win32InitGameMemory(&Memory, &Api);
            
            opengl OpenGL = OpenglInit();
            OpenGL.VertexArray =  (vertex *) AllocMem(Megabytes(8));
            
            OpenGLInitTexturesQueue(&OpenGL);
            OpenGLInitMeshesQueue(&OpenGL);
            OpenGLInitRenderCommands(&OpenGL);
            
            // GlobalGameVideo.OpenGL = OpenGL;
            
            if (Memory.PermanentStorage)
            {
                win32_state Win32State = {};
                
                HDC DeviceContext = GetDC(Window);
                int MonitorRefreshHz = GetDeviceCaps(DeviceContext, VREFRESH);
                int GameUpdateHz = MonitorRefreshHz;
                
                // HALF VREFRESH 30FPS
                f32 TargetFPS = GameUpdateHz;
                f32 TargetSecondsPerFrame = 1.0f / TargetFPS;
                b32 SleepIsGranular = timeBeginPeriod(1) == TIMERR_NOERROR;
                
                LARGE_INTEGER CounterPerSecond;
                QueryPerformanceFrequency(&CounterPerSecond);
                GlobalPerfCounterFrequency = CounterPerSecond.QuadPart;
                u64 LastCounter = Win32GetPerfCounter();
                
                game_input Input[2] = {};
                game_input *OldInput = &Input[0];
                game_input *NewInput = &Input[1];
                
                f32 PrevDT = TargetSecondsPerFrame;
                while (GlobalRunning)
                {
                    game_keyboard *NewKeyboardController = &NewInput->Keys;
                    game_keyboard *OldKeyboardController = &OldInput->Keys;
                    *NewKeyboardController = {};
                    
                    // NewKeyboardController->IsConnected = true;
                    
                    for (int i = 0; i < ArrayCount(NewKeyboardController->Buttons); i++)
                    {
                        NewKeyboardController->Buttons[i].IsDown = OldKeyboardController->Buttons[i].IsDown;
                    }
                    
                    Win32ProcessMessages(NewKeyboardController);
                    {
                        POINT Point;
                        GetCursorPos(&Point);
                        ScreenToClient(Window, &Point);
                        NewInput->MouseX = i32(Point.x);
                        NewInput->MouseY = i32(Point.y);
                        
                        NewInput->MouseButtons[0].IsDown = GetKeyState(VK_LBUTTON) & (1 << 15);
                        NewInput->MouseButtons[1].IsDown = GetKeyState(VK_RBUTTON) & (1 << 15);
                        NewInput->MouseButtons[2].IsDown = GetKeyState(VK_MBUTTON) & (1 << 15);
                        NewInput->MouseButtons[3].IsDown = GetKeyState(VK_XBUTTON1) & (1 << 15);
                        NewInput->MouseButtons[4].IsDown = GetKeyState(VK_XBUTTON2) & (1 << 15);
                    }
                    
                    //NewInput->Dt = TargetSecondsPerFrame;
                    NewInput->Dt = PrevDT;
                    
                    win32_window_dimension Dim = Win32GetWindowDimension(Window);
                    game_render_commands* Commands = OpenGLBeginFrame(&OpenGL, Dim.Width, Dim.Height);
                    
                    GameUpdateVideo(&Memory, Commands, NewInput);
                    
                    OpenGLEndFrame(Commands, &OpenGL);
                    
                    SwapBuffers(HandleDc);
                    
                    f32 SecondsElapsed = Win32GetSecondsElapsed(LastCounter, Win32GetPerfCounter());
                    /*if (SecondsElapsed < TargetSecondsPerFrame)
                    {
                        if (SleepIsGranular)
                        {
                            DWORD SleepMS = (DWORD)(1000 * (TargetSecondsPerFrame - SecondsElapsed));
                            if (SleepMS > 0)
                            {
                                Sleep(SleepMS);
                            }
                        }
                        
                        while (SecondsElapsed < TargetSecondsPerFrame)
                        {
                            SecondsElapsed = Win32GetSecondsElapsed(LastCounter, Win32GetPerfCounter());
                        }
                    }
                    else
                    {
                        // We missed a frame
                        // TODO: logging
                    }*/
                    PrevDT = SecondsElapsed;
                    u64 EndCounter = Win32GetPerfCounter();
                    LastCounter = EndCounter;
                    
                    game_input *Tmp = OldInput;
                    OldInput = NewInput;
                    NewInput = Tmp;
                }
            }
        }
    }
    else
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
    }
#else
    
#endif
    
    // clock_t begin = clock();
    
    // clock_t end = clock();
    // double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    // DebugPrint("Time Spent: %f\n", time_spent);
}
