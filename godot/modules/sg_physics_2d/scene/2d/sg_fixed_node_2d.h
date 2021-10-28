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

#ifndef SG_FIXED_NODE_2D_H
#define SG_FIXED_NODE_2D_H

#include <scene/2d/node_2d.h>

#include "../../math/sg_fixed_vector2.h"
#include "../../math/sg_fixed_transform_2d.h"

class SGCollisionObject2D;

class SGFixedNode2D : public Node2D {
	GDCLASS(SGFixedNode2D, Node2D);

	friend SGCollisionObject2D;
	friend SGFixedVector2;

	Ref<SGFixedTransform2D> fixed_transform;
	Ref<SGFixedVector2> fixed_scale;

	bool updating_transform;

protected:
	static void _bind_methods();

	virtual void _changed_callback(Object *p_changed, const char *p_prop) override;

	_FORCE_INLINE_ SGFixedTransform2DInternal get_fixed_transform_internal() const { return fixed_transform->get_internal(); }
	SGFixedTransform2DInternal get_global_fixed_transform_internal() const;

	void update_fixed_transform_internal(const SGFixedTransform2DInternal &p_transform);
	void update_global_fixed_transform_internal(const SGFixedTransform2DInternal &p_global_transform);

	void _set_fixed_position(const SGFixedVector2Internal &p_fixed_position);

	void _fixed_transform_changed();
	void _fixed_position_changed();
	void _fixed_scale_changed();

	int64_t _get_fixed_position_x() const;
	void _set_fixed_position_x(int64_t p_x);
	int64_t _get_fixed_position_y() const;
	void _set_fixed_position_y(int64_t p_y);

	int64_t _get_fixed_scale_x() const;
	void _set_fixed_scale_x(int64_t p_x);
	int64_t _get_fixed_scale_y() const;
	void _set_fixed_scale_y(int64_t p_y);

public:
	void set_fixed_transform(const Ref<SGFixedTransform2D> &p_transform);
	Ref<SGFixedTransform2D> get_fixed_transform() const;

	void set_fixed_position(const Ref<SGFixedVector2> &p_fixed_position);
	Ref<SGFixedVector2> get_fixed_position();

	void set_fixed_scale(const Ref<SGFixedVector2> &p_fixed_scale);
	Ref<SGFixedVector2> get_fixed_scale();

	void set_fixed_rotation(int64_t p_fixed_rotation);
	int64_t get_fixed_rotation() const;

	void set_global_fixed_transform(const Ref<SGFixedTransform2D> &p_global_transform);
	Ref<SGFixedTransform2D> get_global_fixed_transform() const;

	void set_global_fixed_position(const Ref<SGFixedVector2> &p_fixed_position);
	Ref<SGFixedVector2> get_global_fixed_position();

	void set_global_fixed_rotation(int64_t p_fixed_rotation);
	int64_t get_global_fixed_rotation() const;

	SGFixedNode2D();
	~SGFixedNode2D();
};

#endif