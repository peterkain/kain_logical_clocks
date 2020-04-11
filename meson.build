project('lamport_clock', 'cpp', default_options : ['cpp_std=c++17', 'warning_level=3', 'werror=true'])

src = ['src/main.cpp', 'src/lamport_clock.cpp']
inc_dirs = include_directories(['extern/asio/include', 'include', 'extern/include'])

executable('process', sources : src, include_directories : inc_dirs)