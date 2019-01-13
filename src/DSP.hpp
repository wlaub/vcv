#ifndef TTT_DSP_H
#define TTT_DSP_H

#include <math.h>
#include <functional>
#include "rack.hpp"

namespace ttt
{

float buffer_clamp(float val, float min, float max);

float interpolate(float left, float right, float alpha);
float reverse_interpolate(float val, float alpha, int dir);

class Interpolator
{
    // Handles translation between a floating index and integer indices
    // Pos is the floating index.
    // Left and Right represent integer indices immediately before and after
    // pos.
    // Alpha is an interpolation factor ranging from 0 when Pos is Left to 1
    // when Pos is Right.
    public:
        unsigned int left;
        unsigned int right;
        float alpha;
        float pos;

        std::function<float(float, float, float)> _interp = interpolate;
        std::function<float(float, float, int)> _deinterp = reverse_interpolate;

        Interpolator(float pos, unsigned int length)
        {   
            //Create an interpolator given floating position a wrapping length
            this->pos = pos;

            this->left = floor(pos);
            this->right = this->left+1; 
            if(this->right == length) 
                this->right=0;

            this->alpha = pos-floor(pos);
        }
        Interpolator(unsigned int left, unsigned int right, float alpha)
        {
            //Recover the floating position from left, right, and alpha
            this->left = left;
            this->right = right;
            this->alpha = alpha;
            this->pos = left + (right-left)*alpha;
        }

        float interp(float* buffer)
        {
            //Apply interpolation to the given buffer
            return this->_interp(buffer[this->left], buffer[this->right], this->alpha);
        }

        float* deinterp(float val)
        {
            //Apply single-ended reverse interpolation of the given value
            //to the given side (0/1 -> lower/higher index)
            static float result[2];
            result [0] = this->_deinterp(val, this->alpha, 0);
            result [1] = this->_deinterp(val, this->alpha, 1);           
            return result;
        }

};


class CircularBuffer
{

    public:
        float* data;
        unsigned int head;
        unsigned int tail;
        float loc;
        unsigned int length;

        CircularBuffer(unsigned int length)
        {
            this->length = length;
            this->data = new float[length];
            this->head = 0;
        }

        ~CircularBuffer()
        {
            delete[] this->data;
        }

        //Core Operations
        int push(float val)
        {// printf("buffer_push\n");
            //Insert a new value and shift the buffer
            this->head +=1;
            if(this->head == this->length) this->head = 0;
            this->data[this->head] = val;

            return 0;
        }


        //Discrete Buffer Operations
        unsigned int get_index(unsigned int pos)
        {// printf("buffer_get_index\n");
            //Return the index of the given position in the past
            //e.g. x[-pos]

            while(pos >= this->length)
            {
                pos -= this->length;
            }
            if(pos <= this->head)
            {
                return this->head - pos;
            }
            else
            {
                return (this->length-pos)+this->head;
                //pos = head + d, d>0
                //length-pos        = length-head-d
                //length-pow + head = length-head-d+head
                //                  = length - d
            }

        }

        float add(int index, float val)
        {
            //Add the given value at the given index; Clamp as needed
            //Index is referenced to head
            index = this->get_index(index);
            this->data[index]  += val;
            this->data[index] = buffer_clamp(this->data[index], -10.f, 10.f);
            return this->data[index];
        }

        float get_tap(int index)
        {
            //Get the value at the given index
            index = this->get_index(index);
            return this->data[index];
        }

        //Floating Buffer Operation

        float get_floating_index(float pos)
        { //printf("buffer_get_index\n");
            //Return the index of the given position in the past
            //e.g. x[-pos]

            while(pos >= this->length)
            {
                pos -= this->length;
            }
            if(pos <= this->head)
            {
                return this->head - pos;
            }
            else
            {
                return (this->length-pos)+this->head;
            }

        }

        float* add_floating(float pos, float val)
        {
            //Add the given value at the given position; Clamp as needed
            //Position is referenced to head
            Interpolator in(this->get_floating_index(pos), this->length);
            float new_val = buffer_clamp(in.interp(this->data)+val, -10.f, 10.f);
            float* result = in.deinterp(new_val);
            this->data[in.left] = result[0];
            this->data[in.right] = result[1];
            return result;
        }

        float get_tap_floating(float pos)
        {// printf("buffer_get_tap\n");
            //Get the value at the given position
            float new_idx = this->get_floating_index(pos);
            Interpolator in(new_idx, this->length);
            return in.interp(this->data);
        }

};
}

#endif
