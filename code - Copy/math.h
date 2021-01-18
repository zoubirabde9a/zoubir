#if !defined(MATH_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define MATH_H
#include "intrinsics.h"

struct v2
{
    union
    {
        float Data[2];
        struct
        {
            float X, Y;
        };
    };    
};

struct v3
{
    union
    {
        float Data[3];
        struct
        {
            float X, Y, Z;
        };
        struct
        {
            v2 XY;
            float Ignored_;
        };
        struct
        {
            float Ignored_;
            v2 YZ;
        };
    };    
};

struct v4
{
    union
    {
        float Data[4];
        struct
        {
            float X, Y, Z, W;
        };
    };
};

inline v2
operator*(float A, v2 B)
{
    v2 Result;

    Result.X = A * B.X;
    Result.Y = A * B.Y;
    
    return Result;
}

inline v2
operator*(v2 A, float B)
{
    v2 Result;

    Result.X = B * A.X;
    Result.Y = B * A.Y;
    
    return Result;
}

inline v2 &
operator*=(v2& A, float B)
{
    A = B * A;

    return A;
}

inline v2
operator-(v2 A)
{
    v2 Result;
    
    Result.X = -A.X;
    Result.Y = -A.Y;
    
    return Result;
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    
    return Result;
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    
    return Result;
}

inline v2
operator-(v2 A, float B)
{
    v2 Result;
    
    Result.X = A.X - B;
    Result.Y = A.Y - B;
    
    return Result;
}

inline v2&
operator-=(v2& A, v2 B)
{
    A = A - B;
    
    return A;
}






inline v3
operator*(float A, v3 B)
{
    v3 Result;

    Result.X = A * B.X;
    Result.Y = A * B.Y;
    Result.Z = A * B.Z;
    
    return Result;
}

inline v3
operator*(v3 A, float B)
{
    v3 Result;

    Result.X = B * A.X;
    Result.Y = B * A.Y;
    Result.Z = B * A.Z;
    
    return Result;
}

inline v3 &
operator*=(v3& A, float B)
{
    A = B * A;

    return A;
}

inline v3
operator-(v3 A)
{
    v3 Result;
    
    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;
    
    return Result;
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;
    
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    
    return Result;
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result;
    
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    
    return Result;
}

inline v3
operator-(v3 A, float B)
{
    v3 Result;
    
    Result.X = A.X - B;
    Result.Y = A.Y - B;
    Result.Z = A.Z - B;
    
    return Result;
}

inline v3&
operator-=(v3& A, v3 B)
{
    A = A - B;
    
    return A;
}



struct mat4
{
    float Data[16];    
};


inline mat4
OrthoMatrix(float Left, float Right,
                 float Bottom, float Top,
                 float Near, float Far)
{
    mat4 Result =
        {
            2.f / (Right - Left), 0.f, 0.f, 0.f, 
            0.f, 2.f / (Top - Bottom), 0.f, 0.f, 
            0.f, 0.f, -2 / (Far - Near), 0.f, 
            -(Right + Left) / (Right - Left), -(Top + Bottom)/(Top - Bottom), (Far + Near)/(Far - Near), 1.f
        };

    return Result;
}

inline float
SquareRoot(float A)
{
    float Result = sqrtf(A);
    return Result;
}

inline float
Square(float A)
{
    float Result = A * A;
    return Result;
}

inline float
DotProduct(v2 A, v2 B)
{
    float Result = A.X * B.X + A.Y * B.Y;
    return Result;
}

inline float
LengthSq(v2 A)
{
    float Result = DotProduct(A, A);
    return Result;
}

inline float
Length(v2 A)
{
    float Result = SquareRoot(LengthSq(A));
    return Result;
}

//
//
//

inline float
DotProduct(v3 A, v3 B)
{
    float Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
    return Result;
}

inline float
LengthSq(v3 A)
{
    float Result = DotProduct(A, A);
    return Result;
}

inline float
Length(v3 A)
{
    float Result = SquareRoot(LengthSq(A));
    return Result;
}

#endif
