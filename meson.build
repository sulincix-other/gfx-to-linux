project('gfx2linux', 'c')

srcs = [
    'src/webservice.c',
    'src/websocket.c',
    'src/uinput.c',
    'src/parse.c',
    'src/main.c'
]

deps = [
    dependency('libwebsockets')
]

# create html2c
h = custom_target('html2c',
    command: [
        '/bin/bash',
        meson.current_source_dir()/'tools/html2c.sh',
        meson.current_source_dir() / 'src/page.html',
        meson.build_root() /'page.c'
     ],
    output: 'page.c'
)
srcs += [h]

# Create an executable
executable('main', srcs, dependencies: deps)
