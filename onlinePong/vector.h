#pragma once
namespace VectMath
{

    struct vector {
        float x, y = 0;
    };
    struct rect {
        vector vect;
        int x, y = 0;
        int h, w = 0;
    };
     vector sum(vector a, vector b);
     void sumR(vector* a, rect* b);
     vector mult(vector a, vector b);
     vector mult(vector a, rect b);
     rect multR(vector a, rect b);
     vector mult(vector a, float b);


}

