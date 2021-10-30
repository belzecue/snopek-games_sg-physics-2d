extends SGArea2D

onready var color_rect = $ColorRect

const SPEED = SGFixed.ONE * 10

var vector: SGFixedVector2

func _ready() -> void:
	color_rect.color = Color(randf(), randf(), randf(), 1.0)
	vector = SGFixed.vector2(SGFixed.NEG_ONE + (randi() % SGFixed.TWO), -65536 + (randi() % SGFixed.TWO)).normalized()
	vector.imul(SPEED)

func move_me() -> Array:
	var timing := 0
	var timings := []
	
	timing = OS.get_ticks_usec()
	#var overlapping_areas = get_overlapping_areas()
	var overlapping_areas = get_overlapping_areas(false)
	timings.append(OS.get_ticks_usec() - timing)
	
	if overlapping_areas.size() > 0:
		modulate = Color(0.0, 0.0, 0.0, 1.0)
	else:
		modulate = Color(1.0, 1.0, 1.0, 1.0)
	
	timing = OS.get_ticks_usec()
	fixed_position.iadd(vector)
	timings.append(OS.get_ticks_usec() - timing)
	
	timing = OS.get_ticks_usec()
	sync_to_physics_engine()
	timings.append(OS.get_ticks_usec() - timing)
	
	var viewport_size = get_viewport().size
	if fixed_position.x < 0 or fixed_position.x > SGFixed.from_int(viewport_size.x):
		vector.x = -vector.x
	if fixed_position.y < 0 or fixed_position.y > SGFixed.from_int(viewport_size.y):
		vector.y = -vector.y
	
	return timings
