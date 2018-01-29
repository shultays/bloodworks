
function CS1E5.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
    local d = initTimedRandomMonster()
    d.monsterSpeed = 0.8
    d.randomMoveChance = 1.0
    d.hitPoint = 0.5
    d.damage = 0.5
    d.randomMonsterMax = 120
    d.dontAddObjectiveForRandom = true
    
    missionData.curObjective = 0
    missionData.maxObjective = 1
    missionData.health = addGameObject("ObjectiveBar", {text = "Kill the boss monster"} )
    
    setMonsterDropInterval(8.0)
    setDropInterval(10.0)
    
    
    local boss = addMonster("Alien")
    makeBossDefault(boss, 1)
    boss.hitPoint = boss.hitPoint * 5
    boss.data.randomMove = false
    boss.position = player.position + Vec2.new(0.0, 200)
    boss.moveAngle = math.pi * -0.5    
    boss.data.targetShift = Vec2.new(0.0, 0.0)
    
    missionData.maxObjective = boss.hitPoint
    
    missionData.boss = boss
    addCustomOnKill(boss, function (monster)
        missionData.curObjective = 1
        missionData.maxObjective = 1
        missionData.boss = nil
    end)
end

function CS1E5.onTick()
    if missionData.maxObjective == missionData.curObjective and missionData.ended ~= true then
        missionConfig:setIntMax("season1", 6)
        showGameReset("Congrats!", "CS1E5", "Press Space to Continue")
        missionData.ended = true
    end
    
    if missionData.boss ~= nil then
        missionData.curObjective = missionData.maxObjective - missionData.boss.hitPoint
    end
end

function CS1E5.onPlayerDied()
    if missionData.ended ~= true then
        showGameReset()
    end
end

function CS1E5.isEnabled(missionConfig)
    return missionConfig:getInt("season1", 1) >= 5
end
