project(
  'factory-game', 'cpp',
  version : '0.1.0',
  default_options : ['cpp_std=c++20']
)

# input dependencies.
deps = [dependency('glm')]

inc = include_directories('include')

# build target.
exe = executable(
  'main',
  [
    'src/main.cc',
    'src/draw.cc',
    'src/foundation.cc',
    'src/machine.cc',
    'src/state.cc',
  ],
  include_directories: inc
)
