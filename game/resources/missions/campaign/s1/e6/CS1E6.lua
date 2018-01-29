
function CS1E6.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
    local d = initTimedMonster()
    d.monsterSpeed = 0.75
    d.randomMonsterMax = 35
    d.hitPoint = 0.35
    d.damage = 2.0
    d.customFunction = function(monster)
        makeBossDefault(monster, 2)
    end
    
    d.maxRandomMonsterAtOnce = 8
    d.randomMonsterTimerInterval = 0.3
    d.takebreath = false

    missionData.curObjective = 0
    missionData.maxObjective = d.getMaxMonster()
    missionData.health = addGameObject("ObjectiveBar", {text = "Kill All Enemies"} )
    
    setMonsterDropInterval(8.0)
    setDropInterval(10.0)
end

function CS1E6.onTick()
    if missionData.maxObjective == missionData.curObjective and missionData.ended ~= true then
        missionConfig:setIntMax("season1", 7)
        showGameReset("Congrats!", "CS1E2", "Press Space to Continue")
        missionData.ended = true
    end
end

function CS1E6.onPlayerDied()
    if missionData.ended ~= true then
        showGameReset()
    end
end

function CS1E6.isEnabled(missionConfig)
    return missionConfig:getInt("season1", 1) >= 6
end
