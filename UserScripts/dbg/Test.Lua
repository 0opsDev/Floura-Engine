

function init()
	io.write("Main.Lua Running init \n")

	return 0
end

function update() --updates GameLoop speed

end


function UpdateDelta() --updates at 60hz deltatime

	return 60
end

function runOnce() --updates at 60hz deltatime
	io.write("[LUA S5] ran once\n")
end