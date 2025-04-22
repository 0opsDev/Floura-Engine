

function init()
	io.write("Main.Lua Running init \n")

	return 0
end

function update() --updates GameLoop speed

ModelDraw("modelPath.gltf", "modelName", 1, 2, 3)

ModelDraw("NOTTmodelPath.gltf", "NOTTmodelName", 4, 2, 0)

	--io.write("[LUA S5] HostFunction returned "..r.."\n")
end


function UpdateDelta() --updates at 60hz deltatime

end