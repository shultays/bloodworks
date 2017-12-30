
function Tutorial.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    missionData.state = 0
    missionData.t = 0
    missionData.objective = 0
    
    missionData.congratsTime = 1.3
    missionData.dontSpawnBonus = true
    
    missionData.extraMin = 0.0
    
    missionData.monsterC = 90
    missionData.xpPerMonster = math.floor(player.experienceForNextLevel / missionData.monsterC)
    
    missionData.perkPerLevel = 4
end

function Tutorial.swapText()
    if missionData.text == missionData.text1 then
        missionData.text = missionData.text2
    else
        missionData.text = missionData.text1
    end
end

function Tutorial.getDummy()
    local monster = addMonster("Alien")
    monster.data.maxMoveSpeed = 0.0
    monster.animationSpeed = 0.0
    monster.moveSpeed = 0.0
    monster.data.maxRotateSpeed = 0.0
    monster.data.canHit = false
    monster.position = player.position + Vec2.new(100.0, 100.0)
    
    monster.experienceMultiplier = 0.0
    monster.dropChance = 0.0
    monster:setScale(1.0)
    monster.colorMultiplier:clear()
        
    local buff = monster.colorMultiplier:addBuff(Vec4.fromColor(0x00FFFFFF))
    monster.colorMultiplier:setBuffDuration(buff, 0.3)
    monster.colorMultiplier:setBuffFadeInFadeOut(buff, 0.0, 0.3)

    missionData.objective = missionData.objective + 1
    return monster
end

