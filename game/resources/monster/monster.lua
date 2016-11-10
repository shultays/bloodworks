
function Monster.init(monster)
	local luaMonster = monsters[monster.index]

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
	
	luaMonster.moving = true
	luaMonster.lastHitTime = 0.0
	
	
	monster:playAnimation("walk", math.random())
end

function Monster.onTick(monster)
	local luaMonster = monsters[monster.index]
	
	local vPlayer = vector(player.x, player.y)
	local diff = vector(vPlayer.x - monster.position.x, vPlayer.y - monster.position.y)
	local length = diff:len()
	
	if length < 20 + monster.collisionRadius then
		if luaMonster.moving or luaMonster.lastHitTime + 1.5 < time then
			luaMonster.lastHitTime = time
			luaMonster.moving = false
			monster:playAnimation("attack")
		end
	else
	
		if luaMonster.moving == false then
			luaMonster.moving = true
			monster:playAnimation("walk", math.random())
		end
	
	end
	
	monster.moveAngle = diff:angle()
	
	if luaMonster.moving then
		monster.moveSpeed = 40;
	else
		monster.moveSpeed = 0;
	end
end

