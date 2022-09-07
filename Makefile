# **************************************************************************** #
#   VARIABLES                                                                  #
# **************************************************************************** #

NAME        := webserv

CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98

CPPFLAGS    :=
DEPFLAGS     = -MT $@ -MMD -MP -MF $(DDIR)/$*.d

LDFLAGS     :=
LDLIBS      :=

VPATH       := src/
SRCS        := webserv.cpp

BUILDDIR    := build

ODIR        := $(BUILDDIR)/obj
OBJS        := $(SRCS:%.cpp=$(ODIR)/%.o)

DDIR        := $(BUILDDIR)/deps
DEPS        := $(SRCS:%.cpp=$(DDIR)/%.d)

# **************************************************************************** #
#   RULES                                                                      #
# **************************************************************************** #

.PHONY: all clean fclean re

$(BUILDDIR)/$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(ODIR)/%.o: %.cpp $(DDIR)/%.d | $(ODIR) $(DDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

$(ODIR):
	mkdir -p $@

$(DDIR):
	mkdir -p $@

all: $(BUILDDIR)/$(NAME)

clean:
	$(RM) -r $(DDIR) $(ODIR)

fclean: clean
	$(RM) -r $(BUILDDIR)

re: fclean all

run: $(BUILDDIR)/$(NAME)
	@$(BUILDDIR)/$(NAME) $(ARGS)

$(DEPS):
-include $(DEPS)
