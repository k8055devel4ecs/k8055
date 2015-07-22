# little example for k8055
# blink output
# version 1
import sys,os
import pyk8055lib
import time

devname ="K8055$"

myk8055obj =  pyk8055lib.K8055LibClass(devname)

time.sleep(1)
i = 0
while i < 100:
	i = i + 1
	myk8055obj.WriteData(0x03,0,0)
	print("03")
	time.sleep(5)
	myk8055obj.WriteData(0xF0,0,0)
	print("F0")
	time.sleep(5)
	myk8055obj.WriteData(0xF1,0x40,130)
	print("0xF1,0x40,130")
	time.sleep(5)
	erg = myk8055obj.ReadData()
	print('read data1 ',erg[0],erg[1],erg[2],erg[3])
	time.sleep(5)
	myk8055obj.WriteData(0x42,0x80,200)
	print("0x42,0x80,200")
	time.sleep(5)
	erg = myk8055obj.ReadData()
	print('read data2 ',erg[0],erg[1],erg[2],erg[3])
	time.sleep(5)
exit

