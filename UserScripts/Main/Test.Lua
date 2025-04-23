

function init()
	io.write("Main.Lua Running init \n")

	return 0
end

function update() --updates GameLoop speed

ModelDraw("modelPath.gltf", "modelName69420", true, 1, 2, 3,  1, 2, 3, 4,  1, 1, 1)

ModelDraw("NOTTmodelPath.gltf", "NOTTmodelName99", false, 4, 2, 0, 0, 0, 0, 0, 1, 1, 1)

end


function UpdateDelta() --updates at 60hz deltatime

end

function runOnce() --updates at 60hz deltatime
	io.write("[LUA S5] ran once\n")
end