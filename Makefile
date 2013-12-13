os :
	make -f Makefile_OS clean
	make -f Makefile_OS all

task :
	make -f Makefile_Task clean
	make -f Makefile_Task all

test :
	make -f Makefile_SelfTest clean
	make -f Makefile_SelfTest all
	make -f Makefile_CrossTest clean
	make -f Makefile_CrossTest all

all: os task test

clean :
	make -f Makefile_OS clean
	make -f Makefile_Task clean
	make -f Makefile_SelfTest clean
	make -f Makefile_CrossTest clean

exe :
	cp bin/kernel.bin ~
	sudo chmod 777 /dev/ttyUSB0
