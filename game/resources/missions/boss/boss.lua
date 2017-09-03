
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

    missionData.firstTick = false
end


function BossFight.onPlayerDied()
    showGameReset()
end

function BossFight.onMonsterDied(monster)
end

function BossFight.onDebugTick()
end