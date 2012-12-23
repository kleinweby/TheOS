PLATFORM='i386'

CFLAGS  = [ "-ccc-host-triple #{PLATFORM}-pc-linux-gnu" ]
LDFLAGS = []
DEFINES = []

TOOLCHAIN="#{File.dirname(__FILE__)}/toolchain"

ENV['PATH'] = "#{TOOLCHAIN}/bin:#{ENV['PATH']}"

CC      = ENV['CC'] || "clang"
LD      = "#{PLATFORM}-theos-elf-ld"
AR      = "#{PLATFORM}-theos-elf-ar"
OBJCOPY = "#{PLATFORM}-theos-elf-objcopy"
STRIP   = "#{PLATFORM}-theos-elf-strip"
GDB     = "#{TOOLCHAIN}/bin/#{PLATFORM}-theos-elf-gdb"
NASM    = 'nasm'

OBJ_DIR = '.objs'

DEFINES << '-D__PLATFORM_'+PLATFORM.upcase+'__'
CFLAGS << '-std=c1x' << '-ggdb' << '-fno-builtin' << '-Werror' << '-Weverything'
