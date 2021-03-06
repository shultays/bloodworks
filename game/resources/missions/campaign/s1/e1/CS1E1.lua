
function CS1E1.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
    local d = initTimedMonster()
    d.monsterSpeed = 0.7
    d.randomMonsterMax = 20
    
    
    missionData.curObjective = 0
    missionData.maxObjective = d.getMaxMonster()
    missionData.health = addGameObject("ObjectiveBar", {text = "Kill All Enemies"} )
    
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
    if missionData.ended ~= true then
        showGameReset()
    end
end

function CS1E1.isEnabled(missionConfig)
    return missionConfig:getInt("season1", 1) >= 1
end
