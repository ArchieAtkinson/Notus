[no-cd, no-exit-message]
@ run:
    just build
    build/zephyr/zephyr.exe

[no-cd, no-exit-message]
build:
    #!/usr/bin/env sh
    if [ -z `just find_nearest_prj` ]; then
        echo "Couldn't find prj.conf in current or parent directory(s)"
        exit 1
    fi
    west build -b native_sim -p auto -s `just find_nearest_prj`

[no-cd, no-exit-message]
@ clean:
    echo "Deleting build dir..."
    rm -R build

[no-exit-message]
@ twister test_dir='tests':
    twister --clobber-output --coverage -T {{test_dir}} --platform native_sim


[no-cd, private, no-exit-message]
find_nearest_prj:
    #!/usr/bin/env sh
    current_dir=$(pwd)
    while [ "$current_dir" != "/" ]; do
        if [ -f "$current_dir/prj.conf" ]; then
            echo "$current_dir"
            exit 0
        fi
        current_dir=$(dirname "$current_dir")
    echo ""
    done
