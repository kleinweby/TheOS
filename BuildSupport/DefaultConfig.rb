PLATFORM='x86'
ARCH='i386'

TOOLCHAIN="#{File.dirname(File.dirname(__FILE__))}/Toolchain/#{PLATFORM.downcase}.toolchain"

ROOT="#{File.dirname(File.dirname(__FILE__))}/root"

# -F should be relative to #{ROOT} ?!?
CFLAGS  = [ "-isysroot #{ROOT}", "-F#{ROOT}/System/Frameworks"]
LDFLAGS = []
DEFINES = []

ENV['PATH'] = "#{TOOLCHAIN}/bin:#{ENV['PATH']}"

CC      = "clang"
LD      = "ld"
AR      = "ar"
OBJCOPY = "objcopy"
STRIP   = "strip"
GDB     = "#{TOOLCHAIN}/bin/#{PLATFORM}-theos-elf-gdb"
NASM    = 'nasm'

OBJ_DIR = '.objs'

DEFINES << '-D__PLATFORM_'+ARCH.upcase+'__' << "-D__PLATFORM__="+PLATFORM.upcase
CFLAGS << '-ggdb' << '-fno-builtin' << '-Werror' << '-Weverything'
