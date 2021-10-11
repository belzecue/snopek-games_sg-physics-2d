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

#include "sg_collision_object_2d.h"

#include <core/engine.h>

#include "sg_collision_shape_2d.h"
#include "sg_collision_polygon_2d.h"
#include "../../internal/sg_world_2d_internal.h"
#include "../../internal/sg_bodies_2d_internal.h"

void SGCollisionObject2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("sync_to_physics_engine"), &SGCollisionObject2D::sync_to_physics_engine);

	ClassDB::bind_method(D_METHOD("get_collision_layer"), &SGCollisionObject2D::get_collision_layer);
	ClassDB::bind_method(D_METHOD("set_collision_layer", "collision_layer"), &SGCollisionObject2D::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &SGCollisionObject2D::get_collision_mask);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &SGCollisionObject2D::set_collision_mask);

	ClassDB::bind_method(D_METHOD("set_collision_layer_bit", "bit", "value"), &SGCollisionObject2D::set_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("get_collision_layer_bit"), &SGCollisionObject2D::get_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("set_collision_mask_bit", "bit", "value"), &SGCollisionObject2D::set_collision_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_mask_bit"), &SGCollisionObject2D::get_collision_mask_bit);

	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
}

void SGCollisionObject2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			add_to_world(SGWorld2DInternal::get_singleton());
			break;
		
		case NOTIFICATION_READY:
			sync_to_physics_engine();
			break;

		case NOTIFICATION_EXIT_CANVAS:
			remove_from_world(SGWorld2DInternal::get_singleton());
			break;
	}
}

void SGCollisionObject2D::add_to_world(SGWorld2DInternal *p_world) const {
	if (internal->get_object_type() == SGCollisionObject2DInternal::OBJECT_AREA) {
		p_world->add_area((SGArea2DInternal *)internal);
	}
	else {
		p_world->add_body((SGBody2DInternal *)internal);
	}
}

void SGCollisionObject2D::remove_from_world(SGWorld2DInternal *p_world) const {
	if (internal->get_object_type() == SGCollisionObject2DInternal::OBJECT_AREA) {
		p_world->remove_area((SGArea2DInternal *)internal);
	}
	else {
		p_world->remove_body((SGBody2DInternal *)internal);
	}
}

void SGCollisionObject2D::add_shape(SGShape2DInternal *p_shape) {
	internal->add_shape(p_shape);
}

void SGCollisionObject2D::remove_shape(SGShape2DInternal *p_shape) {
	internal->remove_shape(p_shape);
}

String SGCollisionObject2D::get_configuration_warning() const {
	String warning = SGFixedNode2D::get_configuration_warning();

	bool has_shape_child = false;
	for (int i = 0; i < get_child_count(); i++) {
		if (Object::cast_to<SGCollisionShape2D>(get_child(i))) {
			has_shape_child = true;
			break;
		}
		if (Object::cast_to<SGCollisionPolygon2D>(get_child(i))) {
			has_shape_child = true;
			break;
		}
	}
	if (!has_shape_child) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("This node needs at least one SGCollisionShape2D as a child.");
	}

	return warning;
}

void SGCollisionObject2D::sync_from_physics_engine() {
	SGFixedTransform2DInternal physics_transform = internal->get_transform();

	SGFixedNode2D *fixed_parent = Object::cast_to<SGFixedNode2D>(get_parent());
	if (!fixed_parent) {
		update_fixed_transform_internal(physics_transform);
		return;
	}

	SGFixedTransform2DInternal parent_transform = fixed_parent->get_global_fixed_transform_internal();
	update_fixed_transform_internal(parent_transform.affine_inverse() * physics_transform);
}

void SGCollisionObject2D::sync_to_physics_engine() const {
	for (int i = 0; i < get_child_count(); i++) {
		SGCollisionShape2D *shape = Object::cast_to<SGCollisionShape2D>(get_child(i));
		if (shape) {
			shape->sync_to_physics_engine();
		}
		else {
			SGCollisionPolygon2D *polygon = Object::cast_to<SGCollisionPolygon2D>(get_child(i));
			if (polygon) {
				polygon->sync_to_physics_engine();
			}
		}
	}

	// Update the body last, because then the shape info will be all setup to
	// be used for updating the body's broadphase element.
	internal->set_transform(get_global_fixed_transform_internal());
}

uint32_t SGCollisionObject2D::get_collision_layer() const {
	return collision_layer;
}

void SGCollisionObject2D::set_collision_layer(uint32_t p_collision_layer) {
	collision_layer = p_collision_layer;
	internal->set_collision_layer(collision_layer);
	_change_notify("collision_layer");
}

uint32_t SGCollisionObject2D::get_collision_mask() const {
	return collision_mask;
}
void SGCollisionObject2D::set_collision_mask(uint32_t p_collision_mask) {
	collision_mask = p_collision_mask;
	internal->set_collision_mask(collision_mask);
	_change_notify("collision_mask");
}

void SGCollisionObject2D::set_collision_layer_bit(int p_bit, bool p_value) {
	uint32_t l = collision_layer;
	if (p_value) {
		l |= (1 << p_bit);
	}
	else {
		l &= ~(1 << p_bit);
	}
	set_collision_layer(l);
}

bool SGCollisionObject2D::get_collision_layer_bit(int p_bit) const {
	return get_collision_layer() & (1 << p_bit);
}

void SGCollisionObject2D::set_collision_mask_bit(int p_bit, bool p_value) {
	uint32_t m = collision_mask;
	if (p_value) {
		m |= (1 << p_bit);
	}
	else {
		m &= ~(1 << p_bit);
	}
	set_collision_mask(m);
}

bool SGCollisionObject2D::get_collision_mask_bit(int p_bit) const {
	return get_collision_mask() & (1 << p_bit);
}

SGCollisionObject2D::SGCollisionObject2D(SGCollisionObject2DInternal *p_internal) {
	internal = p_internal;
	internal->set_data(this);

	collision_layer = 1;
	collision_mask = 1;
}

SGCollisionObject2D::~SGCollisionObject2D() {
	memdelete(internal);
}
