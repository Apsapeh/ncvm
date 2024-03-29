function set_mode_rules()
    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        set_symbols("hidden")
        --set_fpmodels("fast")
        set_optimize("fastest")
        set_strip("all")
    end
end


add_rules("mode.debug", "mode.release")

add_repositories("apsapeh-repo https://github.com/Apsapeh/xmake-repo.git")

add_requires("extc 18da0333c5a02ffe99eaf043c227290006383a14")


target("ncvm-static")
    add_packages("extc")
    set_languages("c89")
    set_kind("static")
    add_includedirs("include")
    add_files("src/ncvm.c")
    set_mode_rules()

target("ncvm")
    add_packages("extc")
    set_languages("c89")
    add_cxxflags("-fPIC", {target = {"clang", "clang++", "gcc", "g++"}})
    add_defines("__NCVM_DYN_LIB_EXPORT")
    set_kind("shared")
    add_includedirs("include")
    add_files("src/ncvm.c")
    set_mode_rules()


target("ncvm-bin")
    add_packages("extc")
    set_kind("binary")
    set_languages("c89")
    add_deps("ncvm-static")
    add_includedirs("include")
    add_files("bin/ncvm/*.c")
    set_basename("ncvm")
    set_rundir("$(projectdir)")
    set_mode_rules()

target("ncvm-asm-bin")
    add_packages("extc")

    set_kind("binary")
    set_languages("c89")
    add_deps("ncvm-static")
    add_includedirs("include")
    add_files("bin/ncvm-asm/*.c")
    set_basename("ncvm-asm")
    set_rundir("$(projectdir)")
    set_mode_rules()

target("ncvm-cpp-example")
    add_packages("extc")
    set_kind("binary")
    set_languages("c++11")
    add_deps("ncvm-static")
    add_includedirs("include")
    add_files("examples/main.cpp")
    set_rundir("$(projectdir)")
    set_mode_rules()
