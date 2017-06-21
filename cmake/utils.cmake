cmake_policy(SET CMP0054 NEW)
set(_CopyDepScriptFile "${CMAKE_CURRENT_LIST_DIR}/copy_deps.ps1")

macro(OrganizeSources)
    foreach (_srcFile ${ARGN})
        get_filename_component(folder ${_srcFile} DIRECTORY)

        if ("${folder}_" STREQUAL "_")
            set(_NoSrcGroupFiles ${_NoSrcGroupFiles} ${_srcFile})
            continue()
        endif ("${folder}_" STREQUAL "_")

        string(REPLACE "/" "\\" _srcGroupName ${folder})

        set(_srcGroupFilesList "${_srcGroupName}_files")
        set(${_srcGroupName}_FILES ${${_srcGroupName}_FILES} ${_srcFile})
        list(LENGTH ${_srcGroupName}_FILES _currentGroupSize)
        if (${_currentGroupSize} EQUAL 1)
            set(_sourceGroups ${_sourceGroups} ${_srcGroupName})
        endif ()
    endforeach ()
    foreach (_srcGroup ${_sourceGroups})
        if (${CMAKE_VERBOSE_MAKEFILE})
            message("adding source group ${_srcGroup}")
            message("files in group:${${_srcGroup}_FILES}")
        endif (${CMAKE_VERBOSE_MAKEFILE})
        source_group(${_srcGroup} FILES ${${_srcGroup}_FILES})
    endforeach ()
    source_group("" FILES ${_NoSrcGroupFiles})
endmacro(OrganizeSources)

macro(CheckDependency DepName)
    if (NOT ${DepName}_FOUND)
        message(FATAL_ERROR "Dependency ${DepName} not found by path ${DepName}_DIR=${${DepName}_DIR}")
    endif ()
endmacro(CheckDependency)

macro(_ParseCopyDepsArguments)
    set(_NextVariable "")
    foreach (arg in ${ARGN})
        if ("${arg}" STREQUAL "TARGET")
            set(_NextVariable "TARGET")
        elseif ("${arg}" STREQUAL "LIBRARIES")
            set(_NextVariable "LIBRARIES")
        else ()
            set(${_NextVariable} ${${_NextVariable}} ${arg})
        endif ()
    endforeach ()
endmacro()

# usage CopyDependencies(TARGET ${targets...} LIBRARIES ${libraries ...})
function(CopyDependencies)
    _ParseCopyDepsArguments(${ARGN})
    add_custom_command(TARGET ${TARGET} POST_BUILD
                       COMMAND powershell.exe
                       -file \"${_CopyDepScriptFile}\"
                       -PInstallDir \"$<TARGET_FILE_DIR:${TARGET}>\"
                       -PBuildType $<CONFIG>
                       -PLibraryDependencies "${LIBRARIES}"
                       -PVerbose=$True)
endfunction()
