
function CS1E2.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
    local d = initTimedLineMonster()
    d.randomMonsterMax = 20
    d.monsterSpeed = 0.8
    d.hitPoint = 0.8
    
    missionData.curObjective = 0
    missionData.maxObjective = d.getMaxMonster()
    missionData.health = addGameObject("ObjectiveBar", {text = "Kill All Enemies"} )
    
    setMonsterDropInterval(8.0)
    setDropInterval(10.0)
end

function CS1E2.onTick()
    if missionData.maxObjective == missionData.curObjective and missionData.ended ~= true then
        missionConfig:setIntMax("season1", 3)
        showGameReset("Congrats!", "CS1E3", "Press Space to Continue")
        missionData.ended = true
    end
end

function CS1E2.onPlayerDied()
    if missionData.ended ~= true then
        showGameReset()
    end
end

function CS1E2.isEnabled(missionConfig)
    return missionConfig:getInt("season1", 1) >= 2
end
