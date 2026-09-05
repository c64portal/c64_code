OSCAR64 ?= /Users/tomek/c64/oscar64/bin/oscar64
NCFTPPUT ?= ncftpput
U64 ?= u64.pl

TARGET := main
SRC := $(TARGET).c
PRG := $(TARGET).prg
DEPS := $(SRC) fade.c fade.h pic.prg women.fade

OSCAR_FLAGS ?= -n -O2

FTP_HOST ?= u64
FTP_DIR ?= /USB0/Temp

.PHONY: all build upload u64 clean

all: build

build: $(PRG)

$(PRG): $(DEPS)
	$(OSCAR64) $(OSCAR_FLAGS) $(SRC)

upload: $(PRG)
	$(NCFTPPUT) $(FTP_HOST) $(FTP_DIR) $(PRG)

u64: upload
	$(U64) $(FTP_HOST) -c run:$(PRG)

clean:
	rm -f $(TARGET).prg \
		$(TARGET).lbl \
		$(TARGET).int \
		$(TARGET).asm \
		$(TARGET).map \
		$(TARGET).dbj \
		$(TARGET).out \
		$(TARGET).crt
