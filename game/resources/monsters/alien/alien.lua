
function Alien.init(monster)
	data = monster.data
	monster:setScale(math.random() * 0.4 + 0.5)

	local r = math.floor(230 + 25 * math.random())
	local g = math.floor(230 + 25 * math.random())
	local b = math.floor(230 + 25 * math.random())
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
	monster.moveAngle =  0
	data.moving = true
	data.lastHitTime = 0.0
	
	monster:playAnimation("walk", math.random())
	
    data.maxMoveSpeed = 70.0
    data.maxRotateSpeed = 0.03
    data.playerSeeRange = 150.0
	
	StunController.init(monster)
	MonsterGroupHelper.init(monster)
	MonsterMeleeHelper.init(monster)
	data.minDamage = 8
	data.maxDamage = 12
	
	data.tickWaitTime = 0.0
	
	BulletShooter.init(monster)
end


function Alien.onHit(monster, damage, args)
	StunController.onHit(monster, damage, args)
end


function Alien.onTick(monster)
    data = monster.data
	
	diffToPlayer = player.position - monster.position 
	distanceToPlayer = diffToPlayer:length()
	angleToPlayer = diffToPlayer:getAngle()

	data.tickWaitTime = data.tickWaitTime - dt
	if data.tickWaitTime < 0.0 then
		data.tickWaitTime = 0.2 + math.random() * 0.2 + lerp(0.0, 1.2, clamp((distanceToPlayer - 100) / 1500))
		local posToMove = player.position
		
		if data.randomMove or player.isDead then
			posToMove = data.randomPos
			if posToMove == nil or posToMove:distanceSquared(monster.position) < 60 * 60 then
				posToMove = getRandomMapPosition()
				data.randomPos = posToMove
			end
		end
		
		
		if distanceToPlayer < data.playerSeeRange and player.isDead == false then
			local c = (distanceToPlayer - data.playerSeeRange * 0.5) / data.playerSeeRange * 0.5
			if c < 0.0 then
				c = 0.0
			end
			posToMove = posToMove * c + player.position * (1.0 - c)
		end
		
		diffToMovePos = posToMove - monster.position
		distanceToMovePos = diffToMovePos:length()
		angleToMovePos = diffToMovePos:getAngle()
		data.moveAngle = MonsterGroupHelper.fixAngle(monster, angleToMovePos)
		
	end

	BulletShooter.onTick(monster)
	MonsterMeleeHelper.onTick(monster)
	monster.moveAngle = approachAngle(monster.moveAngle, data.moveAngle, data.maxRotateSpeed * timeScale)
	
	if data.moving then
		monster.moveSpeed = data.maxMoveSpeed * StunController.getSlowAmount(monster);
	else
		monster.moveSpeed = 0.0;
	end
end
