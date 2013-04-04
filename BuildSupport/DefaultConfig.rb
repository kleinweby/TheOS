PLATFORM='i386'

TOOLCHAIN="#{File.dirname(__FILE__)}/toolchain"

ROOT="#{File.dirname(File.dirname(__FILE__))}/root"

# -F should be relative to #{ROOT} ?!?
CFLAGS  = [ "-isysroot #{ROOT}", "-F#{ROOT}/System/Frameworks"]
LDFLAGS = []
DEFINES = []

ENV['PATH'] = "#{TOOLCHAIN}/bin:#{ENV['PATH']}"

CC      = ENV['CC'] || "#{PLATFORM}-unkown-theos-clang"
LD      = "#{PLATFORM}-theos-elf-ld"
AR      = "#{PLATFORM}-theos-elf-ar"
OBJCOPY = "#{PLATFORM}-theos-elf-objcopy"
STRIP   = "#{PLATFORM}-theos-elf-strip"
GDB     = "#{TOOLCHAIN}/bin/#{PLATFORM}-theos-elf-gdb"
NASM    = 'nasm'

OBJ_DIR = '.objs'

DEFINES << '-D__PLATFORM_'+PLATFORM.upcase+'__' << "-D__PLATFORM__=X86"
CFLAGS << '-ggdb' << '-fno-builtin' << '-Werror' << '-Weverything'
