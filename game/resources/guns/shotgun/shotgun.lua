
function Shotgun.init(gun)
    gun.spreadAngle = 0.25
    gun.crosshairDistance = 350.0
    ShootTimer.initGun(gun, 1.05)
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end


function Shotgun.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local killCount = { c = 0}
            for i = 1, 10 do
                local bullet = gun:addBullet()
                local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
                particle.args.initialScale = 2.5
                particle.args.fadeOutSpeed = 1.6
                bullet.moveSpeed = bullet.moveSpeed * (math.random() * 0.4 + 0.6)
                bullet.data.killCount = killCount
            end
        end
    end
end



function Shotgun.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if data.checkAchievement then
        if hasAchievement( "ACH_SHOTGUN" ) or player.isDead or missionData.isSurvival ~= true then
            data.checkAchievement = false
            return
        end
            
        if  monster ~= nil and monster.isDead == true then
            bullet.data.killCount.c = bullet.data.killCount.c + 1
            if bullet.data.killCount.c == 3 then
                data.achievementProcess = data.achievementProcess + 1
                if data.achievementProcess >= 20 then
                    addAchievement( "ACH_SHOTGUN" ) 
                    data.checkAchievement = false
                end
             end
        end
    end
 end

