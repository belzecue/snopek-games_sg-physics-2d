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

#include "register_types.h"

#include <core/class_db.h>
#include <core/engine.h>

#include "./math/sg_fixed_singleton.h"
#include "./math/sg_fixed_vector2.h"
#include "./math/sg_fixed_rect2.h"
#include "./math/sg_fixed_transform_2d.h"
#include "./scene/2d/sg_fixed_position_2d.h"
#include "./scene/2d/sg_area_2d.h"
#include "./scene/2d/sg_static_body_2d.h"
#include "./scene/2d/sg_kinematic_body_2d.h"
#include "./scene/2d/sg_ray_cast_2d.h"
#include "./scene/2d/sg_collision_shape_2d.h"
#include "./scene/2d/sg_collision_polygon_2d.h"
#include "./scene/resources/sg_shapes_2d.h"
#include "./internal/sg_world_2d_internal.h"

#include "./editor/sg_fixed_math_editor_plugin.h"
#include "./editor/sg_collision_shape_2d_editor_plugin.h"
#include "./editor/sg_collision_polygon_2d_editor_plugin.h"

static SGFixed *fixed_singleton;
static SGWorld2DInternal *world_singleton;

void register_sg_physics_2d_types() {
	ClassDB::register_class<SGFixed>();
	ClassDB::register_class<SGFixedVector2>();
	ClassDB::register_class<SGFixedRect2>();
	ClassDB::register_class<SGFixedTransform2D>();

	ClassDB::register_class<SGFixedNode2D>();
	ClassDB::register_class<SGFixedPosition2D>();
	ClassDB::register_virtual_class<SGCollisionObject2D>();
	ClassDB::register_class<SGArea2D>();
	ClassDB::register_class<SGStaticBody2D>();
	ClassDB::register_class<SGKinematicBody2D>();
	ClassDB::register_class<SGKinematicCollision2D>();
	ClassDB::register_class<SGRayCast2D>();

	ClassDB::register_class<SGCollisionShape2D>();
	ClassDB::register_class<SGCollisionPolygon2D>();

	ClassDB::register_virtual_class<SGShape2D>();
	ClassDB::register_class<SGRectangleShape2D>();
	ClassDB::register_class<SGCircleShape2D>();

	fixed_singleton = memnew(SGFixed);
	Engine::get_singleton()->add_singleton(Engine::Singleton("SGFixed", SGFixed::get_singleton()));

	world_singleton = memnew(SGWorld2DInternal);

#ifdef TOOLS_ENABLED
	EditorPlugins::add_by_type<SGFixedMathEditorPlugin>();
	EditorPlugins::add_by_type<SGCollisionShape2DEditorPlugin>();
	EditorPlugins::add_by_type<SGCollisionPolygon2DEditorPlugin>();
#endif
}

void unregister_sg_physics_2d_types() {
	memdelete(fixed_singleton);
	memdelete(world_singleton);
}
