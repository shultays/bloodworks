

function ExplosionFireParticle.initSystem()
	addAttribute("color", "vec3")
	addAttribute("moveSpeed", "vec2")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeoutTime", "float")
	addAttribute("explosionEffect", "float")
	addAttribute("uvStart", "vec2")
	addAttribute("uvSize", "vec2")
end


function ExplosionFireParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	local shift = Vec2.new(0.0, 0.0)
	shift:setAngle(math.random() * math.pi * 2.0)
	
	params.pos = params.pos + shift * (3.0 + math.random() * 10.0)
	
	params.color = Vec3.new(0.6 + math.random() * 0.3, 0.2 + math.random() * 0.3, 0.0)
	
	params.moveSpeed = Vec2.new(0.0, 0.0)
	params.moveSpeed:setAngle(math.random() * math.pi * 2.0)
	params.moveSpeed = params.moveSpeed * (math.random() * 12.0)
	
	params.initialScale = 12.0 + math.random() * 8.0
	
	params.scaleSpeed = params.size / params.duration + math.random() * 6.0
	
	params.initialAlpha = 0.5 + math.random() * 0.5
	
	params.fadeoutTime = params.duration + 0.2 - math.random() * 0.4
	
	params.explosionEffect = 0.001 + math.random() * 0.003;
	
	params.uvSize = Vec2.new(0.2 + math.random() * 0.2, 0.2 + math.random() * 0.2)
	params.uvStart = Vec2.new(math.random() * (1.0 - params.uvSize.x), math.random() * (1.0 - params.uvSize.y))
	
end