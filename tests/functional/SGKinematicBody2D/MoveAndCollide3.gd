extends "res://addons/gut/test.gd"

onready var kinematic_right = $SGKinematicBody2DRight
onready var kinematic_right2 = $SGKinematicBody2DRight2
onready var kinematic_left = $SGKinematicBody2DLeft
onready var kinematic_left2 = $SGKinematicBody2DLeft2
var movements := []
var slides := []


func move_and_compare(kinematic: SGKinematicBody2D, velocity: SGFixedVector2) -> void:
	var before := kinematic.fixed_position.copy()
	kinematic.move_and_slide(velocity)
	movements.append(before.distance_to(kinematic.fixed_position))
	slides.append(kinematic.get_slide_count())


func _physics_process(delta: float) -> void:
	movements.clear()
	move_and_compare(kinematic_right, SGFixed.from_float_vector2(Vector2.RIGHT * 30))
	move_and_compare(kinematic_right2, SGFixed.from_float_vector2(Vector2.RIGHT * 30))
	move_and_compare(kinematic_left, SGFixed.from_float_vector2(Vector2.LEFT * 30))
	move_and_compare(kinematic_left2, SGFixed.from_float_vector2(Vector2.LEFT * 30))
	


