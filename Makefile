CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g -Iinclude -w

TARGET = szci
SRCDIR = src
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

LIBS =

ifeq ($(OS),Windows_NT)
    LIBS = -lpdcurses
else
    LIBS = -lncurses -ltinfo
    CFLAGS += -D_GNU_SOURCE
endif

# Directories for install
LOCALBIN = $(HOME)/.local/bin
SYSTEMBIN = /usr/local/bin

all: $(BUILDDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

install:
ifeq ($(OS),Windows_NT)
	@echo "Install is not supported on Windows."
else
ifeq ($(shell id -u),0)
	@echo "Installing szci system-wide to $(SYSTEMBIN)/"
	mkdir -p $(SYSTEMBIN)
	install -Dm755 $(TARGET) $(SYSTEMBIN)/szci
	@echo "System-wide installation complete."
else
	@mkdir -p $(LOCALBIN)
	@echo "Installing szci locally to $(LOCALBIN)/"
	install -Dm755 $(TARGET) $(LOCALBIN)/szci
	@echo "Local installation complete. Make sure $(LOCALBIN) is in your PATH."
endif
endif

uninstall:
ifeq ($(OS),Windows_NT)
	@echo "Uninstall is not supported on Windows."
else
ifeq ($(shell id -u),0)
	@echo "Removing system-wide szci from $(SYSTEMBIN)/"
	rm -f $(SYSTEMBIN)/szci
	@echo "System-wide szci removed."
else
	@echo "Removing local szci from $(LOCALBIN)/"
	rm -f $(LOCALBIN)/szci
	@echo "Local szci removed."
endif
endif

.PHONY: all clean install uninstall
