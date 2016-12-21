
function TestMission.init()
	mission.level = 0
end

function TestMission.onTick()
	if getMonsterCount() == 0 then
		mission.level = mission.level + 1
		local gameObject = addGameObject("FadeOutImage")
		gameObject.data.startTime = time
		gameObject.data.fadeOutStartTime = 2.0
		gameObject.data.fadeInDuration = 0.5
		gameObject:setLevel(RenderableLevel.gui + 5)
		gameObject.data.renderable = gameObject:addText("Level : " .. mission.level, "resources/fontData.txt")
        gameObject.data.renderable.alignment = RenderableAlignment.bottom
        gameObject.data.renderable:update()
        gameObject:setPosition(Vec2.new(0, 50))
		
		local v = Vec2.new(math.random() * 600 - 300, math.random() * 500 - 250)
		local ang = math.pi * 2.0 * math.random()
		local vShift = Vec2.new(math.cos(ang), math.sin(ang)) * 50.0
		local monsters = {"Alien", "Alien2", "Spider"}
		monsters = {"Alien2"}
		local selected = monsters[ math.random( #monsters ) ] 
		for i = 1, 6 do
			local monster = addMonster(selected)
			monster.position = v
			monster.moveAngle =  (player.position - v):getAngle()
			v = v + vShift
		end
	
	end
end


function TestMission.clear()


end
