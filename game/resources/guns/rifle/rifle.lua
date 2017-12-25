
function Rifle.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 350.0
    ShootTimer.initGun(gun, 0.55)
    SpreadHelper.initGun(gun)
    gun.data.maxSpread = 0.25
    gun.data.spreadDecreaseStartTime = 0.35
    gun.data.spreadDecreaseSpeed = 0.80
    gun.data.spreadIncreasePerShoot = 0.03
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end


function Rifle.onTick(gun)
    SpreadHelper.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            SpreadHelper.onShoot(gun)
            local bullet = gun:addBullet()
            local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
            particle.args.initialScale = 3.0
            particle.args.fadeOutSpeed = 0.8
        end
    end
    
end


function Rifle.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if data.checkAchievement then
        if hasAchievement( "ACH_RIFLE" ) or player.isDead or missionData.isSurvival ~= true then
            data.checkAchievement = false
            return
        end
            
        if monster == nil then
            data.achievementProcess = 0
        elseif monster.isDead == true then
            data.achievementProcess = data.achievementProcess + 1
            if data.achievementProcess >= 40 then
                addAchievement( "ACH_RIFLE" ) 
                data.checkAchievement = false
            end
        end
    end
 end