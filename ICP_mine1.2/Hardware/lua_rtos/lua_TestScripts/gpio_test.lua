print("\r\ngpio_test\r\n")

pio.mode(pio.LED2,pio.OUTPP,pio.PULLUP)
while(true)
do
  pio.toggle(pio.LED2)
  tmr.delayms(1000)
end
