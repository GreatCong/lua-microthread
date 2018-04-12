print("\r\ngpio_test\r\n")

pio.mode(pio.LED2,pio.OUTPP,pio.UP)
while(true)
do
  pio.toggle(pio.LED2)
  pio.delay(1000)
end
