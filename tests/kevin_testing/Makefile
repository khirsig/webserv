# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/07 09:56:24 by tjensen           #+#    #+#              #
#    Updated: 2022/09/15 10:53:22 by khirsig          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := cpp_parser

CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++11

CPPFLAGS    :=
DEPFLAGS     = -MT $@ -MMD -MP -MF $(DDIR)/$*.d

LDFLAGS     :=
LDLIBS      :=

VPATH       := src/
SRCS        := main.cpp 	\
			Tokenizer.cpp	\
			Token.cpp		\
			Parser.cpp		\
			Server.cpp		\
			Location.cpp	\
			Interpreter.cpp \

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
