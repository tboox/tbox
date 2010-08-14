/* This is a dummy file.
 * The actual version info is in toplevel cairo-version.h.
 * The purpose of this file is to make most of the source files NOT depend
 * on the real cairo-version.h, and as a result, changing library version
 * would not cause a complete rebuild of all object files (just a relink).
 * This is useful when bisecting. */
#ifndef CAIRO_VERSION_H
#define CAIRO_VERSION_H

#define CAIRO_VERSION_MAJOR 1
#define CAIRO_VERSION_MINOR 9
#define CAIRO_VERSION_MICRO 6

#endif
