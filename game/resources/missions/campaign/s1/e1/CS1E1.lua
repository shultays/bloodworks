
function CS1E1.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
    
    missionData.spawnData = {}
    missionData.spawnData.monsterSpeed = 0.7
    
    player.damageMultiplier:addBuffWithId(1, 3.75)
    
    missionData.curObjective = 0
    
    missionData.health = addGameObject("ObjectiveBar", {text = "Kill All Enemies"} )
    
    initTimedMonster()
    missionData.randomMonsterMax = 1
    missionData.maxObjective = 1
    
    setMonsterDropInterval(8.0)
    setDropInterval(10.0)
end

function CS1E1.onTick()
    if missionData.maxObjective == missionData.curObjective and missionData.ended ~= true then
        missionConfig:setIntMax("season1", 2)
        showGameReset("Congrats!", "CS1E2", "Press Space to Continue")
        missionData.ended = true
    end
end

function CS1E1.onPlayerDied()
    showGameReset()
end

function CS1E1.isEnabled(missionConfig)
    return missionConfig:getInt("season1", 1) >= 1
end
