
function QuadDamage.spawn(pos)
	local data = QuadDamage.data
	if data.boost == nil then
		data.boost = addGameObject("QuadDamage")
		player.damageMult = player.damageMult * 4.0
		QuadDamage.shaderStartTime = time
		data.postProcess = addPostProcess("resources/post_process/red_shift.ps")
		data.postProcess:setShaderWeight(0.0)
	else
		data.boost.data.time = time
	end
	playSound({path = "resources/sounds/metal_riff.ogg", volume = 0.5})
end

function QuadDamage.init(gameObject)
	gameObject.data.time = time
end


function QuadDamage.onTick(gameObject)
	local t = time - QuadDamage.shaderStartTime
	local a = 1.0
	if t < 0.3 then
		a = t / 0.3
	end
	t = time - gameObject.data.time
	if t > 9.0 then
		a = (10.0 - t)
	end
	QuadDamage.data.postProcess:setShaderWeight(a)

	if t > 10.0 then
		QuadDamage.data.boost = nil
		removePostProcess(QuadDamage.data.postProcess)
		QuadDamage.data.postProcess = nil
		player.damageMult = player.damageMult / 4.0
		gameObject.toBeRemoved = true
	end
end