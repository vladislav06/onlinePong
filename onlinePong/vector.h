#pragma once
namespace VectMath
{
    class vector2
    {
    public:

        struct vector {
            float x, y = 0;
        };
        static vector sum(vector a, vector b);
        static vector mult(vector a, vector b);
        static vector mult(vector a, float b);

    };
}

