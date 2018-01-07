FrostLaser.buffId = getGlobalUniqueId()

function FrostLaser.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 550.0
    gun.data.maxRangeUniform = gun.laser:addUniformFloat("maxRange", 350.0)
    gun.data.timeUniform = gun.laser:addUniformFloat("time", 0.0)
    ShootTimer.initGun(gun, 0.1)
    
    gun.data.checkAchievement = true
    gun.data.achievementProcess = 0
end

function FrostLaser.checkBuff(monster)
    if monster.moveSpeedMultiplier:hasBuffInfo(FrostLaser.buffId) == false then
        monster.moveSpeedMultiplier:addBuffWithId(FrostLaser.buffId, 1.0)
        local buff = monster.moveSpeedMultiplier:getBuffInfo(FrostLaser.buffId)
        buff:setBuffDuration(1.8)
        buff:setBuffFadeInFadeOut(-1.0, 0.3)
    end

    if monster.colorMultiplier:hasBuffInfo(FrostLaser.buffId) == false then
        monster.colorMultiplier:addBuffWithId(FrostLaser.buffId, Vec4:new(1.0, 1.0, 1.0, 1.0))
        local colorbuff = monster.colorMultiplier:getBuffInfo(FrostLaser.buffId)
        colorbuff:setBuffDuration(1.8)
        colorbuff:setBuffFadeInFadeOut(-1.0, 0.3)
    end
end


function FrostLaser.onTick(gun)
    gun.laser:setVisible(false)
    if gun.isTriggered and gun:hasAmmo() then
        gun.laser:setVisible(true)
        local range = 350.0
        local result = getClosestMonsterOnLine(player.gunPos, player.aimDir * range,  0.0, {gun = gun, ignoreFlags = CollisionFlags.NoBulletCollision})
        range = result.distance

        gun.laser:setLength(range)
        gun.laser:addUniformFloat("time", time)
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local monster = result.monster
            if monster ~= nil then
                local args = {doNotStun = true}
                monster:doDamageWithArgs(gun:getRandomDamage(), player.aimDir, args)
                
                
                FrostLaser.checkBuff(monster)
                local buff = monster.moveSpeedMultiplier:getBuffInfo(FrostLaser.buffId)
                local colorbuff = monster.colorMultiplier:getBuffInfo(FrostLaser.buffId)
              
                local amount = buff:getCurrentBuffAmount()
                amount = amount * 0.70
                
                if amount < 0.2 and monster.data.iceBlock == nil and monster.isDead == false then
                    monster.data.iceBlock = addGameObject("IceBlock", { monster = monster, gun = gun })
                end
                
                amount = math.max(amount, 0.2)
                
                buff:setBuffAmount(amount)
                buff:restart()
                
                amount = 1.0 - amount
                colorbuff:setBuffAmount(Vec4:new(1.0 - amount, 1.0 - amount * 0.8, 1.0 - amount * 0.2, 1.0))
                colorbuff:restart()
            end
        end
    end
end

IceBlock = {}

function IceBlock.init(gameObject, args)

    gameObject.data.renderable = gameObject:addTexture(FrostLaser.basePath .. "block.png", "~/resources/default")
    local s =  args.monster.collisionRadius * 1.2 + 15.0
    
    gameObject.data.renderable:setWorldMatrix(Mat3.fromScale(s, s))
    gameObject:setPosition(args.monster.position)
    gameObject:setRotation(math.pi * 2.0 * math.random() )
    
    gameObject.data.monster = args.monster
    gameObject.data.gun = args.gun
    gameObject.data.time = time
    
    gameObject.data.resetCollision = args.monster.hasCollision
    args.monster.hasCollision = false
    
    gameObject.data.colliderSize = s - 10.0
    gameObject.data.collider = gameObject:addCircleCollider( Vec2.new(0.0, 0.0), 0.0, CollisionFlags.NoBulletCollision)
    
    gameObject:setLevel(args.monster:getDrawLevel() + 5)
    
    gameObject.data.rotateSpeed = args.monster.data.maxRotateSpeed
    args.monster.data.maxRotateSpeed = 0.0
    
    gameObject.data.animationSpeed = args.monster.animationSpeed
    args.monster.animationSpeed = 0.0
    
    if args.monster.data.hasIceBlockOnKill == nil then
        args.monster.data.hasIceBlockOnKill = true
        addCustomOnKill(args.monster, function (monster)
            if monster.data.iceBlock ~= nil then
                monster.data.iceBlock.data.monster = nil
            end
        end)
    end
end

function IceBlock.onTick(gameObject)
    local data = gameObject.data
    
    if data.monster ~= nil then

        FrostLaser.checkBuff(data.monster)
        
        local buff = data.monster.moveSpeedMultiplier:getBuffInfo(FrostLaser.buffId)
        local colorbuff = data.monster.colorMultiplier:getBuffInfo(FrostLaser.buffId)
        
        buff:setBuffAmount(0.0)
        buff:restart()
    
        local amount = 0.8
        colorbuff:setBuffAmount(Vec4:new(1.0 - amount, 1.0 - amount * 0.8, 1.0 - amount * 0.2, 1.0))
        colorbuff:restart()
    end
    
    local t = time - data.time
    if t < 0.15 then
        data.renderable:setColor(Vec4.new(1.0, 1.0, 1.0, t / 0.15))
        relocateCircleCollider(gameObject.data.collider, gameObject:getPosition(), gameObject.data.colliderSize * t / 0.15)
    elseif t > 1.85 then
        data.renderable:setColor(Vec4.new(1.0, 1.0, 1.0, (2.0 - t) / 0.15))
    else
        data.renderable:setColor(Vec4.new(1.0, 1.0, 1.0, 1.0))
        relocateCircleCollider(gameObject.data.collider, gameObject:getPosition(), gameObject.data.colliderSize)
    end
    
    if t > 2.0 then
        gameObject.toBeRemoved = true
        if data.monster ~= nil then
            data.monster.data.iceBlock = nil
             
            local buff = data.monster.moveSpeedMultiplier:getBuffInfo(FrostLaser.buffId)
            local colorbuff = data.monster.colorMultiplier:getBuffInfo(FrostLaser.buffId)
            
            buff:setBuffAmount(1.0)
            buff:restart()
        
            colorbuff:setBuffAmount(Vec4:new(1.0, 1.0, 1.0, 1.0))
            colorbuff:restart()
            
            data.monster:doDamage(math.floor(30+20*math.random()), -data.monster.moveDir) 
            
            data.monster.hasCollision = data.resetCollision
    
            data.monster.data.maxRotateSpeed =  data.rotateSpeed
            data.monster.animationSpeed =  data.animationSpeed
            
            local gun = data.gun
            
        
            if gun.data.checkAchievement then
                if hasAchievement( "ACH_FROST_LASER" ) or player.isDead or missionData.isSurvival ~= true then
                    data.checkAchievement = false
                elseif data.monster.isDead then
                    gun.data.achievementProcess = gun.data.achievementProcess + 1
                    if gun.data.achievementProcess >= 40 then
                        addAchievement( "ACH_FROST_LASER" ) 
                    end
                end
           end
         end
    end
end

