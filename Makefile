VERSION = $(shell grep '^Version' ChangeLog | head -n 1 | cut -d' ' -f2 | tr -d ' ')
BIN = telak
O = telak.o fetch.o image.o parse.o toon.o
LDFLAGS = $(shell imlib2-config --libs) $(shell curl-config --libs) -lgcrypt
CFLAGS = -W -Wall $(shell curl-config --cflags) $(shell imlib2-config --cflags) -DTELAK_USER_AGENT="\"$(BIN) $(VERSION)\"" -DTELAK_VERSION="\"$(VERSION)\"" -g

BINDIR=$(DESTDIR)/usr/bin
MANDIR=$(DESTDIR)/usr/share/man/man1
INSTALL = install
MAN=telak.1

$(BIN): $(O)
	$(CC) $(LDFLAGS) -o $(BIN) $(O)

install: $(BIN)
	$(INSTALL) -d -m 755 $(BINDIR)
	$(INSTALL) -m 755 $(BIN) $(BINDIR)

	$(INSTALL) -d -m 755 $(MANDIR)
	$(INSTALL) -m 644 $(MAN) $(MANDIR)

clean:
	rm -f *~ $(O) $(BIN)

release: clean
	mkdir ../$(BIN)-$(VERSION)
	cp -a * ../$(BIN)-$(VERSION)
	cd .. && tar czf $(BIN)-$(VERSION).tar.gz $(BIN)-$(VERSION)
	rm -rf ../$(BIN)-$(VERSION)
