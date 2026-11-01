target("jcc_codegen_llvm")
    set_kind("static")
    add_rules("project_flags")
    add_files("*.cpp")
    add_includedirs("$(projectdir)/src", "$(projectdir)/libs", {public = true})
    add_deps("jcc_core")

    -- LLVM dependency isolated to this module
    on_load(function (target)
        import("lib.detect.find_tool")
        local llvm_config = find_tool("llvm-config")
        if not llvm_config then
            raise("llvm-config not found in PATH")
        end

        local cflags = os.iorun(llvm_config.program .. " --cflags"):trim()
        local ldflags = os.iorun(llvm_config.program .. " --ldflags --libs --system-libs"):trim()

        for _, flag in ipairs(os.argv(cflags)) do
            if flag:startswith("-I") then
                target:add("includedirs", flag:sub(3), {public = true})
            elseif flag:startswith("-D") then
                target:add("defines", flag:sub(3), {public = true})
            end
        end

        for _, flag in ipairs(os.argv(ldflags)) do
            if flag:startswith("-L") then
                target:add("linkdirs", flag:sub(3), {public = true})
            elseif flag:startswith("-l") then
                target:add("links", flag:sub(3), {public = true})
            else
                target:add("ldflags", flag, {public = true})
            end
        end
    end)
