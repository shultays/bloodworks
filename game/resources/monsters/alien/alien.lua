
function Alien.init(monster)
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
	monster.moveAngle =  0
	monster.data.moving = true
	monster.data.lastHitTime = 0.0
	
	
	monster:playAnimation("walk", math.random())
	
	StunController.init(monster)
	MonsterGroupHelper.init(monster)
	MonsterMeleeHelper.init(monster)
end


function Alien.onHit(monster, damage)
	StunController.onHit(monster, damage)
end


function Alien.onTick(monster)
    data = monster.data
	
	diffToPlayer = player.position - monster.position
	distanceToPlayer = diffToPlayer:length()
	angleToPlayer = diffToPlayer:getAngle()
	
	
	MonsterMeleeHelper.onTick(monster)
	
	newAngle = MonsterGroupHelper.fixAngle(monster, angleToPlayer)
	
	monster.moveAngle = approachAngle(monster.moveAngle, angleToPlayer, 0.05 * timeScale)
	
	if data.moving then
		monster.moveSpeed = 50.0 * StunController.getSlowAmount(monster);
	else
		monster.moveSpeed = 0.0;
	end
end
