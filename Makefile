TARGET   = basicapp

CC       = gcc
DEFINES  = -DVK_NO_PROTOTYPES -DDEBUG
CFLAGS   = -std=c17 -Wall -g3
# DEFINES  = -DVK_NO_PROTOTYPES
# CFLAGS   = -std=c17 -flto -O3 -march=native

LINKER   = gcc -o
LFLAGS   = -lm -lSDL2
# LFLAGS   = -flto -O3 -march=native -lm -lSDL2

GLSL_CC    = glslc
GLSL_FLAGS =

# change these to set the proper directories where each files should be
SRCDIR   = src
OBJDIR   = build/obj
BINDIR   = build/bin

SHADER_DIR     = shaders
SHADER_SRC_DIR = $(SRCDIR)/$(SHADER_DIR)
SHADER_OBJ_DIR = $(BINDIR)/$(SHADER_DIR)

SOURCES := $(wildcard $(SRCDIR)/*.c $(SRCDIR)/**/*.c $(SRCDIR)/**/**/*.c       \
	$(SRCDIR)/**/**/**/*.c $(SRCDIR)/**/**/**/**/*.c)

SHADER_SOURCES := $(wildcard $(SHADER_SRC_DIR)/**/*.vert                    \
	$(SHADER_SRC_DIR)/**/*.frag)

INCLUDE_DIRS =
LIB_DIRS     =

OBJECTS         := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SHADER_OBJECTS  := $(SHADER_SOURCES:$(SHADER_SRC_DIR)/%=$(SHADER_OBJ_DIR)/%.svm)

rm = rm -rf

default: $(BINDIR)/$(TARGET)
all: default

$(BINDIR)/$(TARGET): $(OBJECTS) $(SHADER_OBJECTS)
	@mkdir -p $(BINDIR)
	@$(LINKER) $@ $(LIB_DIRS) $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE_DIRS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(SHADER_OBJECTS): $(SHADER_OBJ_DIR)/%.svm : $(SHADER_SRC_DIR)/%
	@mkdir -p $(dir $@)
	@$(GLSL_CC) $(GLSL_FLAGS) $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONE: run
run: $(BINDIR)/$(TARGET)
	-./$(BINDIR)/$(TARGET)


.PHONEY: clean
clean:
	@$(rm) $(OBJDIR)
	@$(rm) $(SHADER_OBJ_DIR)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

.PHONEY: valgrind
valgrind: $(BINDIR)/$(TARGET)
	@valgrind --leak-check=yes --track-origins=yes $(BINDIR)/$(TARGET)
