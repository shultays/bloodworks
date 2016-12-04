
function SwastikaGun.init(gun)
	gun.spreadAngle = 0.10
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.05)
end


function SwastikaGun.onTick(gun)
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			local b = gun:addBullet();
			b.moveAngle = 0.0
			b.data.st = time
			
			b = gun:addBullet();
			b.moveAngle = 3.14159265
			b.data.st = time
			
			b = gun:addBullet();
			b.moveAngle = 3.14159265 * 0.5
			b.data.st = time
			
			b = gun:addBullet();
			b.moveAngle = 3.14159265 * 1.5
			b.data.st = time
		end
	end
end



function SwastikaGun.onBulletTick(gun, bullet)
	if bullet.data.rotated == nil and bullet.data.st < time - 0.5 then
		bullet.moveAngle = bullet.moveAngle - 3.14159265 * 0.5
		bullet.data.rotated = true
	end
end