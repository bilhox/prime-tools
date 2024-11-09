set_xmakever("2.5.9")

set_project("GIMPS")
set_version("0.0.0")
set_toolchains("clang")

set_allowedplats("windows", "linux", "macosx")
set_allowedarchs("windows|x64", "linux|x86_64", "macosx|x86_64")

add_rules("mode.release")
set_languages("c17")
set_optimize("fastest")

target("GIMPS")
    set_kind("binary")

    set_targetdir("build/")

    add_files("src_c/**.c")
    add_headerfiles("src_c/**.h")
