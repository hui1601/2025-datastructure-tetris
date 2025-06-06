CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g -std=c11
LDFLAGS =
INCLUDES = -Iinclude

SRCDIR = src
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

TARGET = tetris
TARGET_PLATFORM := UNKNOWN
UNAME_S_UPPER := $(shell uname -s | tr 'a-z' 'A-Z')

ifneq (,$(findstring MINGW,$(UNAME_S_UPPER)))
	TARGET_PLATFORM := WINDOWS
else ifneq (,$(findstring MSYS,$(UNAME_S_UPPER)))
	TARGET_PLATFORM := WINDOWS
else ifeq ($(UNAME_S_UPPER),LINUX)
	TARGET_PLATFORM := UNIX
else ifeq ($(UNAME_S_UPPER),DARWIN)
	TARGET_PLATFORM := APPLE
endif

ifeq ($(TARGET_PLATFORM),UNKNOWN)
	ifeq ($(OS),Windows_NT)
		TARGET_PLATFORM := WINDOWS
	else
		TARGET_PLATFORM := UNIX
	endif
endif

ifeq ($(TARGET_PLATFORM),WINDOWS)
	TARGET_EXE = $(TARGET).exe
	RM = rm -f
	MKDIR_P = mkdir -p
	CFLAGS += -D_WIN32
else ifeq ($(TARGET_PLATFORM),UNIX)
    TARGET_EXE = $(TARGET)
    RM = rm -f
    MKDIR_P = mkdir -p
    CFLAGS += -D__unix__
else ifeq ($(TARGET_PLATFORM),APPLE)
    TARGET_EXE = $(TARGET)
    RM = rm -f
    MKDIR_P = mkdir -p
    CFLAGS += -D__APPLE__
else
    $(error "Unsupported target platform or detection failed! UNAME_S: $(UNAME_S), OS: $(OS)")
endif

all: $(TARGET_EXE)

$(TARGET_EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

run: $(TARGET_EXE)
	./$(TARGET_EXE)

clean:
	@echo "Cleaning up..."
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(TARGET_EXE)
	-$(RM) result.txt
	-$(RM) -rf $(OBJDIR)
	@echo "Cleanup complete."

.PHONY: all clean run

.SILENT: clean