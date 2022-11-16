# **************************************************************************** #
#   VARIABLES                                                                  #
# **************************************************************************** #

NAME        :=	webserv

CXX         :=	c++
CXXFLAGS    :=	-Wall -Wextra -Werror -std=c++98 -g -fsanitize=address -fsanitize=undefined

CPPFLAGS    :=
DEPFLAGS     =	-MT $@ -MMD -MP -MF $(DDIR)/$*.d

LDFLAGS     :=
LDLIBS      :=

VPATH       :=	src/		\
				src/config/	\
				src/core/	\
				src/http/	\
				src/utils/

SRCS		:=	$(notdir $(foreach dir, $(VPATH), $(wildcard $(dir)/*.cpp)))

BUILDDIR    :=	build

ODIR        :=	$(BUILDDIR)/obj
OBJS        :=	$(SRCS:%.cpp=$(ODIR)/%.o)

DDIR        :=	$(BUILDDIR)/deps
DEPS        :=	$(SRCS:%.cpp=$(DDIR)/%.d)

# **************************************************************************** #
#   RULES                                                                      #
# **************************************************************************** #

.PHONY: all clean fclean re

all: $(BUILDDIR)/$(NAME)

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
