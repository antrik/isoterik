# CFLAGS useful for debugging.
#
# Can keep these in production versions,
# as both size and especially speed differences
# are quite irrelevant for such a simple tool.
CFLAGS=-Wall -Og -g3

LDFLAGS=-ldvdread

all: udffindfile
