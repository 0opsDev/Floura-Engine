

function init()
	io.write("[LUA] Main.Lua Running init \n")
	return 0
end

function update() --updates GameLoop speed
--         path              name         culling trans  rot        scale 
--	ModelDraw("modelPath.gltf", "modelName", true, 1, 2, 3,  1, 2, 3, 4,  1, 1, 1)

--	ModelDraw("NOTTmodelPath.gltf", "NOTTmodelName", false, 4, 2, 0, 0, 0, 0, 0, 1, 1, 1)


end

function UpdateDelta() --updates at 60hz deltatime
--	SetWindow("vsync", "true")
--	SetWindow("Title", "Value")


	return 60
end

function FetchKey(key)
	--io.write("[LUA] FetchKey: "..key.."\n")
end