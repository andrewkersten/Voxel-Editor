#include "transform.h"

void transform_init(struct transform* transform)
{
	transform->translation = GLKVector3Make(0.0, 0.0, 0.0);
	transform->scale = GLKVector3Make(1.0, 1.0, 1.0);

	transform->model_matrix = GLKMatrix4Identity;

	transform_calc(transform);
}

void transform_calc(struct transform* transform)
{
	transform->model_matrix = GLKMatrix4ScaleWithVector3(GLKMatrix4TranslateWithVector3(GLKMatrix4Identity, transform->translation), transform->scale);
}
