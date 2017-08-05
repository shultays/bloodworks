

function FlareTrailParticle.initSystem()
	addAttribute("color", "float")
	addAttribute("moveSpeed", "vec2")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeOutSpeed", "float")
end


function FlareTrailParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = 0.2 + math.random() * 0.3
	
	params.moveSpeed = Vec2.fromAngle(math.random() * math.pi * 2.0) * (2.0 + math.random() * 8.0)
	
	params.initialScale = 3.0 + math.random() * 2.0
	
	params.scaleSpeed = 4.0 + math.random() * 3.0
	
	params.initialAlpha = 0.5 + math.random() * 0.5
	
	params.fadeOutSpeed = 3.8 + math.random() * 0.2
	
	params.texture = FlareTrailParticle.basePath .. "particle.png"
	
end