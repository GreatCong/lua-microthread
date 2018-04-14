-- Function to execute
f1 = function()
for i=1,10 do
	print("ok1-",i)
	thread.sleep(1)
end
end

f2 = function()
for i=1,10 do
	print("ok2-",i)
	print(thread.status(th1))
	thread.sleep(1)
end
end

-- LED init
pio.mode(pio.LED2,pio.OUTPP,pio.PULLUP)
-- Start thread
th1 = thread.start(f1,512,3)
th2 = thread.start(f2)
thread.list()

while(thread.status(th1)=="running")
do
  pio.toggle(pio.LED2)
  tmr.delayms(1000)
end