SDKBUILD:=sdk/build
SDKSRC:=sdk/
.PHONY: all clean sdk src
all: sdk src

sdk: 
	mkdir $(SDKBUILD)
	cmake -S$(SDKSRC) -B$(SDKBUILD)
	$(MAKE) -C $(SDKBUILD)
src: 
	$(MAKE) -C src

clean: clean-sdk clean-src
	
clean-sdk:
	rm -rf $(SDKBUILD)

clean-src:
	$(MAKE) clean -C src