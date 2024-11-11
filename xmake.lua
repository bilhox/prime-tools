set_xmakever("2.5.9")

set_project("GIMPS")
set_version("0.0.0")
set_toolchains("clang")

add_rules("mode.release")
set_languages("c17")
set_optimize("fastest")

target("unity")
    set_kind("static")

    add_files("unity/src/*.c")
    add_includedirs("unity/src/", {public = true})

target("GIMPS")
    set_kind("binary")
    -- set_symbols("debug")

    set_targetdir("build/")

    add_files("src_c/**.c")
    add_headerfiles("src_c/**.h")

target("BigInt_Test")
    set_kind("binary")
    set_symbols("debug")

    add_deps("unity")

    set_targetdir("build/")
    add_includedirs("src_c/", {public = true})

    add_headerfiles("src_c/**.h")
    add_headerfiles("tests/**.h")
    add_files("src_c/**.c")
    add_files("tests/**.c")
    remove_files("src_c/main.c")
