
function Monster.init(monster)
	monster:setScale(math.random() * 0.4 + 0.5)

	local r = math.floor(255 * math.random())
	local g = math.floor(255 * math.random())
	local b = math.floor(255 * math.random())
	local ra = math.random()
	if ra < 0.333 then
		r = 0xff
	elseif ra < 0.666 then
		g = 0xff
	else
		b = 0xff
	end
	monster:setColor(0xFF000000 | (r << 16) | (g << 8) | b)
	
	
	monster.moveAngle = 0
	monster.moveSpeed = 0
	
	monster.data.moving = true
	monster.data.lastHitTime = 0.0
	
	
	monster:playAnimation("walk", math.random())
end

function Monster.onTick(monster)
	local diff = player.position - monster.position
	local length = diff:length()
	
	if length < 20 + monster.collisionRadius then
		if monster.data.moving or monster.data.lastHitTime + 1.5 < time then
			monster.data.lastHitTime = time
			monster.data.moving = false
			monster:playAnimation("attack")
		end
	else
	
		if monster.data.moving == false then
			monster.data.moving = true
			monster:playAnimation("walk", math.random())
		end
	
	end
	
	monster.moveAngle = diff:getAngle()
	
	if monster.data.moving then
		monster.moveSpeed = 40;
	else
		monster.moveSpeed = 0;
	end
end

