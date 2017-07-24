

function RocketSmokeParticle.initSystem()
	addAttribute("color", "float")
	addAttribute("moveSpeed", "vec2")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeOutSpeed", "float")
end


function RocketSmokeParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = 0.2 + math.random() * 0.3
	
	params.moveSpeed = Vec2.new(0.0, 0.0)
	params.moveSpeed:setAngle(math.random() * math.pi * 2.0)
	params.moveSpeed = params.moveSpeed * (2.0 + math.random() * 8.0)
	
	params.initialScale = 4.0 + math.random() * 2.0
	
	params.scaleSpeed = 6.0 + math.random() * 6.
	
	params.initialAlpha = 0.5 + math.random() * 0.5
	
	params.fadeOutSpeed = 0.4 + math.random() * 0.4
	
	params.texture = RocketSmokeParticle.basePath .. "particle.png"
	
end