

ObjectiveBar = {}

function ObjectiveBar.init(gameObject, args)
    gameObject:setLevel(RenderableLevel.GUI + 5)
    
    gameObject.data.barBG = gameObject:addTexture("~/resources/missions/campaign/objective_bar/bar_bg.png", "~/resources/default")
    gameObject.data.barBG:setAlignment(RenderableAlignment.bottom)
    gameObject.data.barBG:setWorldMatrix(Mat3.fromScale(238.0, 16.0))
    
    
    gameObject.data.bar = gameObject:addTexture("~/resources/missions/campaign/objective_bar/bar.png", "~/resources/missions/campaign/objective_bar/bar")
    gameObject.data.bar:setAlignment(RenderableAlignment.bottom)
    gameObject.data.bar:setWorldMatrix(Mat3.fromScale(238.0, 16.0))
    gameObject.data.bar:setColor(Vec4.fromColor(0x44FFFFFF))
    
    gameObject.data.barFG = gameObject:addTexture("~/resources/missions/campaign/objective_bar/bar_fg.png", "~/resources/default")
    gameObject.data.barFG:setAlignment(RenderableAlignment.bottom)
    gameObject.data.barFG:setWorldMatrix(Mat3.fromScale(250.0, 26.0))
    
    
    local text = ""
    if args.text ~= nil then
        text = args.text
    end
    gameObject.data.text = gameObject:addText(text)
    gameObject.data.text:setAlignment(RenderableAlignment.bottom)
    gameObject.data.text:setTextAlignment(TextAlignment.center)
    gameObject.data.text:setVerticalTextAlignment(VerticalTextAlignment.mid)
    gameObject.data.text:setTextSize(42.0)
    gameObject.data.text:setWorldMatrix(Mat3.fromPosition(0.0, -60.0))
    gameObject.data.text:setColor(Vec4.new(1.0, 1.0, 1.0, 1.0))
    
    
    
    gameObject:setPosition(Vec2.new(0, 40))
    
    ObjectiveBar.updateBar(gameObject, 1.0)
    ObjectiveBar.repositionGUI(gameObject)
    
    if args.noBar == true then
        gameObject.data.barBG:setVisible(false)
        gameObject.data.bar:setVisible(false)
        gameObject.data.barFG:setVisible(false)
    end
end

function ObjectiveBar.repositionGUI(gameObject, size)
    gameObject:setPosition(Vec2.new(0, 60 / getCameraZoom()))
    gameObject:setScale(Vec2.new(0.5 / getCameraZoom(), 0.5 / getCameraZoom()))
end


function ObjectiveBar.updateBar(gameObject, scale)
    gameObject.data.bar:setUniformFloat("uScale", scale)
end

function ObjectiveBar.onTick(gameObject)
    if missionData ~= nil and missionData.maxObjective ~= nil and missionData.curObjective ~= nil then
        ObjectiveBar.updateBar(gameObject, missionData.curObjective / missionData.maxObjective )
    end
end