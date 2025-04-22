
--COUNT = 1

--runs at main.cpp init speed
function init()
io.write("Main.Lua Running init \n")

return 0
end

--updates at main.cpp loop speed
function update()
COUNT = COUNT + 1;

--io.write(""..COUNT.."\n")

end

--updates at 60hz deltatime
function UpdateDelta()
COUNT = COUNT + 1;

--io.write(""..COUNT.."\n")

end