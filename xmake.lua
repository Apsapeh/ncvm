includes("@builtin/check")

function set_mode_rules()
    --set_toolset("cc", "/opt/homebrew/opt/llvm/bin/clang")
    --set_toolset("c++", "/opt/homebrew/opt/llvm/bin/clang++")
    --set_toolset("ld", "/opt/homebrew/opt/llvm/bin/clang++")
    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
        add_cflags("-Wall")
        add_defines("_NCVM_DEBUG")
    elseif is_mode("release") then
        set_symbols("hidden")
        --set_fpmodels("fast")
        --set_optimize("aggressive")
        set_optimize("faster")
        set_strip("all")
    end
    --set_warnings("pedantic")
end


add_rules("mode.debug", "mode.release")

add_repositories("apsapeh-repo https://github.com/Apsapeh/xmake-repo.git")

add_requires("extc 9df07382effe0c59722bf67c7ccae7d2e42ca904")

option("big_endian")
option("default_loaders")
    add_defines("__NCVM_DEFAULT_LOADERS")
option_end()


target("ncvm-static")
    add_packages("extc")
    set_languages("c99")
    set_kind("static")
    add_includedirs("include")
    add_files("src/*.c")
    set_mode_rules()
    --set_policy("build.optimization.lto", true)


    add_options("default_loaders")
    if has_config("big_endian") then 
        add_defines("__NCVM_BIG_ENDIAN")
    end

    -- check_bigendian("NCVM_XMAKE_IS_BIG_ENDIAN")
    -- on_config(function(target)
    --     if target:check_sizeof("int") ~= "4" then
    --         raise("'int' size is not 4")
    --     end
    --     if target:check_sizeof("unsigned_long_long", {includes = "ncvm.h"}) ~= "8" then 
    --         raise("'unsigned_long_long' size is not 8")
    --     end
    -- end)

    

target("ncvm")
    add_packages("extc")
    set_languages("c99")
    add_cxxflags("-fPIC", {target = {"clang", "clang++", "gcc", "g++"}})
    add_defines("__NCVM_DYN_LIB_EXPORT")
    set_kind("shared")
    add_includedirs("include")
    add_files("src/*.c")
    set_mode_rules()

    add_options("default_loaders")
    if has_config("big_endian") then 
        add_defines("__NCVM_BIG_ENDIAN")
    end

    -- check_bigendian("NCVM_XMAKE_IS_BIG_ENDIAN")
    -- on_config(function(target)
    --     if target:check_sizeof("int") ~= "4" then
    --         raise("'int' size is not 4")
    --     end
    --     if target:check_sizeof("unsigned_long_long", {includes = "ncvm.h"}) ~= "8" then 
    --         raise("'unsigned_long_long' size is not 8")
    --     end
    -- end)


target("ncvm-cpp-example")
    add_packages("extc")
    set_kind("binary")
    set_languages("c++11")
    add_deps("ncvm-static")
    add_includedirs("include")
    add_files("examples/main.cpp")
    set_policy("build.optimization.lto", true)
    --add_ldflags("-flto=thin")
    set_rundir("$(projectdir)")
    set_mode_rules()


-- Libraries
target("lib1")
    add_packages("extc")
    set_languages("c99")
    add_cxxflags("-fPIC", {target = {"clang", "clang++", "gcc", "g++"}})
    set_kind("shared")
    add_includedirs("include")
    add_files("examples/libs/lib1.c")
    set_mode_rules()



-- tests_dir = "tests"
-- for _, file in ipairs(os.files(tests_dir .. "/**.c")) do
--     local name = path.basename(file)
    
--     target(file:sub(tests_dir:len()+2, -5))
--         set_kind("binary")
--         set_languages("c99")
--         set_default(true)
        
--         add_files(file)
--         add_tests("")
-- end