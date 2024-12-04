project('gfx2linux', 'c')

srcs = [
    'server/main.c',
    'server/parse.c'
]

# create html2c
foreach html : run_command('find','-type', 'f', '-iname', '*.html', check:true).stdout().strip().split('\n')
    h = custom_target('html2c',
        command: [
            '/bin/sh',
            meson.current_source_dir()/'tools/html2c.sh',
            meson.current_source_dir() / 'server/page.html',
            meson.build_root() /'page.c'
        ],
        output: 'page.c'
    )
    srcs += [h]
endforeach

# Create an executable
executable('main', srcs)
