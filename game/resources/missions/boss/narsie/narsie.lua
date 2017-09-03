
function NarSie.init(monster, min)
    data = monster.data

    monster.moveSpeed = 0
    monster.moveAngle = 0.0
    monster.spriteAngle = math.pi * -0.5
    
    monster:playAnimation("stand")
    
    monster:setDrawLevel(RenderableLevel.player + 20)

    monster.knockbackResistance:addBuff(0.01)
    
    monster.data.inhaleTimer = 0.0
    
    monster.hasCollision = false
end

function NarSie.spawnChanceInMission(missionData, min)
    return 0.0
end

function NarSie.onTick(monster)
    data = monster.data
    
    data.inhaleTimer = data.inhaleTimer - dt
    
    if data.inhaleTimer < 0.0 then
        data.inhaleTimer = 1.0 + math.random() * 1.0
        monster:playAnimation("inhale")
    end
end