function Tutorial.onTick()

    if missionData.state == 0 then
        
        missionData.textObject = addGameObject("")
        
        missionData.textObject:setLevel(RenderableLevel.GUI + 500)
        missionData.textObject:setPosition(Vec2.new(0, 120))
        
        missionData.textBG = missionData.textObject:addTexture(missionPath .. "textbg.png", "~/resources/default")
        missionData.textBG:setAlignment(RenderableAlignment.bottom)
        missionData.textBG:setWorldMatrix(Mat3.fromScale(385.0, 95.0))

        missionData.text1 = missionData.textObject:addText("Welcome to Bloodworks!\nThis tutorial will teach you the basics of the game", "resources/fontData.txt")
        missionData.text1:setAlignment(RenderableAlignment.bottom)
        missionData.text1:setTextAlignment(TextAlignment.center)
        missionData.text1:setVerticalTextAlignment(VerticalTextAlignment.mid)
        missionData.text1:setTextSize(20.0)
        missionData.text1:setWorldMatrix(Mat3.fromPosition(0.0, 10.0))
        missionData.text1:setColor(Vec4.new(1.0, 1.0, 1.0, 0.0))
        missionData.t1a = 0.0
        
        missionData.text2 = missionData.textObject:addText("", "resources/fontData.txt")
        missionData.text2:setAlignment(RenderableAlignment.bottom)
        missionData.text2:setTextAlignment(TextAlignment.center)
        missionData.text2:setVerticalTextAlignment(VerticalTextAlignment.mid)
        missionData.text2:setTextSize(20.0)
        missionData.text2:setWorldMatrix(Mat3.fromPosition(0.0, 10.0))
        missionData.t2a = 0.0
        
        missionData.text = missionData.text1
        missionData.state = 10
    
    elseif missionData.state ==  10 then
    
        if missionTime > 3.0 then
            Tutorial.swapText()
            missionData.text:setText("Use WASD to move around\nCollect all gifts to continue!")
            
            missionData.gifts = {}
            local pos = player.position
            for i = 0, 6 do
                local gameObject = addGameObject("")
                
                gameObject:setLevel(RenderableLevel.player - 5)
                gameObject:setPosition(pos + Vec2.fromAngle( i * math.pi * 2.0 / 7) * 150.0 )
                gameObject:setScale( Vec2.new(0.0, 0.0) )
                gameObject.data.rotation = math.pi * 2.0 * math.random()
                gameObject.data.rotationShift = math.pi * 2.0 * math.random()
                gameObject.data.s = 0.0
                local t = gameObject:addTexture(missionPath .. "gift_box.png", "~/resources/default")
                t:setWorldMatrix(Mat3.fromScale(18.0, 18.0))
                
                gameObject.data.particle = gameObject:addParticle("CriticalParticle", {})
                
                missionData.gifts[i] = gameObject
            end
            
            missionData.state = 20
        end
        
    elseif missionData.state ==  20 then
        local hasGift = false
        
        for i = 0, 6 do
            local gameObject = missionData.gifts[i] 
            if gameObject ~= nil then
                hasGift = true
            
                gameObject.data.s = clamp(gameObject.data.s)
                gameObject:setScale( Vec2.new(gameObject.data.s, gameObject.data.s) )
       
                gameObject:setRotation(gameObject.data.rotation + math.sin(gameObject.data.rotationShift + time * 2.0 ) * math.pi * 0.2 )
                
                if gameObject.data.picked ~= nil then
                    gameObject.data.s = gameObject.data.s - dt * 8.0
                    
                    if gameObject.data.s < 0.0 then
                        gameObject.toBeRemoved = true
                        missionData.gifts[i]  = nil
                    end
                else
                    if dt > 0.0 then
                        gameObject.data.particle:addParticle(gameObject:getPosition() + Vec2.fromAngle(math.random() * math.pi * 2.0) * math.random() * 20.0, {})
                    end
                    
                    gameObject.data.s = gameObject.data.s + dt * 4.0
                    
                    if player.position:distance(gameObject:getPosition()) < 25.0 then
                        gameObject.data.picked = true
                        playSound({path = missionPath .. "gifttake.ogg", volume = 0.5})
                    end
                end
            end
        end
    
        if hasGift == false then
            Tutorial.swapText()
            missionData.text:setText("Congrats!")
            missionData.state = 30
            missionData.t = missionTime
        end
    elseif missionData.state ==  30 then
        if missionTime - missionData.t > missionData.congratsTime then
            Tutorial.swapText()
            missionData.text:setText("Use mouse to aim and left mouse button to shoot.\nKill all enemies to continue")
            
            clearBloods()
            for i = 0, 8 do
                local monster = Tutorial.getDummy()
                
                monster.position = player.position + Vec2.fromAngle( i * math.pi * 2.0 / 9) * 150.0 
                monster.moveAngle = (player.position - monster.position):getAngle()
            end
            
            missionData.state = 40
        end
        
    elseif missionData.state == 40 then
        if missionData.reload == nil and player.gun:isReloading() then
            missionData.reload = true
            Tutorial.swapText()
            missionData.text:setText("Your weapon will automatically start reloading when\nit is out of ammo and there is no reload limit.\nYou can also reload manually by pressing R.")
        end
    
        if missionData.objective == 0 then
            Tutorial.swapText()
            missionData.text:setText("Congrats!")
            missionData.state = 50
            missionData.t = missionTime
        end
    elseif missionData.state == 50 then
        if missionTime - missionData.t > missionData.congratsTime then
            Tutorial.swapText()
            missionData.text:setText("Monsters will randomly drop weapons when they die.\nGo near weapon to equip it.")
            missionData.state = 60
        end
    elseif missionData.state == 60 then
        if player.gun.scriptName == "PlasmaGun" then
            Tutorial.swapText()
            missionData.text:setText("'Plasma Gun' will work better against group of enemies.\nKill all enemies to continue.")
            missionData.state = 70
            
            clearBloods()
            for i = 0, 6 do
                for j = 0, 3 do
                    local monster = Tutorial.getDummy()
                    monster:setScale(0.7)
                    monster.position = player.position + Vec2.fromAngle( i * math.pi * 2.0 / 7) * 160  + Vec2.fromAngle(math.random() * math.pi * 2.0) * math.sqrt( math.random() ) * 50.0
                    monster.moveAngle = math.random() * math.pi * 2.0
                    monster.hitPoint = 20
                end
            end
        end
        
        
    elseif missionData.state == 70 then
        if missionData.objective == 0 then
            Tutorial.swapText()
            missionData.text:setText("Congrats!")
            missionData.state = 80
            missionData.t = missionTime
        end
    elseif missionData.state == 80 then
        if missionTime - missionData.t > missionData.congratsTime then
            Tutorial.swapText()
            missionData.text:setText("You can equip ultimate weapons. These weapons have\nlimited ammo, long cooldowns between each shot.\nPick up the ultimate.")
            missionData.state = 90
        end
    elseif missionData.state == 90 then
        if player.ultimate ~= nil then
            Tutorial.swapText()
            missionData.text:setText("Aim at enemies and\nuse right mouse button to activate your ultimate.")
            missionData.state = 100
            clearBloods()
            for i = 0, 50 do
                    local monster = Tutorial.getDummy()
                    monster:setScale(0.7)
                    monster.position = player.position + Vec2.fromAngle( player.aimDir:getAngle() + math.pi + ( math.pi * 0.35 * (math.random() - 0.5) ) ) * (120 + math.random() * 200)
                    monster.moveAngle = math.random() * math.pi * 2.0
            end
        end
    elseif missionData.state == 100 then
        if missionData.objective == 0 then
            Tutorial.swapText()
            missionData.text:setText("Congrats!")
            missionData.state = 110
            missionData.t = missionTime
        end
    elseif missionData.state == 110 then
        if missionTime - missionData.t > missionData.congratsTime then
            Tutorial.swapText()
            missionData.text:setText("Sometimes a boss monster will appear on the map.\nThere are different types of bosses.\nThis particular boss has large amount of HP.")
            missionData.state = 120
            
                    
            local monster = addMonster("Alien")
            monster.data.canHit = false
            monster.position = player.position - player.aimDir * 400.0
            monster.moveAngle = (player.position - monster.position):getAngle()
                
            makeBossDefault(monster, 1)
            monster.data.randomMove = false
            local buff = monster.colorMultiplier:addBuff(Vec4.fromColor(0x00FFFFFF))
            monster.colorMultiplier:setBuffDuration(buff, 0.3)
            monster.colorMultiplier:setBuffFadeInFadeOut(buff, 0.0, 0.3)

            monster.experienceMultiplier = 0.0
            
            missionData.objective = missionData.objective + 1
        end
    elseif missionData.state == 120 then
        if missionData.objective == 0 then
            Tutorial.swapText()
            missionData.text:setText("Congrats!")
            missionData.state = 130
            missionData.t = missionTime
        end
    elseif missionData.state == 130 then
        if missionTime - missionData.t > missionData.congratsTime then
            Tutorial.swapText()
            missionData.text:setText("It looks like you almost gained\nenough experience to level up!\nKill this last monster to level up.")
            missionData.state = 140
            
            local monster = Tutorial.getDummy()
            monster.position = player.position - player.aimDir * 200.0
            monster.moveAngle = (player.position - monster.position):getAngle()
            
        end
    elseif missionData.state == 140 then
        if missionData.objective == 0 then
            Tutorial.swapText()
            missionData.text:setText("After leveling up, you can take a perk.\nPerks will give permanent bonuses to your character.\nPick a perk to continue.")
            missionData.state = 150
        end
    elseif missionData.state == 150 then
        if numActivePerks() == 1 then
            Tutorial.swapText()
            missionData.text:setText("Congrats!")
            missionData.state = 160
            missionData.t = missionTime
        end
    elseif missionData.state == 160 then
        if missionTime - missionData.t > missionData.congratsTime then
            Tutorial.swapText()
            missionData.text:setText("Kill all enemies to finish the tutorial!")
            missionData.state = 170
            missionData.t = missionTime
        end
    elseif missionData.state == 170 then
        if missionTime - missionData.t > missionData.congratsTime then
        
            missionData.t = missionTime
            for i = 0, 29 do
                local monster = addMonster("Alien")
                monster.data.canHit = true
                
                monster.position = player.position + Vec2.fromAngle( math.pi * 2.0 * math.random() ) * (250 + math.random() * 300)
                monster.moveAngle = math.random() * math.pi * 2.0
                    
                monster.experienceMultiplier = 0.0
                
                missionData.objective = missionData.objective + 1
            
                monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.6
                monster.data.moveSpeed = 0.0
                monster.hitPoint = 40 

                monster.data.minDamage  = 3
                monster.data.maxDamage  = 5
                monster.data.hitInterval  = 3.0
                
                monster.data.randomMove = true
                monster.data.targetShift = Vec2.new(0.0, 0.0)
            
                
                local buff = monster.colorMultiplier:addBuff(Vec4.fromColor(0x00FFFFFF))
                monster.colorMultiplier:setBuffDuration(buff, 0.3)
                monster.colorMultiplier:setBuffFadeInFadeOut(buff, 0.0, 0.3)
                
            end
            missionData.dontSpawnBonus = nil
            
            missionData.state = 180
        end
        
    elseif missionData.state == 180 then
        missionData.text:setText("\nKill all enemies to finish the tutorial!\nRemaining : " .. getMonsterCount() )
        if missionTime - missionData.t > 10.0 then
            local count = getMonsterCount()
            
            for i = 0, count - 1 do
                local monster = getMonsterAtIndex(i)
                
                if missionTime - missionData.t > 10.0 + i / 4.0 then
                    monster.data.randomMove = false
                end
            end
        end
        
        
        if missionData.objective == 0 then
            showGameReset("Congrats!")
            missionData.state = 190
            missionData.textObject.toBeRemoved = true
            missionData.ended = true
        end
    end
    
    
    if missionData.ended ~= true then
        if missionData.text == missionData.text1 then
            missionData.t1a = missionData.t1a + nonsloweddt * 8.0
            missionData.t2a = missionData.t2a - nonsloweddt * 8.0
        else
            missionData.t1a = missionData.t1a - nonsloweddt * 8.0
            missionData.t2a = missionData.t2a + nonsloweddt * 8.0
        end
        
        missionData.t1a = clamp(missionData.t1a)
        missionData.t2a = clamp(missionData.t2a)

        missionData.text1:setColor(Vec4.new(1.0, 1.0, 1.0, missionData.t1a))
        missionData.text2:setColor(Vec4.new(1.0, 1.0, 1.0, missionData.t2a))
    end
    
    if isKeyReleased(keys.Delete) and false then    
        local count = getMonsterCount()
        
        for i = 0, count - 1 do
            local monster = getMonsterAtIndex(i)
            monster.experienceMultiplier = 0.0
            monster:doDamage(10000, Vec2.randDir())
        end
    end
    
    gameResetTick()
