extends "res://addons/gut/test.gd"

func test_deterministic_rotation():
	var t: SGFixedTransform2D
	
	t = SGFixedTransform2D.new()
	t.x.x = 65535
	t.x.y = -366
	t.y.x = 366
	t.y.y = 65535
	assert_eq(t.get_rotation(), -290)
	
	t = SGFixedTransform2D.new()
	t.x.x = 65348
	t.x.y = -4956
	t.y.x = 4956
	t.y.y = 65348
	assert_eq(t.get_rotation(), -4359)
	
	t = SGFixed.transform2d(-290, SGFixed.vector2(0, 0))
	assert_eq(t.x.x, 65536)
	assert_eq(t.x.y, -290)
	assert_eq(t.y.x, 290)
	assert_eq(t.y.y, 65536)
	
	# Make sure this gives the same result on all platforms, even after
	# rotating a lot of times and accumulating a lot of error.
	t = SGFixed.transform2d(0, SGFixed.vector2(0, 0))
	for i in range(320):
		t = t.rotated(SGFixed.PI_DIV_4)
	assert_eq(t.x.x, 65535)
	assert_eq(t.x.y, -24)
	assert_eq(t.y.x, 22)
	assert_eq(t.y.y, 65535)
	assert_eq(t.get_rotation(), -18)
	var scale = t.get_scale()
	assert_eq(scale.x, 65535)
	assert_eq(scale.y, 65535)
