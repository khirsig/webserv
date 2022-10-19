# **************************************************************************** #
#   VARIABLES                                                                  #
# **************************************************************************** #

NAME        :=	webserv

CXX         :=	c++
CXXFLAGS    :=	-Wall -Wextra -std=c++98 -g -fsanitize=address -fsanitize=undefined #-Werror

CPPFLAGS    :=
DEPFLAGS     =	-MT $@ -MMD -MP -MF $(DDIR)/$*.d

LDFLAGS     :=
LDLIBS      :=

VPATH       :=	src/		\
				src/config/	\
				src/core/	\
				src/file/	\
				src/http/

SRCS		:=	$(notdir $(foreach dir, $(VPATH), $(wildcard $(dir)/*.cpp)))

BUILDDIR    :=	build

ODIR        :=	$(BUILDDIR)/obj
OBJS        :=	$(SRCS:%.cpp=$(ODIR)/%.o)

DDIR        :=	$(BUILDDIR)/deps
DEPS        :=	$(SRCS:%.cpp=$(DDIR)/%.d)

# **************************************************************************** #
#	SYSTEM SPECIFIC SETTINGS							   					   #
# **************************************************************************** #

UNAME	:= $(shell uname -s)
NUMPROC	:= 8

ifeq ($(UNAME), Linux)
    NUMPROC  := $(shell grep -c ^processor /proc/cpuinfo)
	CPPFLAGS += -D Linux
	CXXFLAGS += -Wno-unused-result
else ifeq ($(UNAME), Darwin)
    NUMPROC  := $(shell sysctl -n hw.ncpu)
	CPPFLAGS += -D Darwin
endif

# **************************************************************************** #
#   RULES                                                                      #
# **************************************************************************** #

.PHONY: all clean fclean re

all:
	@$(MAKE) $(BUILDDIR)/$(NAME) -j$(NUMPROC)

clean:
	$(RM) -r $(DDIR) $(ODIR)

fclean: clean
	$(RM) -r $(BUILDDIR)

re: fclean all

run: $(BUILDDIR)/$(NAME)
	@$(BUILDDIR)/$(NAME) $(ARGS)

$(BUILDDIR)/$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(ODIR)/%.o: %.cpp $(DDIR)/%.d | $(ODIR) $(DDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

$(ODIR):
	mkdir -p $@

$(DDIR):
	mkdir -p $@

$(DEPS):

-include $(DEPS)
