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
foreach data : run_command('ls', 'src/client/', check:true).stdout().strip().split('\n')
    h = custom_target(data,
        command: [
            '/bin/bash',
            meson.current_source_dir()/'tools/html2c.sh',
            meson.current_source_dir() / 'src/client' / data,
            meson.project_build_root() / data + '.c'
         ],
        output: data + '.c'
    )
    srcs += [h]
endforeach

libexecdir = '/usr/libexec'
desktopdir = '/etc/xdg/autostart'

add_project_arguments('-fvisibility=hidden', language: 'c')

# Create executables
executable('gfx2linux-server', srcs, dependencies: deps, install: true, install_dir: libexecdir)
executable('gfx2linux-agent', agent_srcs, dependencies: agent_deps, install: true, install_dir: libexecdir)

# Data files
install_data('data/gfx2linux-agent.desktop', install_dir : desktopdir)

if get_option('service') == 'sysvinit'
    install_data('data/gfx2linux.sysvinit', install_dir : '/etc/init.d', rename:'gfx2linux')
elif get_option('service') == 'openrc'
    install_data('data/gfx2linux.openrc', install_dir : '/etc/init.d', rename:'gfx2linux')
else
    install_data('data/gfx2linux.systemd', install_dir : '/etc/systemd/system/', rename: 'gfx2linux.service')
endif

