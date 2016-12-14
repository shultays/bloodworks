

function PlasmaTrailParticle.initSystem()
	addAttribute("color", "float")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeOutSpeed", "float")
end


function PlasmaTrailParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = 1.0
	params.initialScale = 6.0
	params.scaleSpeed = -10.0 
	params.initialAlpha = 0.6
	params.fadeOutSpeed = 2.0
end