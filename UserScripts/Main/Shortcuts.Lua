newkey = ""
x = 0

function init()
	
	return 0
end

function update() --updates GameLoop speed
	if newkey == "F10"then 
		SetCameraPos(0, 1, 0)
	end
	if newkey == "Right Shift" then
	SetCameraSpeed(0)
	end
end

function UpdateDelta() --updates at 60hz deltatime
	if newkey == "Page Up" then
		x = x + 1
		io.write("frag++")
		SetShader("frag", x)
		io.write(""..x.."\n")
	end
	if newkey == "Page Down" then
		x = x - 1
		io.write("frag--")
		SetShader("frag", x)
		io.write(""..x.."\n")
	end
		if (x >= 4) then 
			x = 0
		elseif x < 0 then
		x = 4
	end

	if newkey == "["then 
		SetSkybox("Assets/Skybox/Kloofendal48dPartlyCloudyPuresky/Skybox.json")
	end

	if newkey == "]"then 
		SetSkybox("Assets/Skybox/OvercastSoilPuresky/Skybox.json")
	end

return 12
end


function FetchKey(key)
	newkey = key
	--io.write("[LUA] FetchKey: "..key.."\n")
end