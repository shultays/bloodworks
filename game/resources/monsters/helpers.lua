
StunController = {}

function StunController.init( monster )
    local data = monster.data
	data.stunDuration = 0.02
	data.slowDuration = 0.05
	data.slowMultiplier = 0.4
	data.stunTime = 0.0
	data.slowTime = 0.0
	data.lastStunHitTime = -1
	data.lastHitTime = -1
end

function StunController.getSlowAmount( monster)
	local mul = 1.0
    local data = monster.data

	if data.stunTime > 0.0 then
		mul = 0.0
		data.stunTime = data.stunTime - dt
	elseif data.slowTime > 0.0 then
		mul = data.slowMultiplier * data.slowTime / data.slowDuration
		data.slowTime = data.slowTime - dt
	end
	return mul
end

function StunController.onHit(monster, damage)
    local data = monster.data
	data.stunTime = data.stunDuration
	data.slowTime = data.slowDuration
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

function MonsterGroupHelper.init(monster, playerIgnoreDistance) 
	monster.data.closestMonsterIndex = -1
	if playerIgnoreDistance == nil then
		monster.data.playerIgnoreDistance = 0.0
	else
		monster.data.playerIgnoreDistance = playerIgnoreDistance
	end
end

function MonsterGroupHelper.fixAngle(monster, angle) 
	local closestMonster = nil
	local data = monster.data
	if data.closestMonsterIndex ~= -1 then
		closestMonster = getMonster(data.closestMonsterIndex)
		if closestMonster ~= nil then
			if closestMonster.position:distanceSquared(monster.position) > 30.0 * 30.0 then
				closestMonster = nil
			end
		end
	end
	
	if closestMonster == nil then
		data.ignoreThis = true
		closestMonster = getClosestMonsterInRangeWithIgnoreData(monster.position, 25.0, "ignoreThis")
		data.ignoreThis = false
	else
		data.ignoreThis = true
		local newClosestMonster = getClosestMonsterInRangeWithIgnoreData(monster.position, 25.0, "ignoreThis")
		data.ignoreThis = false
		
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
			
		local diff = player.position - monster.position
		local distancePlayer = diff:length()
		local cPlayer = (distancePlayer - data.playerIgnoreDistance) / 100
		if cPlayer < 0.0 then
			cPlayer = 0.0
		end
		local dot = toOther:sideVec():dot(diff)
		if dot > 0.0 then
			angle = angle + 0.6 * c * cPlayer
		else
			angle = angle - 0.6 * c * cPlayer
		end
	end
	return angle
end