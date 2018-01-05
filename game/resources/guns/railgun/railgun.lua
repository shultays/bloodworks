
function Railgun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 350.0
    ShootTimer.initGun(gun, 0.85)
    SpreadHelper.initGun(gun)
    gun.data.maxSpread = 0.05
    gun.data.spreadDecreaseStartTime = 0.35
    gun.data.spreadDecreaseSpeed = 0.40
    gun.data.spreadIncreasePerShoot = 0.03
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end


function Railgun.onTick(gun)
    SpreadHelper.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            SpreadHelper.onShoot(gun)
            local bullet = gun:addBullet()
            bullet.penetrateCount = 4
            
            bullet:addTrailParticle("RailGunParticle", Vec2.new(0.0, 0.0), 7.0, {})
            
            bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {initialScale = 1.5, initialAlpha = 0.2, fadeOutSpeed = 0.3, color = Vec3.new(1.0, 1.0, 1.0)})
            
            bullet.data.killCount = 0
        end
    end
end



function Railgun.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if data.checkAchievement then
        if hasAchievement( "ACH_RAIL_GUN" ) or player.isDead or missionData.isSurvival ~= true then
            data.checkAchievement = false
            return
        end
            
        if  monster ~= nil and monster.isDead == true then
            bullet.data.killCount = bullet.data.killCount + 1
            if bullet.data.killCount == 3 then
                data.achievementProcess = data.achievementProcess + 1
                if data.achievementProcess >= 30 then
                    addAchievement( "ACH_RAIL_GUN" ) 
                    data.checkAchievement = false
                end
             end
        end
    end
 end
