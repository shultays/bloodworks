

function CriticalParticle.initSystem()
	addAttribute("color", "vec3")
	addAttribute("moveSpeed", "vec2")
	addAttribute("initialScale", "float")
	addAttribute("rotateSpeed", "float")
	addAttribute("uvStart", "vec2")
end


function CriticalParticle.addParticle(params, pos)
	params.pos = Vec2.new(pos.x, pos.y)
	
	local r = 1.0
	local g = 1.0
	local b = 1.0
	local ra = math.random()
	if ra < 0.333 then
		g = math.random()
		b = math.random()
	elseif ra < 0.666 then
		r = math.random()
		b = math.random()
	else
		r = math.random()
		g = math.random()
	end
	
	params.color = Vec3.new(r, g, b)
	
	params.moveSpeed = Vec2.new(0.0, 0.0)
	params.moveSpeed:setAngle(math.random() * math.pi * 2.0)
	params.moveSpeed = params.moveSpeed * (100.0 + math.random() * 50.0)
	
	if params.bullet ~= nil then
		params.moveSpeed = params.bullet.moveSpeedDir
	end
	
	params.initialScale = 10.0 + math.random() * 10.0
	
	params.rotateSpeed = 6.0 + math.random() * 6.0
	
	if math.random() > 0.5 then
		params.rotateSpeed = -params.rotateSpeed
	end
	
	params.uvStart = Vec2.new(math.floor(math.random() * 4) / 4.0, math.floor(math.random() * 4) / 4.0)
	
end