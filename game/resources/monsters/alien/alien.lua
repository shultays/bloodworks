
function Alien.init(monster)
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
	monster.moveAngle =  0
	monster.data.moving = true
	monster.data.lastHitTime = 0.0
	
	
	monster:playAnimation("walk", math.random())
	
	monster.data.closestMonsterIndex = -1
	
	StunController.init(monster)
end


function Alien.onHit(monster, damage)
	StunController.onHit(monster, damage)
end


function Alien.onTick(monster)
    local data = monster.data
	local diff = player.position - monster.position
	local length = diff:length()
	
	if length < 20 + monster.collisionRadius then
		if data.moving or data.lastHitTime + 1.5 < time then
			data.lastHitTime = time
			data.moving = false
			monster:playAnimation("attack")
			data.willHit = true
		end
		if data.willHit and data.lastHitTime + 0.2 < time then
			data.willHit = false
			player:doDamage(math.floor(5 + math.random() * 6))
			
            if player.slowdownOnHit then
                player:slowdown(0.4, 0.15)
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
	
	
	local newAngle = diff:getAngle()
	
	
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
		local cPlayer = length / 100
		if cPlayer < 0.0 then
			cPlayer = 0.0
		end
		local dot = toOther:sideVec():dot(diff)
		if dot > 0.0 then
			newAngle = newAngle + 0.6 * c * cPlayer
		else
			newAngle = newAngle - 0.6 * c * cPlayer
		end
	end
	
	monster.moveAngle = approachAngle(monster.moveAngle, newAngle, 0.05)
	
	if data.moving then
		monster.moveSpeed = 50.0 * StunController.getSlowAmount(monster);
	else
		monster.moveSpeed = 0.0;
	end
end
