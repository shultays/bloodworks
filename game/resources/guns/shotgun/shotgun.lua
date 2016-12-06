
function Shotgun.init(gun)
	gun.spreadAngle = 0.15
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.85)
end


function Shotgun.onTick(gun)
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			for i = 1, 6 do
				local bullet = gun:addBullet()
				bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 0.0), 0.015, {})
				bullet.moveSpeed = bullet.moveSpeed * (math.random() * 0.3 + 0.7)
			end
		end
	end
end
