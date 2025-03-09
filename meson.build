project('gfx2linux', 'c')

srcs = [
    'src/webservice.c',
    'src/websocket.c',
    'src/uinput.c',
    'src/parse.c',
    'src/notify.c',
    'src/main.c'
]

agent_srcs = [
    'src/agent.c'
]
deps = [
    dependency('libwebsockets'),
    dependency('libevdev')
]

agent_deps = [
    dependency('dbus-1')
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
executable('main', srcs, dependencies: deps, install: true)
executable('agent', agent_srcs, dependencies: agent_deps, install: true)
