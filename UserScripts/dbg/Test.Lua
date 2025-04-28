

function init()
	io.write("Test.Lua Running init \n")

	return 0
end

function update() --updates GameLoop speed

end


function UpdateDelta()
	--io.write("[LUA S5] update\n")
	return 1
end

function runOnce() --updates at 60hz deltatime
	io.write("[LUA S5] ran once\n")
end