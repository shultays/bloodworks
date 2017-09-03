
function BossFight.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    missionData.perkPerLevel = 3
    missionData.startTime = time
    missionData.firstTick = true
    addRandomColliders(25, 500.0)
end

function BossFight.onTick()
    local min = missionTime / 60.0

    if player.isDead then
        if isKeyPressed(keys.Space) then
            loadMission("BossFight")
        end
        
        return
    end

    if missionData.firstTick == true then
        local m = addRandomMonster("NarSie")
        missionData.narsie = m
        
        local spawn
        local t = 10
        
        while t > 0 do
            t = t - 1
            local shift = Vec2.fromAngle(math.random() * math.pi) * 200.0
            spawn = player.position + shift
            if hasCircleCollision(spawn, 30.0) == false then
                break
            end
        end
        
        m.position = spawn
    end
    
    missionData.firstTick = false
end


function BossFight.onPlayerDied()
    showGameReset()
end

function BossFight.onMonsterDied(monster)
end

function BossFight.onDebugTick()
end