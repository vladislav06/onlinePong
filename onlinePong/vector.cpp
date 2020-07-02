#include "vector.h"
namespace VectMath {



	vector2::vector vector2::sum(vector2::vector a, vector2::vector b)
	{
		vector result;
		result.x = a.x + b.x;
		result.y = a.y + b.y;
		return result;
	}

	vector2::vector vector2::mult(vector2::vector a, vector2::vector b)
	{
		vector result;
		result.x = a.x * b.x;
		result.y = a.y * b.y;
		return result;
	}

	vector2::vector vector2::mult(vector2::vector a, float b)
	{
		vector result;
		result.x = a.x * b;
		result.y = a.y * b;
		return result;
	}
}