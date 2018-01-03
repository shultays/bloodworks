
function KorayTest.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}

    missionData.buffId = getUniqueId()
end

function KorayTest.onTick()
    if missionData.ticked == nil then
        player:setGun( getGun("RocketLauncher"))
        player:setUltimate( getGun("RocketBarrage"))
        missionData.ticked = true
        player.clipCountMultiplier:addBuffWithId(AmmoManiac.buffId, 400.0)
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
    
end

