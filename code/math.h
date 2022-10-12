#pragma once
#include "platform.h"
#define M_PI 3.141549

inline f32 Radians(f32 Degrees)
{
    return Degrees / 180.0f * M_PI; // degree to rad
}

union v2{
    struct 
    {
        f32 x;
        f32 y;
    };
    f32 Data[2];
};
union v3{
    struct 
    {
        f32 x;
        f32 y;
        f32 z; 
    };
    struct 
    {
        f32 r;
        f32 g;
        f32 b; 
    };
    v2 xy;
    f32 Data[3];
};


union v4{
    struct 
    {
        f32 x;
        f32 y;
        f32 z; 
        f32 w;
    };
    struct 
    {
        f32 r;
        f32 g;
        f32 b; 
        f32 a;
    };
    
    v3 xyz;
    
    f32 Data[3];
};



union mat4
{
    struct
    {
        v4 A;
        v4 B;
        v4 C;
        v4 D;
    };
    f32 M[4][4];
    f32 Data[16];
};

inline v4 V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;
    
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;
    
    return Result;
}

inline v4 V4(v3 A, f32 W)
{
    v4 Result;
    
    Result.x = A.x;
    Result.y = A.y;
    Result.z = A.z;
    Result.w = W;
    
    return Result;
}

inline v3 V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;
    
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    
    return Result;
}

inline v3 V3(f32 X)
{
    v3 Result;
    
    Result.x = X;
    Result.y = X;
    Result.z = X;
    
    return Result;
}

inline v2 V2(f32 X, f32 Y)
{
    v2 Result;
    
    Result.x = X;
    Result.y = Y;
    
    return Result;
}

inline v2 V2(f32 X)
{
    v2 Result;
    
    Result.x = X;
    Result.y = X;
    
    return Result;
}

inline v2 operator+(v2 A, v2 B)
{
    v2 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y; 
    return Result;
} 

inline v3 operator+(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y; 
    Result.z = A.z + B.z; 
    return Result;
} 

inline v3& operator+=(v3 &A, v3 B)
{
    A = A + B;
    return A;
} 

inline v3 operator*(v3 A, f32 m)
{
    v3 Result;
    Result.x = A.x*m;
    Result.y = A.y*m; 
    Result.z = A.z*m; 
    return Result;
} 

inline v3 operator*(f32 m, v3 A)
{
    v3 Result;
    Result.x = A.x*m;
    Result.y = A.y*m; 
    Result.z = A.z*m; 
    return Result;
} 


inline v2 operator*(v2 A, f32 m)
{
    v2 Result;
    Result.x = A.x*m;
    Result.y = A.y*m; 
    return Result;
} 

inline v2 operator*(f32 m, v2 A)
{
    v2 Result;
    Result.x = A.x*m;
    Result.y = A.y*m; 
    return Result;
} 

inline v3 operator-(v3 A, v3 B)
{
    v3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y; 
    Result.z = A.z - B.z; 
    return Result;
} 

inline mat4 operator*(mat4 A, mat4 B)
{
    mat4 Result = {};
    for (i32 i=0;i<4;++i)
    {
        for(i32 j=0;j<4;++j)
        {
            Result.M[i][j]=0;
            for(i32 k=0;k<4;k++)
            {
                Result.M[i][j]+=A.M[i][k]*B.M[k][j];
            }
        }
    }
    return Result;
}

inline v4 operator*(mat4 A, v4 B)
{
    v4 Result = {};
    Result.x = A.M[0][0] * B.x + A.M[0][1] * B.y + A.M[0][2] * B.z + A.M[0][3] * B.w; 
    Result.y = A.M[1][0] * B.x + A.M[1][1] * B.y + A.M[1][2] * B.z + A.M[1][3] * B.w; 
    Result.z = A.M[2][0] * B.x + A.M[2][1] * B.y + A.M[2][2] * B.z + A.M[2][3] * B.w; 
    Result.w = A.M[3][0] * B.x + A.M[3][1] * B.y + A.M[3][2] * B.z + A.M[3][3] * B.w; 
    return Result;
}

inline v4 operator-(v4 B)
{
    v4 Result = {};
    Result.x = -B.x; 
    Result.y = -B.y; 
    Result.z = -B.z; 
    Result.w = B.w; 
    return Result;
}

inline v3 operator-(v3 B)
{
    v3 Result = {};
    Result.x = -B.x; 
    Result.y = -B.y; 
    Result.z = -B.z; 
    return Result;
}

inline mat4 Identity()
{
    mat4 Result = {};
    
    Result.Data[0] = 1;
    Result.Data[5] = 1;
    Result.Data[10] = 1;
    Result.Data[15] = 1;
    
    return Result;
}

inline v3 GetCol(mat4 A, i32 Col){
    v3 Result = {};
    Result.x = A.M[0][Col];
    Result.y = A.M[1][Col];
    Result.z = A.M[2][Col];
    return Result;
}

inline v3 GetRow(mat4 A, i32 Row){
    v3 Result = {};
    Result.x = A.M[Row][0];
    Result.y = A.M[Row][1];
    Result.z = A.M[Row][2];
    return Result;
}

inline mat4 Translate(v3 B)
{
    mat4 Result = Identity();
    Result.M[0][3] = B.x; 
    Result.M[1][3] = B.y; 
    Result.M[2][3] = B.z; 
    Result.M[3][3] = 1; 
    
    return Result;
}

inline mat4 Scale(v3 B)
{
    mat4 Result = Identity();
    Result.M[0][0] = B.x; 
    Result.M[1][1] = B.y; 
    Result.M[2][2] = B.z; 
    Result.M[3][3] = 1; 
    
    return Result;
}

