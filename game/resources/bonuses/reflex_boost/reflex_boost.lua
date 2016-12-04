
function ReflexBoost.spawn(pos)
	if ReflexBoost.boost == nil then
		ReflexBoost.boost = addGameObject("ReflexBoost")
	else
		ReflexBoost.boost.data.time = time
	end
end

function ReflexBoost.init(gameObject)
	gameObject.data.time = time
	multiplyGameSpeed(0.5)
end


function ReflexBoost.onTick(gameObject)
	if time - gameObject.data.time > 5.0 then
		ReflexBoost.boost = nil
		multiplyGameSpeed(1.0 / 0.5)
		gameObject.toBeRemoved = true
	end
end