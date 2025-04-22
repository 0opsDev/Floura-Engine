

function init()
	io.write("Main.Lua Running init \n")

	return 0
end

function update() --updates GameLoop speed


	io.write("[LUA S5] GameLoop returned ""\n")
end


function UpdateDelta() --updates at 60hz deltatime
	io.write("[LUA S5] UpdateDelta returned ""\n")
end

function runOnce() --updates at 60hz deltatime
	io.write("[LUA S5]  ran once""\n")
end