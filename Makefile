
NAME			:= hammockswing
VERSION_MAJ  	:= 1
VERSION_MIN  	:= 0
VERSION_GIT		:= $(shell git describe | cut -d - -f 2)
GITMODIFIED     := $(shell (git st | grep "modified:\|added:\|deleted:" -q) && echo "M")
VERSION      	:= $(VERSION_MAJ).$(VERSION_MIN)-$(VERSION_GIT)$(GITMODIFIED)

BIN		:= $(NAME)-$(VERSION).elf
FHEX	:= $(NAME)-$(VERSION)-flash.ihx
EHEX	:= $(NAME)-$(VERSION)-eeprom.ihx
ZIP		:= $(NAME)-$(VERSION).zip
ZIPSRC	:= $(NAME)-$(VERSION)-source.zip
BAT		:= $(NAME)-$(VERSION).bat

SRC		+= main.c
SRC		+= uart.c
SRC		+= event.c
SRC		+= timer.c
SRC		+= cmd.c
SRC		+= encoder.c

#
# Special options for each target
# 

CROSS		+= avr-
CFLAGS  	+= -DF_CPU=16384000
CFLAGS  	+= -mmcu=atmega328p -Os -I. -fshort-enums 
CFLAGS  	+= -DVERSION_GIT=\"$(VERSION_GIT)$(GITMODIFIED)\"
LDFLAGS 	+= -mmcu=atmega328p 

default: $(BIN) 

#############################################################################

OBJS		:= $(subst .c,.o, $(SRC))
DEPS		:= $(subst .c,.d, $(SRC))

CFLAGS  	+= -g -I. -I..
CFLAGS  	+= -Wall -Werror
CFLAGS  	+= -MMD
LDFLAGS 	+= -g
CFLAGS  	+= -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\"
ADFLAGS 	+= -p m328p -c avrispv2 -P usb -V -F -s

ifdef ccache
CROSS	:= ccache $(CROSS)
endif

CC 		:= $(CROSS)gcc
LCC 		:= gcc
LD 		:= $(CROSS)gcc
OBJCOPY 	:= $(CROSS)objcopy
OBJDUMP 	:= $(CROSS)objdump
STRIP		:= $(CROSS)strip
SIZE		:= $(CROSS)size
NSIS		:= makensis
AVRDUDE 	:= sudo avrdude


ifdef verbose
E	:= 
P	:= @true
else
E	:= @
P	:= @echo
endif


%.o: %.c
	$(P) " [CC   ] $<"
	$(E) $(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(P) " [LD   ] $@"
	$(E) $(LD) -o $@ $^ $(LDFLAGS)
	$(E) $(SIZE) $@

clean:
	$(P) " [CLEAN]" 
	$(E) rm -f *.o linux/*.o avr/*.o
	$(E) rm -f $(BIN) *.elf *.ihx tags $(BIN).elf *.zip *.bat

.PHONY: tags
tags:
	$(P) " [CTAGS]"
	$(E) ctags $(SRC)
	
$(FHEX) $(EHEX): $(BIN)
	$(P) " [OBJCP] $@"
	$(E) $(OBJCOPY) -j .text -j .data -O ihex $(BIN) $(FHEX)
	$(E) $(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $(BIN) $(EHEX)

install-flash: $(FHEX) $(EHEX)
	$(P) " [INST ] $^"
	$(E) $(AVRDUDE) $(ADFLAGS) -e -U flash:w:$(FHEX):i

install-eeprom: $(FHEX) $(EHEX)
	$(P) " [INST ] $^"
	$(E) $(AVRDUDE) $(ADFLAGS) -U eeprom:w:$(EHEX):i

install: install-flash install-eeprom

reset: $(FHEX) $(EHEX)
	$(P) " [INST ] $^"
	$(E) $(AVRDUDE) $(ADFLAGS) 

install-fuses: $(FHEX) $(EHEX)
	$(P) " [INST ] $^"
	$(E) $(AVRDUDE) $(ADFLAGS) -e -U hfuse:w:0xd9:m -U lfuse:w:0xd6:m  -U efuse:w:0x04:m

strip:	$(BIN)
	$(P) " [STRIP]"
	$(E) $(STRIP) $(BIN)

bat: $(BAT)

$(BAT): $(FHEX) $(EHEX)
	$(P) " [BAT  ] $@"
	$(E) ./mkprogram $^ > $@

zip: $(ZIP)
zipsrc: $(ZIPSRC)

$(ZIP):	$(BAT) $(FHEX) $(EHEX)
	$(P) " [ZIP  ] $@"
	$(E) zip -q $(ZIP) $^

$(ZIPSRC): clean
	$(P) " [ZIP  ] $@"
	$(E) zip -q $(ZIPSRC) *.c *.h Makefile

versions:
	git log | grep commit | awk '{print $$2}' | xargs git describe

-include $(DEPS)
