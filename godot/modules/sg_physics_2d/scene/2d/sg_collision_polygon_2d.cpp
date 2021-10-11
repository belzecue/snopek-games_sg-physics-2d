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

#include "sg_collision_polygon_2d.h"

#include <core/engine.h>
#include "sg_collision_object_2d.h"

void SGCollisionPolygon2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_polygon", "polygon"), &SGCollisionPolygon2D::set_polygon);
	ClassDB::bind_method(D_METHOD("get_polygon"), &SGCollisionPolygon2D::get_polygon);

	ClassDB::bind_method(D_METHOD("set_fixed_polygon", "polygon"), &SGCollisionPolygon2D::set_fixed_polygon);
	ClassDB::bind_method(D_METHOD("get_fixed_polygon"), &SGCollisionPolygon2D::get_fixed_polygon);

	ClassDB::bind_method(D_METHOD("set_disabled", "disabled"), &SGCollisionPolygon2D::set_disabled);
	ClassDB::bind_method(D_METHOD("get_disabled"), &SGCollisionPolygon2D::get_disabled);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "polygon", PROPERTY_HINT_NONE, "", 0), "set_polygon", "get_polygon");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "fixed_polygon", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR), "set_fixed_polygon", "get_fixed_polygon");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disabled"), "set_disabled", "get_disabled");

	//
	// For storage in TSCN and SCN files only.
	//

	ClassDB::bind_method(D_METHOD("_get_fixed_polygon_pairs"), &SGCollisionPolygon2D::_get_fixed_polygon_pairs);
	ClassDB::bind_method(D_METHOD("_set_fixed_polygon_pairs", "pairs"), &SGCollisionPolygon2D::_set_fixed_polygon_pairs);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "fixed_polygon_pairs", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "_set_fixed_polygon_pairs", "_get_fixed_polygon_pairs");

}

void SGCollisionPolygon2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (!Engine::get_singleton()->is_editor_hint() && !get_tree()->is_debugging_collisions_hint()) {
				break;
			}

			if (fixed_polygon.size() == 0) {
				break;
			}

			if (polygon.size() == 0) {
				update_polygon();
			}

			int polygon_count = polygon.size();
			for (int i = 0; i < polygon_count; i++) {
				Vector2 p = polygon[i];
				Vector2 n = polygon[(i + 1) % polygon_count];
				// draw line with width <= 1, so it does not scale with zoom and break pixel exact editing
				draw_line(p, n, Color(0.9, 0.2, 0.0, 0.8), 1);
			}

			if (polygon_count > 2) {
				draw_colored_polygon(polygon, get_tree()->get_debug_collisions_color());
			}
		} break;
		
		case NOTIFICATION_PARENTED:
			collision_object = Object::cast_to<SGCollisionObject2D>(get_parent());
			if (collision_object && !disabled && !concave) {
				collision_object->add_shape(internal_shape);
			}
			break;
		
		case NOTIFICATION_UNPARENTED:
			if (collision_object && !disabled && !concave) {
				collision_object->remove_shape(internal_shape);
			}
			collision_object = nullptr;
			break;

	}

}

void SGCollisionPolygon2D::update_polygon() const {
	polygon.clear();
	polygon.resize(fixed_polygon.size());

	for (int i = 0; i < fixed_polygon.size(); i++) {
		Ref<SGFixedVector2> p = fixed_polygon.get(i);
		if (p.is_valid()) {
			polygon.write[i] = p->to_float();
		}
	}

	update_aabb();
}

void SGCollisionPolygon2D::update_aabb() const {
	aabb = Rect2();
	for (int i = 0; i < polygon.size(); i++) {
		if (i == 0) {
			aabb = Rect2(polygon[i], Size2());
		}
		else {
			aabb.expand_to(polygon[i]);
		}
	}

	if (aabb == Rect2()) {
		aabb = Rect2(-10, -10, 20, 20);
	} else {
		aabb.position -= aabb.size * 0.3;
		aabb.size += aabb.size * 0.6;
	}
}

