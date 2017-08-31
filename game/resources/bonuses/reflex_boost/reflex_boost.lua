ReflexBoost.buffId = getGlobalUniqueId()

function ReflexBoost.spawn(bonus, pos)
    local data = ReflexBoost.data

    local duration = 6.0 * player.data.bonusDurationMultiplier
    
    local gameSpeed = getGameSpeedMultiplier()
    gameSpeed:addBuffWithId(ReflexBoost.buffId, 0.5)
    gameSpeed:setBuffDuration(ReflexBoost.buffId, duration)
    gameSpeed:setBuffFadeInFadeOut(ReflexBoost.buffId, 0.25, 0.25)
    
    if bonus:isActive() == false then
        bonus:setActive(true)
        data.shaderStartTime = time
        data.postProcess = addPostProcess("resources/post_process/sepia.ps")
        data.postProcess:setShaderWeight(0.0)
    end
    data.time = time
    playSound({path = ReflexBoost.basePath .. "clock.ogg", volume = 2.2})
    
    addBuffIcon("ReflexBoost", ReflexBoost.basePath .. "icon.png")
end


function ReflexBoost.onTick(bonus)
    local data = ReflexBoost.data
    
    local gameSpeed = getGameSpeedMultiplier()
    if gameSpeed:hasBuffInfo(ReflexBoost.buffId) == false then
        removeBuffIcon("ReflexBoost")
        bonus:setActive(false)
        
        removePostProcess(data.postProcess)
        data.postProcess = nil
        return
    end
    local t = time - data.shaderStartTime
    local a = 1.0
    if t < 0.25 then
        a = t * 4.0
    end
    
    local remaining = gameSpeed:getBuffRemainingTime(ReflexBoost.buffId)
    if remaining < 0.25 then
        a = remaining * 4.0
    end
    ReflexBoost.data.postProcess:setShaderWeight(a * 0.60)
end