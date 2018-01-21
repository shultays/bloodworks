
function CS1E2.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
end

function CS1E2.onTick()
end

function CS1E2.onPlayerDied()
    showGameReset()
end

function CS1E2.isEnabled(missionConfig)
    return missionConfig.getInt("season1", 2) >= 1
end
