
#include "DSP.hpp"


namespace ttt
{
float buffer_clamp(float val, float min, float max)
{
    return rack::clamp(val, min, max);
}

float interpolate(float left, float right, float alpha)
{
    //Buffer interpolate function from left/right values and distance from
    //left index to value at the corresponding position
    return left*(1-alpha)+right*(alpha);
}
float reverse_interpolate(float val, float alpha, int dir)
{
    //Reverse interpolate from value at float position alpha from left index
    //to value that would appear at left and right side integer indices
    if(dir == 0) //left
    {
        return  val*(1-alpha);
    }
    else //right
    {
        return alpha*val;
    }
}
}

