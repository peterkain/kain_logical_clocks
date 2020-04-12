project('lamport_clock', 'cpp', default_options : ['cpp_std=c++17', 'warning_level=3', 'werror=true'])

src = ['src/main.cpp']
inc_dirs = include_directories(['include', get_option('asio_include_dir'),
    get_option('cli11_include_dir'), get_option('spdlog_include_dir')])

thread_dep = dependency('threads')

executable('process', sources : src, include_directories : inc_dirs, dependencies : thread_dep)