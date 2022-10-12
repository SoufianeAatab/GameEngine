#pragma once

struct prng{
    i32 Seed;
};

#define _A_ (8253729)
#define _C_ (2396403)
#define _M_ (1<<31)

u32 RandI32(prng* Prng, i32 Start, i32 End){
    u32 Result = 0;
    u32 A = Prng->Seed * 15485863;
    // Wikipedia
    // Will return in range 0 to 1 if seed >= 0 and -1 to 0 if seed < 0.
    Result = (A * A * A % 2038074743);
    ++Prng->Seed;
    return Result;
}   

f32 Rand(prng* Prng, i32 Start, i32 End){
    i32 Temp = 0;
    Temp = (_A_ * Prng->Seed * _C_) % _M_;
    f32 Result = (f32)Temp / (f32)_M_;
    Result = (Result * (End-Start)) + Start; 
    ++Prng->Seed;
    return Result;
}