

function FlameParticle.initSystem()
	addAttribute("color", "vec3")
	addAttribute("moveSpeed", "vec2")
	addAttribute("initialScale", "float")
	addAttribute("rotateSpeed", "float")
	addAttribute("uvStart", "vec2")
end

function FlameParticle.setDefaultArgs(args)
	args.moveSpeed = 100.0
	args.initialScale = 12.0
	args.rotateSpeed = 6.0
end

function FlameParticle.addParticle(params, pos, args)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = Vec3.new(1.0, 1.0 - math.random() * 0.2, 1.0 - math.random() * 0.2)
	params.moveSpeed = params.moveSpeed * (1.0 + math.random() * 0.1)
	params.moveSpeed:rotateBy(-0.05 + math.random() * 0.1)
	
	params.initialScale = args.initialScale * (1.0 + math.random())
	params.rotateSpeed = args.rotateSpeed * (1.0 + math.random())
	
	params.particleBeginTime = params.particleBeginTime + math.random() * dt
	
	if math.random() > 0.5 then
		params.rotateSpeed = -params.rotateSpeed
	end
	
	params.uvStart = Vec2.new(math.floor(math.random() * 12) / 12.0, math.floor(math.random() * 6) / 6.0)
	
end