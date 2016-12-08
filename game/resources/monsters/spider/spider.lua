
function Spider.init(monster)
	data = monster.data
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
    
	monster:setColor(0xFF000000 + (r * 2 ^ 16) + (g  * 2 ^ 8) + b)
    
	
	local diff = player.position - monster.position
	
	monster.moveSpeed = 0
	monster.moveAngle = math.random() * math.pi * 2.0
	monster.data.targetAngle = monster.moveAngle - 0.4 + math.random() * 0.8
	monster.data.moving = true
	monster.data.lastHitTime = 0.0
	
	monster.data.moveTimer = -0.5 + math.random() * 2.5
	if monster.data.moveTimer > 0.0 then
		monster:playAnimation("walk", math.random())
	else
		monster:playAnimation("stand", math.random())
	
	end
	
	StunController.init(monster)
	MonsterGroupHelper.init(monster)
	monster.data.playerIgnoreDistance = 30.0
	MonsterMeleeHelper.init(monster)
end


function Spider.onHit(monster, damage)
	StunController.onHit(monster, damage)
end


function Spider.onTick(monster)
    data = monster.data
	
	diffToPlayer = player.position - monster.position
	distanceToPlayer = diffToPlayer:length()
	angleToPlayer = diffToPlayer:getAngle()
	
	MonsterMeleeHelper.onTick(monster)
	

	local cPlayer = (distanceToPlayer - 20.0) / 150.0
	
	if cPlayer > 1.0 then
		cPlayer = 1.0
	elseif cPlayer < 0.0 then
		cPlayer = 0.0
	end
	
	if data.moveTimer > 0.0 then

		if cPlayer < 1.0 then
			data.targetAngle = approachAngle(data.targetAngle, angelToPlayer, 0.04 + 0.08 * cPlayer)
		end
		
		monster.moveAngle = approachAngle(monster.moveAngle, data.targetAngle, 0.02)
		local moveNewAngle = MonsterGroupHelper.fixAngle(monster, monster.moveAngle)
		monster.moveAngle = approachAngle(monster.moveAngle, moveNewAngle, 0.03)
		data.moveTimer = data.moveTimer - dt
		if data.moveTimer <= 0.0 then
			monster:playAnimation("stand", math.random())
		end
		if data.moving then
			monster.moveSpeed = 100.0 * StunController.getSlowAmount(monster);
		else
			monster.moveSpeed = 0.0;
		end
	else
		monster.moveSpeed = 0.0
		data.moveTimer = data.moveTimer - dt
		if data.moveTimer <= -1.0 then
			monster:playAnimation("walk", math.random())
			monster.data.moveTimer = 0.5 + math.random() * 2.5
			data.targetAngle = angleToPlayer + cPlayer * (- 0.8 + math.random() * 1.6)
		end
	end
	
end