inline mat4 Translate(mat4 A, v3 B)
{
    mat4 Result = A;
    Result.M[0][3] += B.x; 
    Result.M[1][3] += B.y; 
    Result.M[2][3] += B.z; 
    Result.M[3][3] = 1; 
    
    return Result;
}

inline mat4 Transpose(mat4 M)
{
    mat4 Result = {};
    for (i32 i = 0; i < 4; ++i)
    {
        for(i32 j = 0; j < 4; ++j)
        {
            Result.M[j][i] = M.M[i][j];
        }    
    }
    return Result;
}

inline mat4 RotateX(f32 AngleRadians)
{
    f32 a = cosf(AngleRadians);
    f32 b = sinf(AngleRadians);
    
    mat4 Result = {
        1,0,0,0,
        0,a,-b,0,
        0,b, a,0,
        0,0,0,1
    };
    return Result;
}

inline mat4 RotateY(f32 AngleRadians)
{
    f32 a = cosf(AngleRadians);
    f32 b = sinf(AngleRadians);
    mat4 Result = {
        a,0, b,0,
        0,1,0,0,
        -b,0, a,0,
        0,0,0,1
    };
    return Result;
}

inline mat4 RotateZ(f32 AngleRadians)
{
    f32 a = cosf(AngleRadians);
    f32 b = sinf(AngleRadians);
    mat4 Result = {
        a,-b,0,0,
        b,a,0,0,
        0,0,1,0,
        0,0,0,1
    };
    return Result;
}

inline v3
Lerp(v3 A, f32 t, v3 B)
{
    v3 Result = (1.0f - t)*A + t*B;
    
    return(Result);
}

inline mat4 CameraTransform(v3 A, v3 B, v3 C, v3 T)
{
    mat4 Result = {
        A.x, B.x, C.x, 0.0f,
        A.y, B.y, C.y, 0.0f,
        A.z, B.z, C.z, 0.0f,
        0, 0, 0, 1,
    };
    Result = Result * Translate(-T);
    return Result;
}

inline f32 Square(f32 A){
    return A*A;
}


inline f32
Dot(v3 A, v3 B)
{
    f32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
    
    return(Result);
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.y*B.z - A.z*B.y;
    Result.y = A.z*B.x - A.x*B.z;
    Result.z = A.x*B.y - A.y*B.x;
    
    return(Result);
}

inline f32
LengthSq(v3 A)
{
    f32 Result = Dot(A, A);
    
    return(Result);
}

inline f32 
SquareRoot(f32 A)
{
    // TODO(casey): Replace with SSE
    f32 Result = (f32)sqrt(A);
    return(Result);
}

inline v3
Normalize(v3 A)
{
    v3 Result = {};
    
    f32 LenSq = LengthSq(A);
    if(LenSq > Square(0.0001f))
    {
        Result = A * (1.0f / SquareRoot(LenSq));
    }
    
    return(Result);
}

inline mat4 Perspective(f32 AspectRatio, f32 Fov)
{
    // f32 a = 1.0f;
    // f32 b = AspectRatio;
    // f32 c = 1.0; // NOTE(casey): This should really be called "film back distance"
    
    // f32 n = 0.1f; // NOTE(casey): Near clip plane _distance_
    // f32 f = 100.0f; // NOTE(casey): Far clip plane _distance_
    
    // // NOTE(casey): These are the perspective correct terms, for when you divide by -z
    // f32 d = (n+f) / (n-f);
    // f32 e = (2*f*n) / (n-f);
    
    // mat4 Result = {
    //     a*c,0,0,0,
    //     0,b*c,0,0,
    //     0,0,d,e,
    //     0,0,-1,0
    // };
    
    f32 fov = Radians(0.5 * Fov);
    f32 f = 1.0f/tanf(fov);
    //f = 1.5f;
    f32 a = f / AspectRatio;
    f32 Far = 200.0f;
    f32 Near = 0.1f;
    f32 c = (Near + Far) / (Near - Far);
    f32 d = (2 * Far * Near) / (Near - Far);
    mat4 Result = {
        a,0,0,0,
        0,f,0,0,
        0,0,c,d,
        0,0,-1,0
    };
    
    return Result;
}

inline mat4 LookAt(v3 eye, v3 at, v3 up)
{
    v3 zaxis = Normalize(at - eye);    
    v3 xaxis = Normalize(Cross(zaxis, up));
    v3 yaxis = Cross(xaxis, zaxis);
    
    zaxis.x = -zaxis.x;
    zaxis.y = -zaxis.y;
    zaxis.z = -zaxis.z;
    
    mat4 viewMatrix = {
        V4(xaxis.x, xaxis.y, xaxis.z, -Dot(xaxis, eye)),
        V4(yaxis.x, yaxis.y, yaxis.z, -Dot(yaxis, eye)),
        V4(zaxis.x, zaxis.y, zaxis.z, -Dot(zaxis, eye)),
        V4(0, 0, 0, 1)
    };
    
    return viewMatrix;
}

inline mat4 Ortho(f32 Width,f32 Height)
{
    f32 Near = 1.0f;
    f32 Far = 100;
    // -Z points away from camera(but in depth buffer smaller z are consideren closer). We invert the Z coordinate.
    f32 a = 2.0f / Width;
    f32 b = 2.0f / Height; 
    f32 c = -2.0f / (Far - Near);
    f32 d = 1;
    //f32 e = (Height) / (Height);
    f32 e = 1;
    f32 f = (Far + Near) / (Far - Near);
    
    mat4 Result = {
        a,0, 0,-1,
        0,b, 0,-1,
        0,0, c,-f,
        0,0, 0, 1
    };
    
    return Result;
}
