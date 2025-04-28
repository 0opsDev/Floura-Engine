function init()
	io.write("[LUA] Main.Lua Running init \n")
	return 0
end

function update() --updates GameLoop speed
--         path              name         culling trans  rot        scale 
--	ModelDraw("modelPath.gltf", "modelName", true, 1, 2, 3,  1, 2, 3, 4,  1, 1, 1)

--	ModelDraw("NOTTmodelPath.gltf", "NOTTmodelName", false, 4, 2, 0, 0, 0, 0, 0, 1, 1, 1)


end
--x = 0
function UpdateDelta() --updates at 1hz deltatime
--x = x + 1

--SetShader("frag", x)
--io.write(""..x.."\n")

--if (x >= 4) then 
--x = 0
--end

	return 1
end