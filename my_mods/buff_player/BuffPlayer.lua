
function BuffPlayer.init(mod)
    local speedMult = mod.config:getFloat("speedMult", 1.5, "increase to make player go faster")
    player.maxSpeed:addBuff(speedMult)
    
    local accelSpeedMult = mod.config:getFloat("accelerationMult", 2.0, "increase to make player accelerate faster")
    player.accelerationMultiplier:addBuff(accelSpeedMult)
    
    local rotateSpeedMult = mod.config:getFloat("rotateSpeedMult", 1.5, "increase to make player rotate faster")
    player.maxRotateSpeed:addBuff(rotateSpeedMult)
    
    local shootSpeedMult = mod.config:getFloat("shootSpeedMult", 1.0, "decrease to make player shoot faster")
    player.shootSpeedMultiplier:addBuff(shootSpeedMult)
    
    local damageMult = mod.config:getFloat("damageMult", 1.0, "increase to make player do more damage")
    player.damageMultiplier:addBuff(damageMult)
end

function BuffPlayer.onTick(mod)

end