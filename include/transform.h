#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "GLKMath.h"

struct transform
{
	GLKVector3 translation;
	GLKVector3 scale;

	GLKMatrix4 model_matrix;
};

void transform_init(struct transform* transform);

void transform_calc(struct transform* transform);

#endif
