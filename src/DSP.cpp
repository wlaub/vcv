
#include "DSP.hpp"


namespace ttt
{
Biquad::Biquad(int number)
{
    nfilt = number;
    head = new int[nfilt];
    memset(head, 0, sizeof(int)*nfilt);
    w = new double*[nfilt];
    for(int i = 0; i < nfilt; ++i)
    {
        w[i] = new double[2];       
        memset(w[i], 0, sizeof(double)*2);
    }
 
}

Biquad::Biquad(int number, const double sections[6]) : Biquad::Biquad(number)
{
    //Initialize from array of [b0,b1*z^-1,b2*z^-2, a0, a1, a2]
    //
    a[0] = sections[3];
    a[1] = sections[4]/a[0];
    a[2] = sections[5]/a[0];
    b[0] = sections[0]/a[0];
    b[1] = sections[1]/a[0];
    b[2] = sections[2]/a[0];
    a[0]=1;

}

double Biquad::step(double x, int i)
{
        double y;
        int zero = head[i];
        int one = 1^head[i];

        double wnew = x - a[1]*w[i][zero] - a[2]*w[i][one];
        y = b[0]*wnew + b[1]*w[i][zero] + b[2]*w[i][one];
        head[i] = one;
        w[i][one] = wnew;

        double clip = 100;

        if(y > clip)
        {
            y = clip;
        }
        else if(y<-clip)
        {
            y = -clip;
        }

        return y;
}



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

