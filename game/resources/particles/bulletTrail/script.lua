

function BulletTrailParticle.initSystem()
	addAttribute("color", "vec3")
	addAttribute("rotation", "float")
	addAttribute("initialScale", "float")
	addAttribute("scaleSpeed", "float")
	addAttribute("initialAlpha", "float")
	addAttribute("fadeOutSpeed", "float")
	
	addUniform("lastBulletTime", "float")
end


function BulletTrailParticle.addParticle(params, pos, args)
	params.pos = Vec2.new(pos.x, pos.y)
	
	params.color = Vec3.new(0.8, 0.6, 0.3)
	
	params.initialScale = args.initialScale
	
	params.scaleSpeed = 5.0
	
	params.initialAlpha = 0.4
	
	params.fadeOutSpeed = args.fadeOutSpeed
	
	params.rotation = params.bullet.moveAngle
	
	params.lastBulletTime = time - params.particleBeginTime
end