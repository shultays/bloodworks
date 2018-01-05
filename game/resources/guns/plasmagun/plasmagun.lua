
function PlasmaGun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 400.0
    ShootTimer.initGun(gun, 0.35)
    SpreadHelper.initGun(gun)
    gun.data.maxSpread = 0.10
    gun.data.spreadDecreaseStartTime = 0.0
    gun.data.spreadDecreaseSpeed = 0.05
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end


function PlasmaGun.onTick(gun)
    SpreadHelper.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            SpreadHelper.onShoot(gun)
            local bullet = gun:addBullet()
            bullet:addTrailParticle("PlasmaTrailParticle", Vec2.new(0.0, 0.0), 3.0, {color = Vec3.new(0.0, 0.6, 0.8)})
        end
    end
end

function PlasmaGun.onBulletHit(gun, bullet, monster)
    local data = gun.data
    if monster ~= nil then
        local m = monster
        local oldGameObjectPos = nil
        local count = 0
        
        if monster.isDead then
            count = count + 1
        end
        while m ~= nill do
            m:addIgnoreId(bullet.id)
            local gameObject = addGameObject("FadeOutImage")
            gameObject.data.startTime = time
            gameObject.data.fadeOutStartTime = 0.0
            gameObject.data.fadeInDuration = 0.0
            gameObject.data.fadeOutDuration = 0.3
            gameObject:setLevel(RenderableLevel.monsters + 5)
            gameObject.data.renderable = gameObject:addTexture(PlasmaGun.basePath .. "bullet.png", "~/resources/default")
            gameObject.data.renderable:setAlignment(RenderableAlignment.world)
            gameObject.data.renderable:setWorldMatrix(Mat3.fromScale(6.0, 6.0))
            if oldGameObjectPos ~= nil then
                gameObject:setPosition(m.position)
                local gameObject2 = addGameObject("FadeOutImage")
                gameObject2.data.startTime = time
                gameObject2.data.fadeOutStartTime = 0.0
                gameObject2.data.fadeInDuration = 0.0
                gameObject2.data.fadeOutDuration = 0.3
                gameObject2:setLevel(RenderableLevel.monsters + 4)
                gameObject2.data.renderable = gameObject2:addTexture(PlasmaGun.basePath .. "line.png", "~/resources/default")
                
                gameObject2.data.renderable:setAlignment(RenderableAlignment.world)
                gameObject2.data.renderable:setWorldMatrix(Mat3.from(
                (m.position + oldGameObjectPos) * 0.5,
                Vec2:new((m.position - oldGameObjectPos):length() * 0.5 - 3, 3.0),
                 -(m.position - oldGameObjectPos):getAngle()
                ))
                
                
                local args = {doNotStun = true}
                m:doDamageWithArgs(math.floor( ( 10 + 10 * math.random() ) * player.damageMultiplier:getBuffedValue() ), (m.position - oldGameObjectPos):normalized(), args)
            
                if m.isDead then
                    count = count + 1
                end
                if math.random() < 0.5 then
                    oldGameObjectPos = m.position
                end
            else 
                gameObject:setPosition(bullet.position)
                oldGameObjectPos = bullet.position
            end
            
            m = getClosestMonsterInRangeWithIgnoreId(bullet.position, 80.0, {bullet.id})
        end

        if data.checkAchievement then
            if hasAchievement( "ACH_PLASMA_GUN" ) or player.isDead or missionData.isSurvival ~= true then
                data.checkAchievement = false
                return
            end
            if count >= 3 then 
                data.achievementProcess = data.achievementProcess + 1
                
                if data.achievementProcess >= 20 then
                    addAchievement( "ACH_PLASMA_GUN" )
                    data.checkAchievement = false
                end
            end
        end
    end
end
