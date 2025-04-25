zephyr_library()
zephyr_library_sources(ezo_ec.c)
zephyr_library_sources_ifdef(CONFIG_EZO_EC_EMUL emul_ezo_ec.c)
message("Hello3")
