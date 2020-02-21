# ssd_sim

1. Running code

```
	mkdir build
	cd build
	cmake ../
	make
	./test
```

This repo contains multiple files that form the building block of an SSD - cell, page and block.

We want to build an SSD using different block/page and cell APIs provided in different \*.hpp files.

We use the gtest framework to test SSD code.

To see how each of the components can be called for reading and writing, please refer test.cpp.

While adding code, please also add relevant tests to the test.cpp file that test the newly added code.

![SSD Layout](fig/SSDSimulatorLayout.png)

2. Configuring Qt

Qt can be installed using the following:

```
sudo apt-get install build-essential
sudo apt-get install qtcreator
sudo apt-get install qt5-default
```

To build a Qt project:

```
qmake -project
qmake
make
```
