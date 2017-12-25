
function RocketLauncher.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 350.0
    ShootTimer.initGun(gun, 0.7)
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end


function RocketLauncher.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            bullet.damage = math.floor(150 + math.random() * 40)
            bullet:addTrailParticle("RocketSmokeParticle", Vec2.new(0.0, -8.0), 4.0, {})
            bullet.moveSpeed = 100
            bullet.radius = 6
            bullet.data.moveAngle = bullet.moveAngle
            bullet.data.moveSpeed = bullet.moveSpeed
            bullet.data.lateralSpeedDir = player.moveVelocity - (player.moveDir * player.moveVelocity:dot(player.aimDir))
            bullet.data.lateralSpeed = bullet.data.lateralSpeedDir:safeNormalize() * 0.3
            bullet.lifeTime = 2.5
            bullet.meshRotation = bullet.moveAngle
            bullet.data.killCount = 0
        end
    end
end

function RocketLauncher.onBulletHit(gun, bullet, monster)
    if gun.data.checkAchievement then
        local killCount = 0
        if  monster ~= nil and monster.isDead then
            killCount = killCount + 1
        end
        addExplosion(bullet.position, 80.0, 180.0, 70, 130, 0.0, false, function(monster)
            if  monster ~= nil and monster.isDead then
                killCount = killCount + 1
                
                if gun.data.checkAchievement then
                    if hasAchievement( "ACH_ROCKET_LAUNCHER" ) or player.isDead or missionData.isSurvival ~= true then
                        gun.data.checkAchievement = false
                        return
                    end
                    if killCount >= 5 then
                        killCount = -100
                        gun.data.achievementProcess = gun.data.achievementProcess + 1
                        
                        if gun.data.achievementProcess  >= 50 then
                            addAchievement( "ACH_ROCKET_LAUNCHER" )
                            gun.data.checkAchievement = false
                        end
                    end
                end
                
            end
        end)
    else
        addExplosion(bullet.position, 80.0, 180.0, 70, 130)
    end
end

function RocketLauncher.onBulletTick(gun, bullet)
    local data = bullet.data
    
    data.moveSpeed = data.moveSpeed + dt * 250
    if data.moveSpeed > 250.0 then
        data.moveSpeed = 250.0
    end
    
    data.lateralSpeed = data.lateralSpeed - dt * 0.5
    
    if data.lateralSpeed < 0.0 then
        data.lateralSpeed = 0.0
    end
    
    local defaultSpeed = Vec2.fromAngle(data.moveAngle) * data.moveSpeed
    
    local finalSpeed = defaultSpeed + data.lateralSpeedDir * data.lateralSpeed
    
    bullet.moveSpeed = finalSpeed:length()
    bullet.moveAngle = finalSpeed:getAngle()
    
end
