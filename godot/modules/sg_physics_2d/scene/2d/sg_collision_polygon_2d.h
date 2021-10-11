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

#ifndef SG_COLLISION_POLYGON_2D_H
#define SG_COLLISION_POLYGON_2D_H

#include "sg_fixed_node_2d.h"

#include "../../math/sg_fixed_vector2.h"
#include "../../internal/sg_shapes_2d_internal.h"

class SGCollisionPolygon2D : public SGFixedNode2D {

	GDCLASS(SGCollisionPolygon2D, SGFixedNode2D);

protected:
	SGCollisionObject2D *collision_object;
	mutable Rect2 aabb;
	mutable Vector<Point2> polygon;
	Array fixed_polygon;
	SGPolygon2DInternal *internal_shape;
	bool disabled;
	bool concave;

protected:
	static void _bind_methods();
	void _notification(int p_what);

	void update_polygon() const;
	void update_aabb() const;
	void update_fixed_polygon();

	void check_concave();

	static bool is_convex(const Array &p_vertices);

	Array _get_fixed_polygon_pairs() const;
	void _set_fixed_polygon_pairs(const Array &p_pairs);

public:
#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const;
#endif
	void set_disabled(bool p_disabled);
	bool get_disabled() const;

	void set_polygon(const Vector<Point2> &p_polygon);
	Vector<Point2> get_polygon() const;

	void set_fixed_polygon(const Array &p_fixed_polygon);
	Array get_fixed_polygon() const;

	void update_internal_shape() const;
	void sync_to_physics_engine() const;

	virtual String get_configuration_warning() const;

	SGCollisionPolygon2D();
	~SGCollisionPolygon2D();
};

#endif
