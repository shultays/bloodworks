
function Spider.init(monster)
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
	MonsterGroupHelper.init(monster, 30.0)
end


function Spider.onHit(monster, damage)
	StunController.onHit(monster, damage)
end


function Spider.onTick(monster)
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
	
	
	

	local cPlayer = (length - 100.0) / 50.0
	
	if cPlayer > 1.0 then
		cPlayer = 1.0
	elseif cPlayer < 0.0 then
		cPlayer = 0.0
	end
	
	if data.moveTimer > 0.0 then

		if cPlayer < 1.0 then
			data.targetAngle = approachAngle(data.targetAngle, diff:getAngle(), 0.04 + 0.08 * cPlayer)
		end
		
		data.targetAngle = MonsterGroupHelper.fixAngle(monster, data.targetAngle)
		monster.moveAngle = approachAngle(monster.moveAngle, data.targetAngle, 0.03)
		data.moveTimer = data.moveTimer - dt
		if data.moveTimer <= 0.0 then
			monster:playAnimation("stand", math.random())
		end
		if data.moving then
			monster.moveSpeed = 50.0 * StunController.getSlowAmount(monster);
		else
			monster.moveSpeed = 0.0;
		end
	else
		monster.moveSpeed = 0.0
		data.moveTimer = data.moveTimer - dt
		if data.moveTimer <= -1.0 then
			monster:playAnimation("walk", math.random())
			monster.data.moveTimer = 0.5 + math.random() * 2.5
			data.targetAngle = diff:getAngle() + cPlayer * (- 0.8 + math.random() * 1.6)
		end
	end
	
end
