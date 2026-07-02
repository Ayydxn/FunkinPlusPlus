-- A system for registering and including dependencies.

-----------
-- USAGE --
-----------

-- 1. Define your dependencies towards the bottom of this file.
--
-- 2. In any project block, call Include or IncludeDependencies to add whichever dependencies the project needs. Example:
--  Include("GLFW")
--  IncludeDependencies({ "GLFW", "spdlog", "ImGui" })

-------------------
-- REGISTERATION --
-------------------

-- TOP-LEVEL fields (applied on all platforms):
--   IncludePaths (table)         - Include paths shared across all platforms
--   LibraryPaths (table)         - Library search paths shared across all platforms
--   Links        (table)         - Libraries to link shared across all platforms
--   Defines      (table)         - Preprocessor defines shared across all platforms
--   Files        (table)         - Source files added to the project (e.g. header-only libs)
--   DependsOn    (table)         - Other registered dependency names this one requires
--
-- PLATFORM-SPECIFIC fields (optional, keyed by Premake system name):
--   platforms        (table)
--     [platform]     (table)    - Platform name: "Windows", "MacOSX", "Linux"
--       IncludePaths (table)    - Platform-specific include paths
--       LibraryPaths (table)    - Platform-specific library search paths
--       Links        (table)    - Platform-specific libraries to link
--       Defines      (table)    - Platform-specific preprocessor defines
--       Files        (table)    - Platform-specific source files

-----------------
-- FULL SCHEMA --
-----------------

--   Register("Name", {
--       -- [1] Always applied
--       IncludePaths     = { ... },
--       LibraryPaths     = { ... },
--       Links            = { ... },
--       Defines          = { ... },
--       Files            = { ... },
--       DependesOn       = { "OtherDep" },
--
--       -- [2] Cross-platform, per-configuration
--       Configurations = {
--           Debug           = { links = { "MyLib_d" }, defines = { "MY_DEBUG" } },
--           Release         = { links = { "MyLib"   } },
--           Distribution    = { links = { "MyLib"   }, defines = { "MY_DIST"  } },
--       },
--
--       -- [3] Per-platform (all configurations)
--       Platforms = {
--           Windows = {
--               Links = { "user32" },
--
--               -- [4] Per-platform AND per-configuration
--               Configurations = {
--                   Debug           = { libdirs = { "vendor/lib/windows/debug"   }, links = { "MyLib_d" } },
--                   Release         = { libdirs = { "vendor/lib/windows/release" }, links = { "MyLib"   } },
--                   Distribution    = { libdirs = { "vendor/lib/windows/release" }, links = { "MyLib"   } },
--               },
--           },
--           Linux = {
--               Configurations = {
--                   Debug           = { libdirs = { "vendor/lib/linux/debug"   }, links = { "MyLib_d" } },
--                   Release         = { libdirs = { "vendor/lib/linux/release" }, links = { "MyLib"   } },
--                   Distribution    = { libdirs = { "vendor/lib/linux/release" }, links = { "MyLib"   } },
--               },
--           },
--       },
--   })

DependencyRegistry = {}

local function NormaliseSettings(settings, context)
    assert(type(settings) == "table", "Expected a table at " .. context)
    
    settings.IncludePaths = settings.IncludePaths or {}
    settings.LibraryPaths     = settings.LibraryPaths     or {}
    settings.Links       = settings.Links       or {}
    settings.Defines     = settings.Defines     or {}
    settings.Files       = settings.Files       or {}
    
    return settings
end

-- Normalise a configurations sub-table (keyed by config name).
local function NormaliseConfigurationsTable(table, context)
    table = table or {}
    
    for cfgName, cfgSettings in pairs(table) do
        NormaliseSettings(cfgSettings, context .. ".configurations." .. cfgName)
    end
    
    return table
end