void SGCollisionPolygon2D::update_fixed_polygon() {
	fixed_polygon.clear();
	fixed_polygon.resize(polygon.size());

	for (int i = 0; i < polygon.size(); i++) {
		Ref<SGFixedVector2> p(memnew(SGFixedVector2));
		p->from_float(polygon[i]);
		fixed_polygon[i] = p;
	}

	check_concave();
	update_internal_shape();
	update_configuration_warning();

	_change_notify("fixed_polygon");
}

void SGCollisionPolygon2D::check_concave() {
	bool was_concave = concave;

	concave = !is_convex(fixed_polygon);

	// Add or remove the shape if our "convex-ness" has changed.
	if (concave != was_concave) {
		if (collision_object && !disabled) {
			if (concave) {
				collision_object->remove_shape(internal_shape);
			}
			else {
				collision_object->add_shape(internal_shape);
			}
		}
	}
}

// Algorithm from https://math.stackexchange.com/a/1745427/969278
//
// License: CC BY-SA 3.0
// Author: Nominal Animal
//
bool SGCollisionPolygon2D::is_convex(const Array &p_vertices) {
	if (p_vertices.size() < 3) {
		return false;
	}

	// This algorithm is based on this answer on StackExchange:
	//   https://math.stackexchange.com/a/1745427

	fixed w_sign = fixed::ZERO;

	int x_sign = 0;
	int x_first_sign = 0;
	int x_flips = 0;
	
	int y_sign = 0;
	int y_first_sign = 0;
	int y_flips = 0;

	Ref<SGFixedVector2> prev;
	Ref<SGFixedVector2> cur = p_vertices.get(p_vertices.size() - 2);
	Ref<SGFixedVector2> next = p_vertices.get(p_vertices.size() - 1);

	for (int i = 0; i < p_vertices.size(); i++) {
		prev = cur;
		cur = next;
		next = p_vertices.get(i);

		ERR_FAIL_COND_V_MSG(!cur.is_valid(), false, "Vertex in polygon is invalid");
		ERR_FAIL_COND_V_MSG(!prev.is_valid(), false, "Vertex in polygon is invalid");
		ERR_FAIL_COND_V_MSG(!next.is_valid(), false, "Vertex in polygon is invalid");

		SGFixedVector2Internal previous_edge = cur->get_internal() - prev->get_internal();
		SGFixedVector2Internal next_edge = next->get_internal() - cur->get_internal();

		if (next_edge.x > fixed::ZERO) {
			if (x_sign == 0) {
				x_first_sign = +1;
			}
			else if (x_sign < 0) {
				x_flips++;
			}
			x_sign = +1;
		}
		else if (next_edge.x < fixed::ZERO) {
			if (x_sign == 0) {
				x_first_sign = -1;
			}
			else if (x_sign > 0) {
				x_flips++;
			}
			x_sign = -1;
		}

		if (x_flips > 2) {
			return false;
		}

		if (next_edge.y > fixed::ZERO) {
			if (y_sign == 0) {
				y_first_sign = +1;
			}
			else if (y_sign < 0) {
				y_flips++;
			}
			y_sign = +1;
		}
		else if (next_edge.y < fixed::ZERO) {
			if (y_sign == 0) {
				y_first_sign = -1;
			}
			else if (y_sign > 0) {
				y_flips++;
			}
			y_sign = -1;
		}

		if (y_flips > 2) {
			return false;
		}

		// Find out the orientation of this pair of edges and ensure it doesn't
		// differ from previous ones.
		fixed w = previous_edge.x * next_edge.y - next_edge.x * previous_edge.y;
		if (w_sign == fixed::ZERO && w != fixed::ZERO) {
			w_sign = w;
		}
		else if (w_sign > fixed::ZERO && w < fixed::ZERO) {
			return false;
		}
		else if (w_sign < fixed::ZERO && w > fixed::ZERO) {
			return false;
		}
	}

	// Final wrap-around sign flips.
	if (x_sign != 0 && x_first_sign != 0 && x_sign != x_first_sign) {
		x_flips++;
	}
	if (y_sign != 0 && y_first_sign != 0 && y_sign != y_first_sign) {
		y_flips++;
	}

	// Convex polygons have two sign flips along each axis.
	if (x_flips != 2 || y_flips != 2) {
		return false;
	}

	// We've passed all the tests!
	return true;
}

