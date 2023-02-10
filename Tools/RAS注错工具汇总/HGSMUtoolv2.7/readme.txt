version v2.7
Build Date: 03/29 2019
Developed by Li Kuang.


1.add supports for two DM1 sockets

version v2.5
Build Date: 12/13 2018
Developed by Li Kuang.


1.add vol&power\clk display
2.change to log all data for AGM

History:
version v2.4
Build Date: 6/10 2018
Developed by Li Kuang.


1.add pkgtype dm1
2.change the log on unsec chip

---------------------------------

version v2.1
Build Date: 5/10 2018
Developed by Li Kuang.

Functions,

	1 Show SMU version in the top of taskbar.
	
2 Show cores effective frequency, real frequency, C0, CC1 and CC6 state.
	
3 Show SOC power consumption.
	4 Show Expectant VDDCR_CPU and actual VDDCR_CPU.

commands,
	
-j, show the next line.
	-u, show the previous line.
	
-l, open or close saving log function.
	-c, enter command mode to set parameters.
		
-vol/0/1.2, force Package0 CPU voltage to 1.2v
		-vol/4/1.2, force Package1 CPU voltage to 1.2v
		
-vol/0/-1,  to unforce.
		-clk/0/3.0, force all cores clock.
		-clk/0/-1,  unforce clock
	
-q or ctrl+c, quit

