
MiniGun.buffId = getGlobalUniqueId()

function MiniGun.init(gun)
    gun.crosshairDistance = 550.0
    ShootTimer.initGun(gun, 0.02)
    
    gun.data.waitTime = 0.0
    gun.data.cooldown = false
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
    gun.data.achievementKillCounts = {}
    
    gun.data.shootStarted = false
end


function MiniGun.onTick(gun)
    local data = gun.data
    local shoot = false
    if gun.isTriggered and gun:hasAmmo() and data.cooldown == false then
        shoot = true
    end
    
    if data.shootStarted then
        if data.waitTime < 0.38 then
            shoot = true
        else
            data.shootStarted = false
        end
    end
    if shoot then
        if data.waitTime == 0.0 then
            playSound({path = MiniGun.basePath .. "start.ogg", volume = 0.4})
            data.shootStarted = true
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
         if data.waitTime <= 0.0 then
            data.waitTime = 0.0
            data.cooldown = false
         else 
            if data.cooldown == false then
                playSound({path = MiniGun.basePath .. "end.ogg", volume = 0.4})
                data.cooldown = true
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
    
    
    if data.checkAchievement then
    
        if hasAchievement( "ACH_MINIGUN" ) or player.isDead or missionData.isSurvival ~= true then
            data.checkAchievement = false
            return
        end
        if data.achievementProcess >= 40 then
            addAchievement( "ACH_MINIGUN" )
            data.checkAchievement = false
            return
        end
    
        local t = math.floor(time) - 5
        
        for key,kill in pairs(data.achievementKillCounts) do
            if t >= key then
                data.achievementProcess = data.achievementProcess - kill
                data.achievementKillCounts[ key ] = nil
            end
        end
    end
end


function MiniGun.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if data.checkAchievement then
        if  monster ~= nil and monster.isDead == true then
            data.achievementProcess = data.achievementProcess + 1
            local t = math.floor(time)
            
            local k = data.achievementKillCounts
            
            if k[t] == nil then
                k[t] = 1
            else
                k[t] =  k[t] + 1
            end
        end
    end
 end

