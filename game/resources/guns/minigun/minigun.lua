
MiniGun.buffId = getGlobalUniqueId()

function MiniGun.init(gun)
    gun.crosshairDistance = 550.0
    ShootTimer.initGun(gun, 0.02)
    
    gun.data.waitTime = 0.0
    gun.data.cooldown = false
end


function MiniGun.onTick(gun)
    local data = gun.data
    local shoot = false
    if gun.isTriggered and gun:hasAmmo() and gun.data.cooldown == false then
        shoot = true
    end
    
    if shoot then
        if data.waitTime == 0.0 then
            playSound({path = MiniGun.basePath .. "start.ogg", volume = 0.7})
        end
        data.waitTime = data.waitTime + dt
        if data.waitTime > 0.4 then
            if data.waitTime > 2.0 then
                data.waitTime =  2.0
            end
            
            gun.playFiringSound = true
            if ShootTimer.checkGun(gun) then
                gun:consumeAmmo()
                local bullet = gun:addBullet()
                bullet.onDamageArgs.customSlowdownAmount = 4.0
                bullet.onDamageArgs.slowdownOnly = true
                
                local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
                particle.args.initialScale = 2.0
                particle.args.fadeOutSpeed = 1.2
            end
        end
    else
         data.waitTime = data.waitTime - dt * 2.0
         if data.waitTime < 0.0 then
            data.waitTime = 0.0
            gun.data.cooldown = false
         else 
            if gun.data.cooldown == false then
                playSound({path = MiniGun.basePath .. "end.ogg", volume = 0.6})
                gun.data.cooldown = true
            end
         end
         
        gun.playFiringSound = false
    end
    
    
    local t = data.waitTime / 0.3
    if t > 1.0 then
        t = 1.0
    end
    gun.spreadAngle = 0.2 * t
    
    
    if player.maxRotateSpeed:hasBuffInfo(MiniGun.buffId) == false then
        player.maxRotateSpeed:addBuffWithId(MiniGun.buffId, 1.0)
    end

    if player.maxSpeed:hasBuffInfo(MiniGun.buffId) == false then
        player.maxSpeed:addBuffWithId(MiniGun.buffId, 1.0)
    end
    player.maxRotateSpeed:getBuffInfo( MiniGun.buffId ) : setBuffAmount(1.0 - 0.95 * t)
    
    player.maxSpeed:getBuffInfo( MiniGun.buffId ) : setBuffAmount(1.0 - 0.5 * t)
    
end
