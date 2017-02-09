
StunController = {}

function StunController.init(monster)
	data.stunDuration = 0.02
	data.slowDuration = 0.15
	data.slowMultiplier = 0.4
	data.stunTime = 0.0
	data.slowTime = 0.0
end

function StunController.getSlowAmount(monster)
	local mul = 1.0

	if data.stunTime > 0.0 then
		mul = 0.0
		data.stunTime = data.stunTime - dt
	elseif data.slowTime > 0.0 then
		mul = data.customSlowdownAmount * data.slowMultiplier * (data.slowDuration - data.slowTime)/ data.slowDuration
		data.slowTime = data.slowTime - dt
	end
	return mul
end

function StunController.onHit(monster, damage, args)
	local data = monster.data
	if args.customSlowdownAmount ~= nil then
		data.customSlowdownAmount = args.customSlowdownAmount
	else
		data.customSlowdownAmount = 1.0
	end
	if args.slowdownOnly then
		data.slowTime = data.slowDuration
	elseif args.doNotStun ~= true then
		data.stunTime = data.stunDuration
		data.slowTime = data.slowDuration
	end
end



MeleeHitImage = {}

function MeleeHitImage.build(monster)
	local gameObject =  addGameObject("MeleeHitImage")
	gameObject.data.startTime = time
	gameObject.data.renderable = gameObject:addTexture("resources/monsters/melee_hit.png", "resources/default")
	gameObject:setPosition(player.position)
	gameObject:setScale(Vec2.new(0.35, 0.35))
	gameObject:setRotation(-math.pi * 0.5 - monster.moveAngle)
end

function MeleeHitImage.init(gameObject)
end

function MeleeHitImage.onTick(gameObject)
	local timeDiff = time - gameObject.data.startTime
	local alpha
	if timeDiff < 0.05 then
		alpha = timeDiff / 0.05
	else 
		alpha = 1.0 - (timeDiff - 0.3) / 0.2
		if alpha < 0.0 then 
			gameObject.toBeRemoved = true
			alpha = 0.0
		elseif alpha > 1.0 then
			alpha = 1.0
		end
	end
	local a =  math.floor(255 * alpha)
	gameObject.data.renderable.color = (a * 2 ^ 24) + 0x00FFFFFF 
	gameObject.data.renderable:update()
end

function MeleeHitImage.clear(monster)

end

MonsterGroupHelper = {}

function MonsterGroupHelper.init(monster) 
	monster.data.closestMonsterIndex = -1
	monster.data.playerIgnoreDistance = 0.0
end

function MonsterGroupHelper.fixAngle(monster, angle) 
	local closestMonster = nil
	if data.closestMonsterIndex ~= -1 then
		closestMonster = getMonster(data.closestMonsterIndex)
		if closestMonster ~= nil then
			if closestMonster.position:distanceSquared(monster.position) > 30.0 * 30.0 then
				closestMonster = nil
			end
		end
	end
	
	if closestMonster == nil then
		closestMonster = getClosestMonsterInRangeWithIgnoreId(monster.position, 25.0, monster.index)
	else
		local newClosestMonster = getClosestMonsterInRangeWithIgnoreId(monster.position, 25.0, monster.index)
		
		if newClosestMonster ~= nil and newClosestMonster ~= closestMonster then
			if closestMonster.position:distanceSquared(monster.position) > newClosestMonster.position:distanceSquared(monster.position) + 15 * 15 then
				closestMonster = newClosestMonster
			end
		end
	end
	
	if closestMonster ~= nil then
		data.closestMonsterIndex = closestMonster.index
		local toOther = closestMonster.position - monster.position;
		local c = 1.0 - toOther:length() / 30.0
			
		local cPlayer = (distanceToPlayer - data.playerIgnoreDistance) / 100
		if cPlayer < 0.0 then
			cPlayer = 0.0
		end
		local dot = toOther:sideVec():dot(diffToPlayer)
		if dot > 0.0 then
			angle = angle + 0.6 * c * cPlayer
		else
			angle = angle - 0.6 * c * cPlayer
		end
	end
	return angle
end

MonsterMeleeHelper = {}
MonsterMeleeHelper.slowBuffId = getGlobalUniqueId()

function MonsterMeleeHelper.init(monster) 
	monster.data.moving = true
	monster.data.lastHitTime = -1.0
	monster.data.hitInterval = 1.5
	monster.data.hitWaitTime = 0.15
	monster.data.minDamage = 5
	monster.data.maxDamage = 11
	monster.data.slowdownAmount = 0.4
	monster.data.slowdownDuration = 0.15
end

function MonsterMeleeHelper.onTick(monster)
	local range = 20.0
	if data.moving == false then
		range = 30.0
	end
	
	if distanceToPlayer < range + monster.collisionRadius and player.isDead == false then
		if data.moving or data.lastHitTime + data.hitInterval < time then
			data.lastHitTime = time
			data.moving = false
			monster:playAnimation("attack")
			data.willHit = true
		end
		if data.willHit and data.lastHitTime + data.hitWaitTime < time then
			data.willHit = false
			player:doDamage(math.floor(data.minDamage + math.random() *(data.maxDamage - data.minDamage)))
			playSound({path = "resources/sounds/melee_woosh.ogg", volume = 0.3})
            if player.data.noSlowdownOnHit == nil then
				player.maxSpeed:addBuffWithId(MonsterMeleeHelper.slowBuffId, data.slowdownAmount)
				player.maxSpeed:setBuffDuration(MonsterMeleeHelper.slowBuffId, data.slowdownDuration)
				player.maxSpeed:setBuffFadeInFadeOut(MonsterMeleeHelper.slowBuffId, data.slowdownDuration/4, data.slowdownDuration/4)
			end
			
			MeleeHitImage.build(monster)
		end
	else
		data.willHit = false
		if data.moving == false then
			data.moving = true
			monster:playAnimation("walk", math.random())
		end
	end
end



BulletShooter = {}

function BulletShooter.init(monster)
	data.bulletMinDamage = 2
	data.bulletMaxDamage = 6
	data.bulletSpeed = 300
	data.bulletRate = 4.0
	data.bulletRandom = 0.3
	data.shootsBullets = false
end

function BulletShooter.onTick(monster)
	if data.shootsBullets then
		if data.bulletTimer == nil then
			data.bulletTimer = data.bulletRate + math.random() * 2.0
		end
		data.bulletTimer = data.bulletTimer - dt
		if data.bulletTimer < 0.0 then
			data.bulletTimer = data.bulletRate + math.random() * 2.0
			
			local bullet = addCustomBullet()
			bullet.damage = math.floor(math.random(data.bulletMinDamage, data.bulletMaxDamage))
			bullet.position = monster.position + monster.moveDir * 6.0
			bullet.moveSpeed = data.bulletSpeed
			bullet.moveAngle = monster.moveAngle + math.random() * data.bulletRandom * 2.0 - data.bulletRandom
			bullet.monsterBullet = true
			bullet:addRenderableTextureWithSize("resources/monsters/bullet.png", Vec2.new(18.0, 18.0))
		end
	end
end








