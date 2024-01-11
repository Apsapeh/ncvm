-- package("extc")
--     set_urls("https://github.com/Apsapeh/extc.git")
--     on_install(function (package)
--         import("package.tools.xmake").install(package)
--     end)

--set_project("ncvm")


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

add_requires("extc")


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


target("ncvm-cpp-example")
    set_kind("binary")
    add_deps("ncvm-static")
    add_includedirs("include")
    add_files("examples/main.cpp")
    set_mode_rules()


--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project? --
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

