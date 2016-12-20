RenderableAlignment = {
	world = 0,
	topLeft = 1,
	top = 2,
	topRight = 3,
	left = 4,
	center = 5,
	right = 6,
	bottomLeft = 7,
	bottom = 8,
	bottomRight = 9
}

RenderableLevel = {
	background = 0,
	monsters = 1000,
	player = 2000,
	gameObjects = 3000,
	bullets = 4000,
	objectGUI = 5000,
	foreground = 6000,
	gui = 7000
}


function printTable(myTable)
	if myTable == nil then
		print("nil")
	else 
		for k, v in pairs( myTable ) do
		   print(k, v)
		end
	end
 end