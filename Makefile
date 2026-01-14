all:
	$(MAKE) -C kernel all

run:
	$(MAKE) -C kernel run

clean:
	$(MAKE) -C kernel clean