function Register(name, config)
    assert(type(name) == "string" and #name > 0, "Dependency names must be a non-empty string!")
    assert(type(config) == "table", "The configuration for dependency '" .. name .. "' must be a table!")

    if DependencyRegistry[name] then
        warn("Overwriting already registered dependency '" .. name .. "'!")
    end
    
    NormaliseSettings(config, name)
    
    config.DependsOn = config.DependsOn or {}
    config.Configurations = NormaliseConfigurationsTable(config.Configurations, name)
    config.Platforms = config.Platforms or {}

    for platform, platformSettings in pairs(config.Platforms) do
        NormaliseSettings(platformSettings, name .. ".platforms." .. platform)
        
        platformSettings.Configurations = NormaliseConfigurationsTable(platformSettings.Configurations, name .. ".platforms." .. platform)
    end

    DependencyRegistry[name] = config
end

local function ApplyDependencySettings(settings)
    if #settings.IncludePaths > 0 then
        includedirs(settings.IncludePaths)
    end
    
    if #settings.LibraryPaths > 0 then
        libdirs(settings.LibraryPaths)
    end
    
    if #settings.Links > 0 then
        links(settings.Links)
    end
    
    if #settings.Defines > 0 then
        defines(settings.Defines)
    end
    
    if #settings.Files > 0 then
        files(settings.Files)
    end
end

function Include(name, visited)
    visited = visited or {}

    if visited[name] then
        return
    end

    visited[name] = true

    local dependencyConfig = DependencyRegistry[name]
    assert(dependencyConfig, "Unknown dependency '" .. name .. "'! Did you forget to register it?")

    for _, dependency in ipairs(dependencyConfig.DependsOn) do
        Include(dependency, visited)
    end
    
    ApplyDependencySettings(dependencyConfig)
    
    for platform, settings in pairs(dependencyConfig.Platforms) do
        filter("system:" .. string.lower(platform))
            ApplyDependencySettings(settings)

        for configName, configSettings in pairs(settings.Configurations) do
            filter { "system:" .. platform, "configurations:" .. configName}
                ApplyDependencySettings(configSettings)
        end
    end
    
    filter {}
end

function IncludeDependencies(names)
    assert(type(names) == "table", "IncludeDependencies expects a table of dependency names!")

    local visited = {}

    for _, name in ipairs(names) do
        Include(name, visited)
    end
end

function IncludeAllDependencies()
    local visited = {}

    for name, _ in pairs(DependencyRegistry) do
        Include(name, visited)
    end
end

function ListDependencies()
    print("Registered Dependencies:")
    for name, config in pairs(DependencyRegistry) do
        local platforms = {}
        for p, _ in pairs(config.Platforms) do table.insert(platforms, p) end

        local cfgs = {}
        for c, _ in pairs(config.Configurations) do table.insert(cfgs, c) end

        local parts = {}
        if #platforms > 0 then
            table.insert(parts, "Platforms: " .. table.concat(platforms, ", "))
        end
        
        if #cfgs > 0 then
            table.insert(parts, "Cross-Platform Configurations: " .. table.concat(cfgs, ", "))
        end
        
        if #parts == 0 then
            table.insert(parts, "Cross-Platform, Configuration-Agnostic")
        end

        print(string.format("  - %-20s [%s]", name, table.concat(parts, " | ")))
    end
end

-----------------------------
-- Dependency Registration --
-----------------------------

local ThirdPartyFolder = "%{wks.location}/Source/ThirdParty/"

Register("spdlog", {
    IncludePaths = { ThirdPartyFolder .. "spdlog/include" }
})

Register("SDL3", {
    IncludePaths = { ThirdPartyFolder .. "SDL3/Include" },
    
    Platforms = {
        Windows = {
            LibraryPaths = { ThirdPartyFolder .. "SDL3/Lib/Win64" },
            Links = { ThirdPartyFolder .. "SDL3/Lib/Win64/SDL3.lib" }
        }
    }
})

ListDependencies()
