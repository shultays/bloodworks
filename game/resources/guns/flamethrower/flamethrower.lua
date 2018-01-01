FlamethrowerObject = {}

function Flamethrower.init(gun)
    ShootTimer.initGun(gun, 0.05)
    gun.data.gameObject = addGameObject("FlamethrowerObject")
    gun.data.particle = gun.data.gameObject:addParticle("FlameParticle", {})
    gun.data.particleTime = 0.0
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
    
end

function Flamethrower.onTick(gun)
    local data = gun.data
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            local playerSpeed = player.moveVelocity
            local bulletSpeed = Vec2.fromAngle(bullet.moveAngle) * bullet.moveSpeed
            local totalSpeed = playerSpeed + bulletSpeed
            bullet.moveSpeed = totalSpeed:length()
            bullet.moveAngle = totalSpeed:getAngle()
            bullet.onDamageArgs.noSlowdown = true
            bullet.penetrateCount = -1
        end
        
        local bulletSpeed =  gun.bulletSpeed * player.bulletSpeedMultiplier:getBuffedValue()
        local totalSpeed = player.moveVelocity + player.aimDir * bulletSpeed
        local speed = totalSpeed:normalize()
        local range = speed * gun.bulletLifeTime
        
        local collision = getCollisionForRay(player.gunPos - totalSpeed * 20.0, totalSpeed * (range + 20.0), 0.0)
        collision = collision - 40.0
        
        data.particleTime = data.particleTime - dt
        
        while data.particleTime < 0.0 do
            data.particleTime = data.particleTime + 0.002
            data.particle:addParticle(player.gunPos, {moveSpeed = (player.aimDir * bulletSpeed ) + player.moveVelocity, lifeTime = collision * 0.6/ range})
        end
        gun.playFiringSound = true
    else
        gun.playFiringSound = false
    end
    
    if data.checkAchievement then
        if hasAchievement( "ACH_FLAME_THROWER" ) or player.isDead or missionData.isSurvival ~= true then
            data.checkAchievement = false
            return
        end
        if data.achievementProcess >= 20 then
            addAchievement( "ACH_FLAME_THROWER" )
            data.checkAchievement = false
        end
    end
end

function Flamethrower.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if monster ~= nil then
        if monster.data.flamethrowerObject == nil then
            monster.data.flamethrowerObject = addGameObject("BurnMonsterObject")
            monster.data.flamethrowerObject.data.monster = monster
            monster.data.flamethrowerObject.data.damageMin = 2
            monster.data.flamethrowerObject.data.damageVar = 3
        end
        monster.data.flamethrowerObject.data.count = 4
        
        if data.checkAchievement and monster.data.flamethrowerAchievementGun == nil then
            monster.data.flamethrowerAchievementGun = gun
            addCustomOnTick(monster, function (monster)
                local gun = monster.data.flamethrowerAchievementGun
                if monster.data.flamethrowerAchievementSet == true then
                    if monster.data.flamethrowerObject == nil then
                        monster.data.flamethrowerAchievementSet = false
                        gun.data.achievementProcess = gun.data.achievementProcess - 1
                    end
                else
                    if monster.data.flamethrowerObject ~= nil then
                        monster.data.flamethrowerAchievementSet = true
                        gun.data.achievementProcess = gun.data.achievementProcess + 1
                    end
                end
            end)
            
            addCustomOnKill(monster, function (monster)
                local gun = monster.data.flamethrowerAchievementGun
                if monster.data.flamethrowerAchievementSet == true then
                    monster.data.flamethrowerAchievementSet = false
                    gun.data.achievementProcess = gun.data.achievementProcess - 1
                end
            end)
        end
    end
end

