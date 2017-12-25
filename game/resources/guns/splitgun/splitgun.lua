
function SplitGun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 400.0
    ShootTimer.initGun(gun, 0.45)
    
    SpreadHelper.initGun(gun)
    gun.data.minSpread = 0.02
    gun.data.maxSpread = 0.10
    gun.data.spreadDecreaseSpeed = 0.25
    gun.data.spreadIncreasePerShoot = 0.025
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end


function SplitGun.onTick(gun)
    SpreadHelper.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            SpreadHelper.onShoot(gun)
            local bullet = gun:addBullet()
            local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
            bullet.data.remainingSplit = 2
            particle.args.initialScale = 2.0
            particle.args.fadeOutSpeed = 1.2
            playSound({path = SplitGun.basePath .. "split_gun.ogg"})
        end
    end
end


function SplitGun.onReloadEnd(gun)
    if gun.data.checkAchievement then
        gun.data.achievementHitCount = {}
    end
end

function SplitGun.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if monster ~= nil then
        if data.splitID ~= nil then
            monster:addIgnoreId(data.splitID)
        end
        if bullet.data.remainingSplit > 0 then
            local newBullet = gun:addBullet()
            local particle = newBullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
            particle.args.initialScale = 2.0
            particle.args.fadeOutSpeed = 1.2
            particle.args.startFadeinSpeed = 10000.0
            newBullet.position = bullet.position
            newBullet.moveAngle = bullet.moveAngle - math.pi * 0.1
            newBullet.data.remainingSplit = bullet.data.remainingSplit - 1
            monster:addIgnoreId(newBullet.id)
            newBullet.damage = math.floor( bullet.damage * 0.6 )
            newBullet.data.splitHit = {}
            newBullet.lifeTime = 0.6 - ( 1 - newBullet.data.remainingSplit ) * 0.3
            local prevBullet = newBullet
            
            newBullet = gun:addBullet()
            particle = newBullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
            particle.args.initialScale = 2.0
            particle.args.fadeOutSpeed = 1.2
            particle.args.startFadeinSpeed = 10000.0
            newBullet.position = bullet.position
            newBullet.moveAngle = bullet.moveAngle + math.pi * 0.1
            newBullet.data.remainingSplit = bullet.data.remainingSplit - 1
            monster:addIgnoreId(newBullet.id)
            newBullet.damage = math.floor( bullet.damage * 0.6 )
            newBullet.data.splitHit = prevBullet.data.splitHit
            newBullet.lifeTime = 0.6 - ( 1 - newBullet.data.remainingSplit ) * 0.3
            
            newBullet.data.splitID = prevBullet.data.id
            prevBullet.data.splitID = newBullet.data.id
            
            playSound({path = SplitGun.basePath .. "split_gun.ogg", position = bullet.position, volume = 0.5})
        end
    end
    
    
    if data.checkAchievement then
        if hasAchievement( "ACH_SPLIT_GUN" ) or player.isDead or missionData.isSurvival ~= true then
            data.checkAchievement = false
            return
        end
        if monster ~= nil and bullet.data.splitHit ~= nil then
        
            if  bullet.data.splitHit[0] == nil then
                bullet.data.splitHit[0] = true
            else
                data.achievementProcess = data.achievementProcess + 1
                if data.achievementProcess >= 50 then
                    addAchievement( "ACH_SPLIT_GUN" ) 
                    data.checkAchievement = false
                end
            end
            bullet.data.splitHit = nil
        end
    end
    
end