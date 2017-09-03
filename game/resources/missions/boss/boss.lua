
function NarSieFight.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    debugInit(missionData)
    missionData.perkPerLevel = 3
    missionData.startTime = time
    missionData.firstTick = true
    addRandomColliders(35, 400.0)
end

function NarSieFight.onTick()
    min = missionTime / 60.0 + missionData.extraMin

    if player.isDead then
        if isKeyPressed(keys.Space) then
            loadMission("NarSieFight")
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


function NarSieFight.onPlayerDied()
    showGameReset()
end

function NarSieFight.onMonsterDied(monster)
end

function NarSieFight.onDebugTick()
    debugTick(missionData)
end