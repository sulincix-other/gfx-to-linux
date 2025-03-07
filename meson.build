project('gfx2linux', 'c')

srcs = [
    'src/webservice.c',
    'src/websocket.c',
    'src/uinput.c',
    'src/parse.c',
    'src/main.c'
]

deps = [
    dependency('libwebsockets'),
    dependency('libevdev')
]

# create html2c
foreach data : run_command('ls', 'data/', check:true).stdout().strip().split('\n')
    h = custom_target(data,
        command: [
            '/bin/bash',
            meson.current_source_dir()/'tools/html2c.sh',
            meson.current_source_dir() / 'data' / data,
            meson.build_root() / data + '.c'
         ],
        output: data + '.c'
    )
    srcs += [h]
endforeach

# Create an executable
executable('main', srcs, dependencies: deps)
