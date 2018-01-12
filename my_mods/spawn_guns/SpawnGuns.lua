
function SpawnGuns.init(mod)
    local t = getGunCount()
    
    mod.data.guns = {}
    mod.data.ultimates = {}
    
    
    local r = 60.0
    local n = 8
    local m = 8
    local a = 0.0
    
    for i = 0, t - 1 do
        local gun = getGunAtIndex(i)
        
        if gun.scriptName ~= "Pistol" then
            if n == m then
                r = r + 50.0
                n = 0
                a = (math.pi * 2.0 / m) * 0.5 - a
            end
            
            local id = spawnGun( player.position + Vec2.fromAngle(math.pi * 2.0 * n / m + a) * r, gun.scriptName )
            
            if gun.ultimate then
                mod.data.ultimates[id] = id
            else
                mod.data.guns[id] = id
            end
            n = n + 1
        end
    end
    
    mod.data.check = 2
    mod.data.t = time
end

function SpawnGuns.onTick(mod)     

    if mod.data.check > 0 then
        if time - mod.data.t > 60.0 then
            mod.data.check = 0
        end
        
        if player.gun ~= nil and player.gun.scriptName ~= "Pistol" and mod.data.guns ~= nil then
            for i in pairs(mod.data.guns) do 
                removeDrop(i)
            end
            mod.data.check = mod.data.check - 1
            mod.data.guns = nil
        end
        
        if player.ultimate ~= nil and mod.data.ultimates ~= nil then
            for i in pairs(mod.data.ultimates) do 
                removeDrop(i)
            end
            mod.data.check = mod.data.check - 1
            mod.data.ultimates = nil
        end
    end
end