#include "vector.h"
namespace VectMath {


	vector sum(vector a, vector b)
	{
		vector result;
		result.x = a.x + b.x;
		result.y = a.y + b.y;
		return result;
	}
	void sumR(vector* a, rect* b) {
		//rect result;
		 b->x= a->x + b->x;
		 b->y= a->y + b->y;
		//return result;
	}
	vector mult(vector a, vector b)
	{
		vector result;
		result.x = a.x * b.x;
		result.y = a.y * b.y;
		return result;
	}
	vector mult(vector a, rect b) {
		vector result;
		result.x = a.x * b.x;
		result.y = a.y * b.y;
		return result;

	}
	rect multR(vector a, rect b) {
		rect result;
		result.x = a.x * b.x;
		result.y = a.y * b.y;
		return result;

	}
	vector mult(vector a, float b)
	{
		vector result;
		result.x = a.x * b;
		result.y = a.y * b;
		return result;
	}
}