

function BulletTrailParticle.initSystem()
	addAttribute("color", "vec3")
	addAttribute("rotation", "float")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeOutSpeed", "float")
end


function BulletTrailParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = Vec3.new(1.0, 1.0, 1.0)
	
	params.initialScale = 8.0
	
	params.scaleSpeed = 2.0
	
	params.initialAlpha = 0.8
	
	params.fadeOutSpeed = 0.2
	params.rotation = 0.0
end