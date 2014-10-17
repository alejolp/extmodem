##########################################################################
## Change macros below according to your environment and your needs
##
##########################################################################

# Review (and if necessary) change these if you are going to 
# install directly by using this makefile

   INSTALL_BIN = $(DESTDIR)/usr/bin

   


##################################################
##  things below should not be changed
##
##################################################


all: compile

install: bin/extmodem
	install -d $(INSTALL_BIN)
	install -m 755 bin/extmodem $(INSTALL_BIN)


compile: 
	python build.py
	


clean:
	rm -R bin
