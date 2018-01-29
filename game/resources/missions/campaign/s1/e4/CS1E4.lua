
function CS1E4.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
    local d = initTimedRandomMonster()
    d.monsterSpeed = 0.8
    d.randomMoveChance = 0.65
    d.hitPoint = 0.8
    d.randomMonsterMax = 60
    
    
    missionData.curObjective = 0
    missionData.maxObjective = d.getMaxMonster()
    missionData.health = addGameObject("ObjectiveBar", {text = "Kill All Enemies"} )
    
    setMonsterDropInterval(8.0)
    setDropInterval(10.0)
end

function CS1E4.onTick()
    if missionData.maxObjective == missionData.curObjective and missionData.ended ~= true then
        missionConfig:setIntMax("season1", 5)
        showGameReset("Congrats!", "CS1E5", "Press Space to Continue")
        missionData.ended = true
    end
end

function CS1E4.onPlayerDied()
    if missionData.ended ~= true then
        showGameReset()
    end
end

function CS1E4.isEnabled(missionConfig)
    return missionConfig:getInt("season1", 1) >= 1
end
