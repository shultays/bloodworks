
function CS1E1.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    
end

function CS1E1.onTick()

end

function CS1E1.onPlayerDied()
    showGameReset()
end

function CS1E1.isEnabled(missionConfig)
    return missionConfig.getInt("season1", 1) >= 1
end
