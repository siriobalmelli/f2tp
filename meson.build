project('f2tp', 'c',
  version : '0.0.1',
  license : 'LGPL-2.1+',
)
description = 'Fast FEC Transport Protocol (library)'
homepage = 'https://github.com/siriobalmelli/f2tp'


# dependencies
deps = [
  dependency('libsodium'),
  dependency('nonlibc')
]


# headers
includes = include_directories(
  'include'
)
install_headers(
  'include/f2tp.h'
)


# libf2tp
sources = files(
  'lib/f2tp.c'
)
libf2tp = library('f2tp',
  sources,
  include_directories: includes,
  dependencies: deps
)
libf2tp_dep = declare_dependency(
  include_directories: includes,
  dependencies: deps,
  link_with: libf2tp
)


# tests
tests = [
  'test_hash.c'
]
foreach t : tests
  name = t.split('.')[0]
  a_test = executable(name, 'tests' / t, dependencies: libf2tp_dep)

  name_format = name.split('_')[0]
  test(name_format, a_test)
endforeach
