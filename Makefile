VERSION_CURRENT = 0
VERSION_REVISION = 0
VERSION_AGE = 0
NAME = wshook
LINKERNAME = lib$(NAME).so
SONAME = $(LINKERNAME).$(VERSION_CURRENT)
REALNAME = $(LINKERNAME).$(VERSION_CURRENT).$(VERSION_REVISION).$(VERSION_AGE)

D_OBJS = objs
D_SRCS = srcs
D_INCS = incs

ifeq ($(PREFIX),)
	PREFIX = /usr/local
endif

LIBDIR = $(PREFIX)/lib
INCDIR = $(PREFIX)/include
INSTALL = install -p
INSTALL_PROGRAM = $(INSTALL) -m644
INSTALL_INCLUDE = $(INSTALL) -m644
INSTALL_DIR = $(INSTALL) -d

SRCS = $(wildcard $(D_SRCS)/*.cpp)
OBJS = $(patsubst $(D_SRCS)/%.cpp,$(D_OBJS)/%.o,$(SRCS))
DEPS = $(patsubst $(D_SRCS)/%.cpp,$(D_OBJS)/%.d,$(SRCS))
INCS = $(wildcard $(D_INCS)/*.hpp)

CXXFLAGS = -std=c++1z -fPIC
CXXFLAGS += -Wall -Wextra -Wpedantic
CXXFLAGS += -I $(D_INCS)

LDFLAGS = -ljsoncpp -lboost_system -lpthread -shared -Wl,-soname,$(SONAME)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(REALNAME) $(OBJS)
	@echo -e "\033[38;5;10m=== $(REALNAME): Compiled !\033[0m"

$(D_OBJS)/%.o: $(D_SRCS)/%.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

include $(DEPS)

$(DEPS): ;

clean:
	find -name "*~" -delete -o -name "#*#" -delete
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) -d $(D_OBJS)

fclean: clean
	$(RM) $(REALNAME)

re: fclean all

debug: CXXFLAGS += -g3 -O0 -D DEBUG
debug: re
	@echo -e "\033[1;49;36m=== $(REALNAME): Debug mode\033[0m"

RELEASEFLAGS = -Ofast
RELEASEFLAGS += -fvisibility=hidden
RELEASEFLAGS += -D_FORTIFY_SOURCE=2 -fstack-protector-all -z noexecstack -z relro -z now -Wl,-z,defs #-Wformat -Wformat-security

release: CXXFLAGS += $(RELEASEFLAGS)
release: re
	strip --strip-all $(REALNAME)
	@echo -e "\033[1;49;36m=== $(REALNAME): Release mode\033[0m"

install:
	# install .so
	$(INSTALL_DIR) $(DESTDIR)$(LIBDIR)
	$(INSTALL_PROGRAM) $(REALNAME) $(DESTDIR)$(LIBDIR)/$(REALNAME)
	cd $(DESTDIR)$(LIBDIR) ; ln -fs $(REALNAME) $(LINKERNAME)
	cd $(DESTDIR)$(LIBDIR) ; ln -fs $(REALNAME) $(SONAME)
	# install includes
	$(INSTALL_DIR) $(DESTDIR)$(INCDIR)/$(NAME)
	$(INSTALL_INCLUDE) $(INCS) $(DESTDIR)$(INCDIR)/$(NAME)

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(REALNAME)
	rm -f $(DESTDIR)$(LIBDIR)/$(LINKERNAME)
	rm -f $(DESTDIR)$(LIBDIR)/$(SONAME)
	rm -rf $(DESTDIR)$(INCDIR)/$(NAME)

tidy:
	clang-tidy -checks='cert-*,cppcoreguidelines-*,google-*,llvm-*,misc-*,modernize-*,performance-*,readility-*,-cppcoreguidelines-pro-type-reinterpret-cast,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-google-readability-namespace-comments,-llvm-namespace-comment,-llvm-include-order,-google-runtime-references'  srcs/*.cpp incs/*.hpp -- -std=c++1z -Iincs

.PHONY: all clean fclean re debug release install uninstall tidy
