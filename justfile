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
@ test test_dir='.':
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

[no-exit-message]
list_image_tags registry image_name access_token:
    #!/usr/bin/env sh
    RESPONSE=$(curl -s -H "Authorization: Bearer $(echo {{access_token}} | base64)" https://{{registry}}/v2/{{image_name}}/tags/list)
    if [ -z "$RESPONSE" ]; then
        echo "Failed to retrieve tags from {{registry}}/{{image_name}}"
        exit 1
    fi
    echo $RESPONSE | jq -r '.tags | join(" ")'

[no-exit-message]
install_module module_subdir +cmake_options="":
    #!/usr/bin/env sh
    cd $WEST_WORKSPACE/{{module_subdir}}
    mkdir -p build
    cmake {{cmake_options}} . -B build
    cd build
    sudo make install

[no-exit-message]
install_zephyr:
    west init -l .
    west update

[no-exit-message]
install_all:
    install_zephyr
    install_module magic_enum -DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF -DMAGIC_ENUM_OPT_BUILD_TESTS=OFF
    install_module expected -DEXPECTED_BUILD_TESTS=OFF -DMAGIC_ENUM_OPT_BUILD_TESTS=OFF
    install_module optional -DOPTIONAL_BUILD_TESTS=OFF
    install_module etl