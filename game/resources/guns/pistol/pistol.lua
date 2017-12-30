
function Pistol.init(gun)
    ShootTimer.initGun(gun, 0.35)
    
    SpreadHelper.initGun(gun)
    gun.data.minSpread = 0.0
    gun.data.maxSpread = 0.10
    gun.data.spreadDecreaseSpeed = 0.25
    gun.data.spreadIncreasePerShoot = 0.02
    
    gun.data.checkAchievement = true
end


function Pistol.onTick(gun)
    local data = gun.data
    SpreadHelper.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            SpreadHelper.onShoot(gun)
            local bullet = gun:addBullet()
            local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
            particle.args.initialScale = 2.0
            particle.args.fadeOutSpeed = 1.2
            particle.args.color = Vec3.new(0.8, 0.8, 0.8)
        end
    end
    
    if data.checkAchievement then
        if hasAchievement( "ACH_PISTOL" ) or player.isDead or missionData.isSurvival ~= true or 
                    player.gun ~= gun or player.ultimate ~= nil then
                    
            data.checkAchievement = false
            return
        end
        
        if missionTime > 3 * 60.0 then
            addAchievement("ACH_PISTOL")
            data.checkAchievement = false
        end
    end
end
