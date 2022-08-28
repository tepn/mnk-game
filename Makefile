# Variables
EXE=mnk-game
EXEGDB=$(EXE)-debug

# Rules and targets
.PHONY: all build debug clean test help

all: build

build:
	@cd src && $(MAKE)
	@cp -f src/$(EXE) .

debug:
	@cd src && $(MAKE) debug
	@cp -f src/$(EXEGDB) .

clean:
	@cd src && $(MAKE) clean
	@cd test && $(MAKE) clean
	@rm -f $(EXE) $(EXEGDB) *~

test:
	@cd test && $(MAKE)

help:
	@echo "help mnk-game/:"
	@echo
	@echo "-------------------"
	@echo "  Usage  "
	@echo "-------------------"
	@echo " make [all]    Build the software"
	@echo " make clean    Remove all files generated by make"
	@echo " make test     Run tests"
	@echo " make help     Display this help"
	@echo "-------------------"
