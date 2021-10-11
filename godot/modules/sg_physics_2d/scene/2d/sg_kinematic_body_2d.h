/*************************************************************************/
/* Copyright (c) 2021 David Snopek                                       */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef SG_KINEMATIC_BODY_2D_H
#define SG_KINEMATIC_BODY_2D_H

#include "sg_collision_object_2d.h"

class SGKinematicCollision2D;

class SGKinematicBody2D : public SGCollisionObject2D {
	GDCLASS(SGKinematicBody2D, SGCollisionObject2D);

protected:
	static void _bind_methods();

public:
	struct Collision {
		SGCollisionObject2D *collider;
		// @todo How can we get the shape in here?
		SGFixedVector2Internal normal;
		SGFixedVector2Internal remainder;

		Collision() {
			collider = nullptr;
		}
	};

	bool move_and_collide(const SGFixedVector2Internal &p_linear_velocity, Collision &p_collision);
	Ref<SGFixedVector2> move_and_slide(const Ref<SGFixedVector2> &p_linear_velocity, int p_max_slides);
	bool rotate_and_slide(int64_t p_rotation, int p_max_slides);

	Ref<SGKinematicCollision2D> _move(const Ref<SGFixedVector2> &p_linear_velocity);

	SGKinematicBody2D();
	~SGKinematicBody2D();

};

class SGKinematicCollision2D : public Reference {
	GDCLASS(SGKinematicCollision2D, Reference);

	friend class SGKinematicBody2D;

	SGKinematicBody2D::Collision collision;
	Ref<SGFixedVector2> normal;
	Ref<SGFixedVector2> remainder;

	void set_collision(const SGKinematicBody2D::Collision &p_collision);

protected:
	static void _bind_methods();

public:
	Object *get_collider() const;
	Ref<SGFixedVector2> get_normal() const;
	Ref<SGFixedVector2> get_remainder() const;

	SGKinematicCollision2D();
};

#endif