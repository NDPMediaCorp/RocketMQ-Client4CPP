
ifeq ($(arch),32)
	ARCH := -m32
	BITS := 32	
else
	ARCH:= -m64
	BITS:=64
	LIBEXT=64
endif

ifeq ($(release),y)
	OUTTYPE := -o2
	OUTCONFIG := release
else
	OUTCONFIG := debug
endif

TOPDIR:=..
OUTDIR=$(TOPDIR)/bin
OBJDIR=$(OUTDIR)/$(BITS)/$(OUTCONFIG)
SRCDIR:=$(TOPDIR)/src
VPATH:=$(SRCDIR)/lib_json
DIRS:=$(SRCDIR)/lib_json
SOURCE := $(foreach dir,$(DIRS), $(wildcard $(dir)/*.cpp))
OBJS := $(patsubst %.cpp,%.o, $(notdir $(SOURCE)))

OUT :=libjsoncpp$(LIBEXT).a
LIBS := 
LIBPATH := 
INCLUDEPATH :=  -I$(TOPDIR)/include $(addprefix -I ,$(DIRS))

CC := g++
AR := ar
CFLAGS := -g -fPIC -Wno-deprecated -O2 -mtune=generic $(ARCH) $(OUTTYPE)
LDFLAGS := -fPIC
CXXFLAGS := $(CFLAGS) $(INCLUDEPATH)

.PHONY : build rebuild clean echoo createdir cpfile

all:createdir build cpfile
createdir:
	mkdir -p $(OBJDIR)
cpfile:
	mkdir -p ../../jsoncpp/lib
	cp $(OUTDIR)/$(OUT) ../../jsoncpp/lib/

build:$(OUT)
rebuild:clean build
clean:
	$(RM) $(OBJDIR)/$(OBJS) $(OUTDIR)/$(OUT)
echoo:
#	echo $(SOURCE)
	@echo $(OBJS)

$(OUT) : $(OBJS)
	$(AR) rcs $(OUTDIR)/$(OUT) $(addprefix $(OBJDIR)/,$(OBJS)) $(LIBDIR) $(addprefix -l ,$(LIBS))

%.o : %.cpp
	$(CC) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $(OBJDIR)/$@

