add_rules("mode.debug", "mode.release")

set_languages("c++23")

-- Third-party libraries
target("fmt")
    set_kind("static")
    add_files("libs/fmt/src/format.cc", "libs/fmt/src/os.cc")
    add_includedirs("libs/fmt/include", {public = true})

target("mjolnir")
    set_kind("static")
    add_files(".deps/mjolnir/src/*.cpp")
    add_includedirs(".deps/mjolnir/include", {public = true})

-- Compiler flags and warnings for our project
rule("project_flags")
    on_load(function (target)
        if is_plat("windows") then
            target:add("warnings", "all", "error")
            target:add("cxflags", "/EHsc", "/WX", "/diagnostics:caret", "/W4", "/utf-8")
        else
            target:add("warnings", "all", "extra", "pedantic", "error")
            target:add("cxflags", "-Wno-error=switch", "-Wno-error=cpp", "--pedantic-errors")
            if is_mode("debug") then
                target:add("undefines", "_FORTIFY_SOURCE")
            end
        end
    end)

-- Source modules
includes("src")

-- Main driver executable
target("jcc")
    set_kind("binary")
    add_rules("project_flags")
    add_files("src/main.cpp")
    add_includedirs("src", "libs")
    add_deps("jcc_codegen_llvm", "jcc_core", "fmt")
