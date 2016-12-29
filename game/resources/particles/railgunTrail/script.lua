

function RailGunParticle.initSystem()
	addAttribute("color", "float")
	addAttribute("moveSpeed", "vec2")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeOutSpeed", "float")
	addAttribute("rotation", "float")
end


function RailGunParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = 0.2 + math.random() * 0.3
	
	params.moveSpeed = params.bullet.moveDir:sideVec()
	params.moveSpeed = params.moveSpeed * (3.0 + math.random() * 2.0)
	if math.random() < 0.5 then
		params.moveSpeed = -params.moveSpeed
	end
	
	params.rotation = params.bullet.moveAngle
	
	params.initialScale = 5.0
	params.initialScale = 5.0
	
	params.scaleSpeed = 10.0 + math.random() * 1.0
	
	params.initialAlpha = 2.0
	
	params.fadeOutSpeed = 4.0 + math.random() * 0.3
	
end