Array SGCollisionPolygon2D::_get_fixed_polygon_pairs() const {
	Array ret;
	for (int i = 0; i < fixed_polygon.size(); i++) {
		Ref<SGFixedVector2> p = fixed_polygon.get(i);
		if (p.is_valid()) {
			Array pair;
			pair.resize(2);
			pair[0] = p->get_x();
			pair[1] = p->get_y();
			ret.push_back(pair);
		}
	}
	return ret;
}

void SGCollisionPolygon2D::_set_fixed_polygon_pairs(const Array &p_pairs) {
	Array points;
	for (int i = 0; i < p_pairs.size(); i++) {
		Array pair = p_pairs.get(i);
		if (pair.size() == 2) {
			int64_t x = pair[0];
			int64_t y = pair[1];
			Ref<SGFixedVector2> point = SGFixedVector2::from_internal(SGFixedVector2Internal(fixed(x), fixed(y)));
			points.push_back(point);
		}
	}
	set_fixed_polygon(points);
}

#ifdef TOOLS_ENABLED
Rect2 SGCollisionPolygon2D::_edit_get_rect() const {
	return aabb;
}

bool SGCollisionPolygon2D::_edit_use_rect() const {
	return true;
}

bool SGCollisionPolygon2D::_edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const {
	return Geometry::is_point_in_polygon(p_point, Variant(polygon));
}
#endif

void SGCollisionPolygon2D::set_disabled(bool p_disabled) {
	if (disabled != p_disabled) {
		disabled = p_disabled;
		if (collision_object && !concave) {
			if (disabled) {
				collision_object->remove_shape(internal_shape);
			}
			else {
				collision_object->add_shape(internal_shape);
			}
		}
	}
}

bool SGCollisionPolygon2D::get_disabled() const {
	return disabled;
}

void SGCollisionPolygon2D::set_polygon(const Vector<Point2> &p_polygon) {
	polygon = p_polygon;
	update_fixed_polygon();
	update_aabb();

	update();
}

Vector<Point2> SGCollisionPolygon2D::get_polygon() const {
	if (fixed_polygon.size() > 0 && polygon.size() == 0) {
		update_polygon();
	}
	return polygon;
}

void SGCollisionPolygon2D::set_fixed_polygon(const Array &p_fixed_polygon) {
	// Should we really be copying the vectors?
	fixed_polygon = p_fixed_polygon;

	// If polygon is empty, then we don't bother regenerating it, since it's
	// not needed at runtime at all.
	if (polygon.size() > 0) {
		update_polygon();
	}

	check_concave();
	update_internal_shape();
	update_configuration_warning();
}

Array SGCollisionPolygon2D::get_fixed_polygon() const {
	return fixed_polygon;
}

void SGCollisionPolygon2D::update_internal_shape() const {
	Vector<SGFixedVector2Internal> points;
	points.resize(fixed_polygon.size());

	for (int i = 0; i < fixed_polygon.size(); i++) {
		Ref<SGFixedVector2> point = fixed_polygon[i];
		if (point.is_valid()) {
			points.write[i] = point->get_internal();
		}
	}

	internal_shape->set_points(points);
}

void SGCollisionPolygon2D::sync_to_physics_engine() const {
	if (!disabled && !concave) {
		internal_shape->set_transform(get_fixed_transform_internal());
	}
}

String SGCollisionPolygon2D::get_configuration_warning() const {
	String warning = SGFixedNode2D::get_configuration_warning();

	if (fixed_polygon.size() < 3) {
		warning += TTR("Need a polygon with 3 or more points.");
	}
	else if (concave) {
		warning += TTR("This polygon is concave. Only convex polygons are supported.");
	}

	return warning;
}

SGCollisionPolygon2D::SGCollisionPolygon2D() {
	aabb = Rect2(-10, -10, 20, 20);
	disabled = false;
	concave = false;
	collision_object = nullptr;
	internal_shape = memnew(SGPolygon2DInternal);
}

SGCollisionPolygon2D::~SGCollisionPolygon2D() {
	if (collision_object && !disabled && !concave) {
		collision_object->remove_shape(internal_shape);
	}
	memdelete(internal_shape);
}
