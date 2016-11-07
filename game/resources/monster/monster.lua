
function Monster.init()
	local monster = monsters[monsterId]

	monster.x = math.random() * 800 - 400;
	monster.y = math.random() * 600 - 300;
	
	monster.scale = math.random() * 0.4 + 0.5

	monster.moving = true
	monster.lastHitTime = 0.0
	playAnimation(monsterId, "walk")
end

function Monster.onTick()
	local monster = monsters[monsterId]

	local vPlayer = vector(player.x, player.y)
	local vMonster = vector(monster.x, monster.y)
	local diff = vPlayer - vMonster
	local length = diff:len()
	
	if length < 20 + 20 * monster.scale then
		if monster.moving or monster.lastHitTime + 1.5 < time then
			monster.lastHitTime = time
			monster.moving = false
			playAnimation(monsterId, "attack")
		end
	else
	
		if monster.moving == false then
			monster.moving = true
			playAnimation(monsterId, "walk")
		end
	
	end
	
	
	monster.moveAngle = diff:angle()
	
	if monster.moving then
		monster.moveSpeed = 20;
	else
		monster.moveSpeed = 0;
	end
end
