target("jcc_core")
    set_kind("static")
    add_rules("project_flags")
    add_files(
        "misc/*.cpp",
        "preprocessor/*.cpp",
        "preprocessor/commands/*.cpp",
        "tokenizer/*.cpp",
        "tokenizer/tokens/*.cpp",
        "parsing/*.cpp",
        "parsing/types/*.cpp",
        "visitors/*.cpp",
        "diagnostics/*.cpp",
        "diagnostics/variants/*.cpp",
        "diagnostics/variants/tk_pp/*.cpp",
        "diagnostics/variants/parsing/*.cpp",
        "diagnostics/variants/sema/*.cpp",
        "diagnostics/variants/visitors/*.cpp"
    )
    add_includedirs("$(projectdir)/src", "$(projectdir)/libs", {public = true})
    add_deps("fmt", "mjolnir")

-- Include submodules
includes("codegen_llvm")
