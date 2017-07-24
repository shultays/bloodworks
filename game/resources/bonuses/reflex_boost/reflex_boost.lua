
function ReflexBoost.spawn(pos)
	local data = ReflexBoost.data
	if data.boost == nil then
		data.boost = addGameObject("ReflexBoost")
		data.postProcess = addPostProcess("resources/post_process/sepia.ps")
		data.postProcess:setShaderWeight(0.0)
		data.shaderStartTime = time
	else
		data.boost.data.time = time
	end
	
	addBuffIcon("ReflexBoost", ReflexBoost.basePath .. "icon.png")
	
	playSound({path = "~/resources/sounds/clock.ogg", volume = "0.9"})
	
	if ReflexBoost.data.isSlow ~= true then
		ReflexBoost.data.isSlow = true
		multiplyGameSpeed(0.5)
	end
end

function ReflexBoost.init(gameObject)
	gameObject.data.time = time
end


function ReflexBoost.onTick(gameObject)
	local t = time - ReflexBoost.data.shaderStartTime
	local a = 0.5
	
	if t < 0.25 then
		a = t * 2.0
	end
	t = time - gameObject.data.time
	
	local duration = 5.0 * player.data.bonusDurationMultiplier
	
	if t > duration - 0.25 then
	    a = (duration - t) * 2.0
		if ReflexBoost.data.isSlow then
			ReflexBoost.data.isSlow = false
			multiplyGameSpeed(1.0 / 0.5)
		end
	end
	
	ReflexBoost.data.postProcess:setShaderWeight(a)
	if (time - gameObject.data.time) > duration then
		ReflexBoost.data.boost = nil
		removePostProcess(ReflexBoost.data.postProcess)
		ReflexBoost.data.postProcess = nil
		gameObject.toBeRemoved = true
		removeBuffIcon("ReflexBoost")
	end
end