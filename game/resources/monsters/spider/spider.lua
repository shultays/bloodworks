
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
	data.targetAngle = monster.moveAngle - 0.4 + math.random() * 0.8
	data.moving = true
	data.lastHitTime = 0.0
	
	monster.data.moveTimer = -0.5 + math.random() * 2.5
	if monster.data.moveTimer > 0.0 then
		monster:playAnimation("walk", math.random())
	else
		monster:playAnimation("stand", math.random())
	
	end
	
	StunController.init(monster)
    
	MonsterGroupHelper.init(monster)
	data.playerIgnoreDistance = 30.0
    
	MonsterMeleeHelper.init(monster)
	data.hitWaitTime = 0.1
	data.hitInterval = 0.9
	data.minDamage = 3
	data.maxDamage = 9
    
    data.maxMoveSpeed = 50.0
    data.maxRotateSpeed = 0.03
    data.playerSeeRange = 100.0
end


function Spider.onHit(monster, damage, args)
	StunController.onHit(monster, damage, args)
end


function Spider.onTick(monster)
    data = monster.data
	

	local posToMove = player.position
	
	if data.randomMove then
		posToMove = data.randomPos
		if posToMove == nil or posToMove:distanceSquared(monster.position) < 60 * 60 then
			posToMove = getRandomMapPosition()
			data.randomPos = posToMove
		end
	end
	
	diffToPlayer = player.position - monster.position
	distanceToPlayer = diffToPlayer:length()
	angleToPlayer = diffToPlayer:getAngle()
	
	if distanceToPlayer < data.playerSeeRange then
		local c = (distanceToPlayer - data.playerSeeRange * 0.5) / data.playerSeeRange * 0.5
		if c < 0.0 then
			c = 0.0
		end
		posToMove = posToMove * c + player.position * (1.0 - c)
	end
	
	
	diffToMovePos = posToMove - monster.position
	distanceToMovePos = diffToMovePos:length()
	angleToMovePos = diffToMovePos:getAngle()
	
	MonsterMeleeHelper.onTick(monster)
	
	local cPlayer = (distanceToMovePos - 100.0) / 100.0
	if cPlayer > 1.0 then
		cPlayer = 1.0
	elseif cPlayer < 0.0 then
		cPlayer = 0.0
	end
	if data.moveTimer > 0.0 then
		if cPlayer < 1.0 then
			data.targetAngle = approachAngle(data.targetAngle, angleToMovePos, (0.08 + 1.38 * (1.0 - cPlayer)) * timeScale)
		end
		local cPlayer2 = cPlayer * 5
		if cPlayer2 > 1.0 then
			cPlayer2 = 1.0
		end
		monster.moveAngle = approachAngle(monster.moveAngle, data.targetAngle, 0.02 * timeScale + (1.0 - cPlayer2) * 0.1)
		local moveNewAngle = MonsterGroupHelper.fixAngle(monster, monster.moveAngle)
		monster.moveAngle = approachAngle(monster.moveAngle, moveNewAngle, 0.03 * timeScale)
		data.moveTimer = data.moveTimer - dt
		if data.moveTimer <= 0.0 then
			monster:playAnimation("stand", math.random())
		end
		if data.moving then
			monster.moveSpeed = 160.0 * StunController.getSlowAmount(monster);
		else
			monster.moveSpeed = 0.0;
		end
	else
		monster.moveSpeed = 0.0
		data.moveTimer = data.moveTimer - dt
		if data.moveTimer <= -1.0 then
			monster:playAnimation("walk", math.random())
			monster.data.moveTimer = 0.5 + math.random() * 2.5
			data.targetAngle = angleToMovePos + cPlayer * (- 0.8 + math.random() * 1.6)
		end
	end
	
end