end


function Tutorial.onMonsterDied(monster)
    missionData.objective = missionData.objective - 1
    if missionData.state ==  40 and missionData.objective == 0 then
        spawnGun(monster.position, "PlasmaGun")
    end
    
    if missionData.state ==  70 and missionData.objective == 13 then
    
        Tutorial.swapText()
        missionData.text:setText("Bonuses/weapons are also randomly spawns at map.\nPick up the bonus.")
        
        local v = Vec2.new(0.0, 0.0)
        
        for i = 0, getMonsterCount() - 1 do
            v = v + getMonsterAtIndex(i).position
        end
        
        v = v / getMonsterCount()
        spawnBonus( (player.position * 0.2 + v  * 0.8) , "Explosion")
    end
    
    if missionData.state ==  70 and missionData.objective == 0 then
        spawnGun(monster.position, "RocketBarrage") 
    end
    
    if missionData.p == nil then
        missionData.p  = 1
    else
        missionData.p  = missionData.p  + 1
    end
    if missionData.state == 140 then
        player:gainExperience(player.experienceForNextLevel - player.experience + 1)
    else 
        player:gainExperience(missionData.xpPerMonster)
    end
end

function Tutorial.onPlayerDied()
    showGameReset()
end

function Tutorial.getPerksForLevelUp()
    return {"CriticalBullets", "PiercingBullets", "LessSpread", "ExplosionOnReload"}
end