QuadDamage.buffId = getGlobalUniqueId()

function QuadDamage.spawn(bonus, pos)
    local data = QuadDamage.data

    local duration = 8.0 * player.data.bonusDurationMultiplier
    
    player.damageMultiplier:addBuffWithId(QuadDamage.buffId, 4.0)
    player.damageMultiplier:setBuffDuration(QuadDamage.buffId, duration)
    if bonus:isActive() == false then
        bonus:setActive(true)
        data.shaderStartTime = time
        data.postProcess = addPostProcess("resources/post_process/red_shift.ps")
        data.postProcess:setShaderWeight(0.0)
    end
    data.time = time
    playSound({path = QuadDamage.basePath .. "metal_riff.ogg", volume = 0.5})
    
    addBuffIcon("QuadDamage", QuadDamage.basePath .. "icon.png")
end

function QuadDamage.onTick(bonus)
    local data = QuadDamage.data
    
    if player.damageMultiplier:hasBuffInfo(QuadDamage.buffId) == false then
        removeBuffIcon("QuadDamage")
        bonus:setActive(false)
        
        removePostProcess(data.postProcess)
        data.postProcess = nil
        return
    end
    
    local t = time - data.shaderStartTime
    local a = 1.0
    if t < 0.3 then
        a = t / 0.3
    end
    
    local remaining = player.damageMultiplier:getBuffRemainingTime(QuadDamage.buffId)
    if remaining < 1.0 then
        a = remaining
    end
    QuadDamage.data.postProcess:setShaderWeight(a)

end
