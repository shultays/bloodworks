
function ParticleTest.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}

    missionData.buffId = getUniqueId()
end

function ParticleTest.onTick()
    if missionData.ticked == nil then
        player:setUltimate( getGun("RocketBarrage"))
        missionData.ticked = true
        player.clipCountMultiplier:addBuffWithId(AmmoManiac.buffId, 400.0)
        
        missionData.gameObject = addGameObject("")
            
        missionData.p = {}
        for i = 0, 40 do
            local p = missionData.gameObject:addParticle("RocketSmokeParticle", {})
            missionData.p[i] = p
        end 
    end
    
    if player.ultimate:isReloading() and player.ultimate.data.shooting ~= nil and player.ultimate.data.shooting <= 0.001 then
        player:setUltimate( nil )
        player:setUltimate( getGun("RocketBarrage"))
        player.ultimate.data.shooting = -1.0
    end
    
    if isKeyPressed(keys.F1) then
        if missionData.speed == true then
             player.shootSpeedMultiplier:addBuffWithId(missionData.buffId, 1.0)
             missionData.speed = false
         else
             player.shootSpeedMultiplier:addBuffWithId(missionData.buffId, 0.1)
             missionData.speed = true
        end
    end
    
    if isKeyPressed(keys.F2) then
        addExplosion(player.position, 200.0, 200.0, 250, 350)
    end
    
    if dt > 0.0 and isKeyUp(keys.X)then
        for i = 0, 4 do
            if isKeyDown(keys.F3 + i) then
                for key,value in pairs(missionData.p, nil) do 
                    local maxZ = 90 + 90 * i
                    for z = 0, maxZ do
                            value:addParticleWithoutArgs(player.position + Vec2.new( (z - maxZ  / 2) * 4 ,  ( key - 20 ) * 10 ))
                    end
                end
            end
        end
    end
end

