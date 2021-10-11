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

#include "sg_kinematic_body_2d.h"

#include "../../internal/sg_bodies_2d_internal.h"
#include "../../internal/sg_world_2d_internal.h"

void SGKinematicBody2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("move_and_collide", "linear_velocity"), &SGKinematicBody2D::_move);
	ClassDB::bind_method(D_METHOD("move_and_slide", "linear_velocity", "max_slides"), &SGKinematicBody2D::move_and_slide, DEFVAL(4));
	ClassDB::bind_method(D_METHOD("rotate_and_slide", "rotation", "max_slides"), &SGKinematicBody2D::rotate_and_slide, DEFVAL(4));
}

bool SGKinematicBody2D::move_and_collide(const SGFixedVector2Internal &p_linear_velocity, SGKinematicBody2D::Collision &p_collision) {
	SGWorld2DInternal *world = SGWorld2DInternal::get_singleton();
	SGWorld2DInternal::OverlapInfo overlap_info;

	// First, get our body unstuck, if it's stuck.
	bool stuck = world->get_best_overlapping_body(internal, &overlap_info);
	if (stuck) {
		for (int i = 0; i < 4; i++) {
			SGFixedTransform2DInternal t = internal->get_transform();
			t.set_origin(t.get_origin() + overlap_info.seperation);
			internal->set_transform(t);

			stuck = world->get_best_overlapping_body(internal, &overlap_info);
			if (!stuck) {
				break;
			}
		}
	}
	if (stuck) {
		sync_from_physics_engine();

		// We can't really continue. Bail with some sort of reasonable values.
		p_collision.collider = Object::cast_to<SGCollisionObject2D>((Object *)overlap_info.body->get_data());
		p_collision.normal = SGFixedVector2Internal::ZERO;
		p_collision.remainder = p_linear_velocity;
		return true;
	}

	// Move the body the full amount.
	SGFixedTransform2DInternal original_transform = internal->get_transform();
	SGFixedTransform2DInternal test_transform = original_transform;
	SGFixedVector2Internal destination = original_transform.get_origin() + p_linear_velocity;
	test_transform.set_origin(destination);
	internal->set_transform(test_transform);

	// Check if we're colliding. If not, sync from physics engine and bail.
	if (!world->get_best_overlapping_body(internal, &overlap_info)) {
		_set_fixed_position(test_transform.get_origin());
		return false;
	}
	
	// Use binary search to find the point at which we collide, and the point just before that.
	fixed low = fixed::ZERO;
	fixed hi = fixed::ONE;
	for (int i = 0; i < 8; i++) {
		fixed cur = (low + hi) * fixed::HALF;
		SGFixedVector2Internal test_position = original_transform.get_origin() + (p_linear_velocity * cur);
		test_transform.set_origin(test_position);
		internal->set_transform(test_transform);
		if (world->get_best_overlapping_body(internal, &overlap_info)) {
			hi = cur;
		}
		else {
			low = cur;
			_set_fixed_position(test_position);
		}
	}

	// At this point, the overlap_info will contain info about the collision at 'hi'
	// which is what we want to store in p_collision.
	p_collision.collider = Object::cast_to<SGCollisionObject2D>((Object *)overlap_info.shape->get_owner()->get_data());
	p_collision.normal = overlap_info.seperation.normalized();
	p_collision.remainder = p_linear_velocity - (p_linear_velocity * low);

	return true;
}

Ref<SGFixedVector2> SGKinematicBody2D::move_and_slide(const Ref<SGFixedVector2> &p_linear_velocity, int p_max_slides) {
	ERR_FAIL_COND_V(!p_linear_velocity.is_valid(), Ref<SGFixedVector2>());
	
	SGFixedVector2Internal motion = p_linear_velocity->get_internal();

	while (p_max_slides) {
		Collision collision;

		if (!move_and_collide(motion, collision)) {
			// No collision, so we're good - bail!
			break;
		}
		motion = collision.remainder.slide(collision.normal);

		if (motion == SGFixedVector2Internal::ZERO) {
			// No remaining motion, so we're good - bail!
			break;
		}

		p_max_slides--;
	}

	return Ref<SGFixedVector2>(memnew(SGFixedVector2(motion)));
}

bool SGKinematicBody2D::rotate_and_slide(int64_t p_rotation, int p_max_slides) {
	SGWorld2DInternal *world = SGWorld2DInternal::get_singleton();
	SGWorld2DInternal::OverlapInfo overlap_info;

	// @todo Can't we do this by manipulating the underlying transform?
	set_fixed_rotation(get_fixed_rotation() + p_rotation);
	sync_to_physics_engine();

	bool stuck = world->get_best_overlapping_body(internal, &overlap_info);
	if (stuck) {
		for (int i = 0; i < p_max_slides; i++) {
			SGFixedTransform2DInternal t = internal->get_transform();
			t.set_origin(t.get_origin() + overlap_info.seperation);
			internal->set_transform(t);

			stuck = world->get_best_overlapping_body(internal, &overlap_info);
			if (!stuck) {
				break;
			}
		}
	}

	sync_from_physics_engine();

	return stuck;
}

Ref<SGKinematicCollision2D> SGKinematicBody2D::_move(const Ref<SGFixedVector2> &p_linear_velocity) {
	SGKinematicBody2D::Collision collision;
	if (move_and_collide(p_linear_velocity->get_internal(), collision)) {
		Ref<SGKinematicCollision2D> result = Ref<SGKinematicCollision2D>(memnew(SGKinematicCollision2D));
		result->set_collision(collision);
		return result;
	}
	return Ref<SGKinematicCollision2D>();
}

SGKinematicBody2D::SGKinematicBody2D()
	: SGCollisionObject2D(memnew(SGBody2DInternal(SGBody2DInternal::BodyType::BODY_KINEMATIC)))
{
}

SGKinematicBody2D::~SGKinematicBody2D() {
}

void SGKinematicCollision2D::set_collision(const SGKinematicBody2D::Collision &p_collision) {
	collision = p_collision;
	normal->set_internal(collision.normal);
	remainder->set_internal(collision.remainder);
}

void SGKinematicCollision2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_collider"), &SGKinematicCollision2D::get_collider);
	ClassDB::bind_method(D_METHOD("get_normal"), &SGKinematicCollision2D::get_normal);
	ClassDB::bind_method(D_METHOD("get_remainder"), &SGKinematicCollision2D::get_remainder);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider"), "", "get_collider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "normal", PROPERTY_HINT_TYPE_STRING, "SGFixedVector2"), "", "get_normal");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "remainder", PROPERTY_HINT_TYPE_STRING, "SGFixedVector2"), "", "get_remainder");
}

Object *SGKinematicCollision2D::get_collider() const {
	return collision.collider;
}

Ref<SGFixedVector2> SGKinematicCollision2D::get_normal() const {
	return normal;
}

Ref<SGFixedVector2> SGKinematicCollision2D::get_remainder() const {
	return remainder;
}

SGKinematicCollision2D::SGKinematicCollision2D() {
	normal = Ref<SGFixedVector2>(memnew(SGFixedVector2));
	remainder = Ref<SGFixedVector2>(memnew(SGFixedVector2));
}
