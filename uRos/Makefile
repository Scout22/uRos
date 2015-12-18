DIRECTORY=exe
.PHONY: all lib_fastsim node_echo node_tele node_simu node_logger node_ia master clean


all: lib_fastsim node_echo node_tele node_simu node_logger master node_ia

lib_fastsim:
	$(MAKE) -C common_file/fastsim/
node_echo:
	$(MAKE) -C node_echo/
	cp node_echo/node_echo $(DIRECTORY)/node_echo
node_tele:
	$(MAKE) -C node_tele/
	cp node_tele/node_tele $(DIRECTORY)/node_tele	
node_simu:
	$(MAKE) -C node_simu/
	cp node_simu/node_simu $(DIRECTORY)/node_simu
node_logger:
	$(MAKE) -C node_logger/
	cp node_logger/node_logger $(DIRECTORY)/node_logger
master:
	$(MAKE) -C master/
	cp master/master $(DIRECTORY)/master
node_ia:
	$(MAKE) -C node_ia/
	cp node_ia/node_ia $(DIRECTORY)/node_ia

clean: 
	$(MAKE) -C common_file/fastsim/ clean & $(MAKE) -C node_echo/ clean & $(MAKE) -C node_tele/ clean & $(MAKE) -C node_simu/ clean & $(MAKE) -C node_logger/ & $(MAKE) -C master/ clean
	cd exe; rm master node_tele node_simu node_echo node_logger node_ia
