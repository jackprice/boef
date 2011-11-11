bin_PROGRAMS = boef
boef_SOURCES = main.cpp modules.cpp host.cpp workspace.cpp interface.cpp fuzz.cpp debug.cpp strings.cpp

boef_LDADD = $(GTK_LIBS) $(VTE_LIBS)
boef_CFLAGS = $(GTK_CFLAGS) $(VTE_CFLAGS)
boef_CXXFLAGS = $(GTK_CFLAGS) $(VTE_CFLAGS)

AM_CPPFLAGS = \
	$(VTE_CFLAGS) \
	$(GTK_CFLAGS)
