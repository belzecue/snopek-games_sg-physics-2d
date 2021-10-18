extends "res://addons/gut/test.gd"

func test_get_overlapping_bodies() -> void:
	var GetOverlappingBodies = load("res://tests/functional/SGArea2D/GetOverlappingBodies.tscn")
	
	var result := []
	
	# Run 5 times to attempt to check if this is deterministic.
	for i in range(5):
		var scene = GetOverlappingBodies.instance()
		add_child(scene)
		
		result = scene.do_get_overlapping_bodies()
		assert_eq(result.size(), 2)
		assert_eq(result[0], scene.static_body1)
		assert_eq(result[1], scene.static_body2)
		
		# Add/remove one of the bodies to change the order in the scene tree.
		scene.remove_child(scene.static_body1)
		scene.add_child(scene.static_body1)
		scene.static_body1.sync_to_physics_engine()
		
		result = scene.do_get_overlapping_bodies()
		assert_eq(result.size(), 2)
		assert_eq(result[0], scene.static_body2)
		assert_eq(result[1], scene.static_body1)
		
		remove_child(scene)
		scene.queue_free()

