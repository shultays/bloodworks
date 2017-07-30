function Invincibility.spawn(bonus, pos)
	playSound({path = "~/resources/sounds/shield.ogg", volume = 0.6})
	
	addBuffIcon("Invincibility", Invincibility.basePath .. "icon.png")
	
	local duration = 6.0 * player.data.bonusDurationMultiplier
	Invincibility.data.timeToDie = time + duration
	if Invincibility.data.gameObject == nil then
		local gameObject = addGameObject("")
		Invincibility.data.gameObject = gameObject
		gameObject:setLevel(RenderableLevel.player + 2)
		gameObject.data.renderable = gameObject:addTexture(Invincibility.basePath .. "shield.png", "~/resources/default")
		gameObject.data.renderable:setAlignment(RenderableAlignment.world)
		gameObject.data.renderable:setWorldMatrix(Mat3.fromScale(34.0, 34.0))
		gameObject.data.renderable:setColor(Vec4.fromColor(0x4488AAFF))
	end
	bonus:setActive(true)
end

function Invincibility.onTick(bonus)
	local data = Invincibility.data
	local gameObject = data.gameObject
	data.gameObject:setPosition(player.position)
	
		
	local remaining = data.timeToDie - time
	if remaining < 2.0 then
		local t = remaining * 4.0
		t = t - math.floor(t)
		if t < 0.5 then
			gameObject.data.renderable:setColor(Vec4.fromColor(0x2288AAFF))
		else
			gameObject.data.renderable:setColor(Vec4.fromColor(0x4488AAFF))
		end
	end
	
	if remaining < 0.0 then
		removeBuffIcon("Invincibility")
		gameObject.toBeRemoved = true
		Invincibility.data.gameObject = nil
		bonus:setActive(false)
	end
end

function Invincibility.onPlayerDamaged(bonus, damage, dir, params)
	if damage <= 0 then
		return damage
	end
	playSound({path = "~/resources/sounds/shield_hit.ogg", volume = 0.6})
	return -1
end