#if !defined(MATH_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define MATH_H
#include "intrinsics.h"

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

#define Absolute(A) ((A > 0) ? (A) : (-A))

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

inline v2
V2(float X, float Y)
{
    v2 Result = {X, Y};
    return Result;
}
inline v2
V2(u32 X, u32 Y)
{
    v2 Result = {(float)X, (float)Y};
    return Result;
}

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
            float Ignored0_;
        };
        struct
        {
            float Ignored1_;
            v2 YZ;
        };
    };    
};

inline v3
V3(float X, float Y, float Z)
{
    v3 Result = {X, Y, Z};
    return Result;
}

inline v3
V3(u32 X, u32 Y, u32 Z)
{
    v3 Result = {(float)X, (float)Y, (float)Z};
    return Result;
}

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

inline v2&
operator+=(v2& A, v2 B)
{
    A = A + B;
    
    return A;
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

inline v3&
operator+=(v3& A, v3 B)
{
    A = A + B;
    
    return A;
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
// NOTE(zoubir): V3
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

inline v3
Minimum3(v3 A, v3 B)
{
    v3 Result;
    
    Result.X = Minimum(A.X, B.X);
    Result.Y = Minimum(A.Y, B.Y);
    Result.Z = Minimum(A.Z, B.Z);

    return Result;
}

inline v3
Maximum3(v3 A, v3 B)
{
    v3 Result;
    
    Result.X = Maximum(A.X, B.X);
    Result.Y = Maximum(A.Y, B.Y);
    Result.Z = Maximum(A.Z, B.Z);

    return Result;
}

//
// NOTE(zoubir): Rectangle2
//

struct rectangle2
{
    v2 Min;
    v2 Max;
};

inline rectangle2
RectCenterHalfDims(v2 Center, v2 HalfDims)
{
    rectangle2 Result;

    Result.Min = Center - HalfDims;
    Result.Max = Center + HalfDims;

    return Result;
}

inline bool32
RectanglesIntersect(rectangle2 A, rectangle2 B)
{
    bool32 Result =
        (A.Min.X < B.Max.X) &&
        (B.Min.X < A.Max.X) &&
        (A.Min.Y < B.Max.Y) &&
        (B.Min.Y < A.Max.Y);
        
    return Result;
}

//
// NOTE(zoubir): Rectangle3
//

struct rectangle3
{
    v3 Min;
    v3 Max;
};

inline rectangle3
RectMinMax(v3 Min, v3 Max)
{
    rectangle3 Result;

    Result.Min = Min;
    Result.Max = Max;

    return(Result);
}

inline rectangle3
RectCenterHalfDims(v3 Center, v3 HalfDims)
{
    rectangle3 Result;

    Result.Min = Center - HalfDims;
    Result.Max = Center + HalfDims;

    return Result;
}

inline rectangle3
AddRadiusTo(rectangle3 Rect, v3 Radius)
{
    rectangle3 Result;

    Result.Min = Rect.Min + Radius;
    Result.Max = Rect.Max + Radius;
    
    return Result;
}

inline bool32
RectanglesIntersect(rectangle3 A, rectangle3 B)
{
    bool32 Result =
        (A.Min.X < B.Max.X) &&
        (B.Min.X < A.Max.X) &&
        (A.Min.Y < B.Max.Y) &&
        (B.Min.Y < A.Max.Y) &&
        (A.Min.Z < B.Max.Z) &&
        (B.Min.Z < A.Max.Z);
        
    return Result;
}

#endif
