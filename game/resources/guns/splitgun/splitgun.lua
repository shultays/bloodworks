
function SplitGun.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 400.0
	ShootTimer.initGun(gun, 0.45)
	
	SpreadHelper.initGun(gun)
	gun.data.minSpread = 0.02
	gun.data.maxSpread = 0.10
	gun.data.spreadDecreaseSpeed = 0.25
	gun.data.spreadIncreasePerShoot = 0.025
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
			
			playSound({path = "~/resources/sounds/split_gun.ogg"})
		end
	end
end



function SplitGun.onBulletHit(gun, bullet, monster)
	if bullet.data.remainingSplit > 0 then
		local newBullet = gun:addBullet()
		local particle = newBullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
		newBullet.penetrateCount = 2 - bullet.penetrateUsed
		particle.args.initialScale = 2.0
		particle.args.fadeOutSpeed = 1.2
		particle.args.startFadeinSpeed = 10000.0
		newBullet.position = bullet.position
		newBullet.moveAngle = bullet.moveAngle - math.pi * 0.1
		newBullet.data.remainingSplit = bullet.data.remainingSplit - 1
		monster:addIgnoreId(newBullet.id)
		
		newBullet = gun:addBullet()
		particle = newBullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
		newBullet.penetrateCount = 2 - bullet.penetrateUsed
		particle.args.initialScale = 2.0
		particle.args.fadeOutSpeed = 1.2
		particle.args.startFadeinSpeed = 10000.0
		newBullet.position = bullet.position
		newBullet.moveAngle = bullet.moveAngle + math.pi * 0.1
		newBullet.data.remainingSplit = bullet.data.remainingSplit - 1
		monster:addIgnoreId(newBullet.id)
		
		
		playSound({path = "~/resources/sounds/split_gun.ogg", position = bullet.position, volume = 0.5})
	end
end