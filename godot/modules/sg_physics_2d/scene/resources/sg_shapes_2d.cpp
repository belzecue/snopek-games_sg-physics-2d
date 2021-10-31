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

#include "sg_shapes_2d.h"

#include <servers/visual_server.h>

#include "../../internal/sg_shapes_2d_internal.h"

void SGShape2D::_bind_methods() {
}

SGShape2D::SGShape2D() {
}

SGShape2D::~SGShape2D() {
}

void SGRectangleShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_extents"), &SGRectangleShape2D::get_extents);
	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &SGRectangleShape2D::set_extents);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "extents", PROPERTY_HINT_TYPE_STRING, "SGFixedVector2", PROPERTY_USAGE_EDITOR), "set_extents", "get_extents");

	//
	// For storage in TSCN and SCN files only.
	//

	ClassDB::bind_method(D_METHOD("_get_extents_x"), &SGRectangleShape2D::_get_extents_x);
	ClassDB::bind_method(D_METHOD("_set_extents_x", "x"), &SGRectangleShape2D::_set_extents_x);
	ClassDB::bind_method(D_METHOD("_get_extents_y"), &SGRectangleShape2D::_get_extents_y);
	ClassDB::bind_method(D_METHOD("_set_extents_y", "y"), &SGRectangleShape2D::_set_extents_y);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "extents_x", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "_set_extents_x", "_get_extents_x");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "extents_y", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "_set_extents_y", "_get_extents_y");

}

void SGRectangleShape2D::set_extents(const Ref<SGFixedVector2>& p_extents) {
	ERR_FAIL_COND(!p_extents.is_valid());

	extents->set_internal(p_extents->get_internal());
	_change_notify("extents");
	emit_changed();
}

Ref<SGFixedVector2> SGRectangleShape2D::get_extents() {
	return extents;
}

void SGRectangleShape2D::fixed_vector2_changed(SGFixedVector2 *p_vector) {
	emit_changed();
}

SGShape2DInternal *SGRectangleShape2D::create_internal_shape() const {
	return memnew(SGRectangle2DInternal(fixed(655360), fixed(655360)));
}

int64_t SGRectangleShape2D::_get_extents_x() const {
	return extents->get_x();
}

void SGRectangleShape2D::_set_extents_x(int64_t p_x) {
	extents->set_x(p_x);
}

int64_t SGRectangleShape2D::_get_extents_y() const {
	return extents->get_y();
}

void SGRectangleShape2D::_set_extents_y(int64_t p_y) {
	extents->set_y(p_y);
}

void SGRectangleShape2D::sync_to_physics_engine(SGShape2DInternal *p_internal_shape) const {
	SGRectangle2DInternal* rectangle = (SGRectangle2DInternal *)p_internal_shape;
	rectangle->set_extents(extents->get_internal());
}

void SGRectangleShape2D::draw(const RID &p_to_rid, const Color &p_color) {
	Size2 float_extents = extents->to_float();

	VisualServer::get_singleton()->canvas_item_add_rect(p_to_rid, Rect2(-float_extents, float_extents * 2.0), p_color);
}

SGRectangleShape2D::SGRectangleShape2D() : SGShape2D(),
	extents(Ref<SGFixedVector2>(memnew(SGFixedVector2(SGFixedVector2Internal(fixed(655360), fixed(655360))))))
{
	extents->set_watcher(this);
}

SGRectangleShape2D::~SGRectangleShape2D() {
	extents->set_watcher(nullptr);
}


void SGCircleShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_radius"), &SGCircleShape2D::get_radius);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SGCircleShape2D::set_radius);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "radius"), "set_radius", "get_radius");
}

void SGCircleShape2D::set_radius(int p_radius) {
	radius = fixed(p_radius);
	_change_notify("extents");
	emit_changed();
}

int SGCircleShape2D::get_radius() const {
	return radius.value;
}

SGShape2DInternal *SGCircleShape2D::create_internal_shape() const {
	return memnew(SGCircle2DInternal(fixed(655360)));
}

void SGCircleShape2D::sync_to_physics_engine(SGShape2DInternal *p_internal_shape) const {
	SGCircle2DInternal *circle = (SGCircle2DInternal *)p_internal_shape;
	circle->set_radius(fixed(radius));
}

void SGCircleShape2D::draw(const RID &p_to_rid, const Color &p_color) {
	float float_radius = radius.to_float();

	Vector<Vector2> points;
	for (int i = 0; i < 24; i++) {

		points.push_back(Vector2(Math::cos(i * Math_PI * 2 / 24.0), Math::sin(i * Math_PI * 2 / 24.0)) * float_radius);
	}

	Vector<Color> col;
	col.push_back(p_color);
	VisualServer::get_singleton()->canvas_item_add_polygon(p_to_rid, points, col);
}

SGCircleShape2D::SGCircleShape2D() : SGShape2D(),
	radius(655360)
{
}

SGCircleShape2D::~SGCircleShape2D() {
}
