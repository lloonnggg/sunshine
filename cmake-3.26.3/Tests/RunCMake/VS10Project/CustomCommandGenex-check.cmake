set(vcProjectFile "${RunCMake_TEST_BINARY_DIR}/foo.vcxproj")
if(NOT EXISTS "${vcProjectFile}")
  set(RunCMake_TEST_FAILED "Project file ${vcProjectFile} does not exist.")
  return()
endif()

set(found_CustomBuild_out 0)
set(found_CustomBuild_out_CONFIG 0)
set(found_CustomBuild_out_CONFIG_CONFIG 0)
set(found_CustomBuild_out_HASH 0)
file(STRINGS "${vcProjectFile}" lines)
foreach(line IN LISTS lines)
  if(line MATCHES [[<CustomBuild Include=".*\\out\.txt\.rule">]])
    set(found_CustomBuild_out 1)
  endif()
  if(line MATCHES [[<CustomBuild Include=".*\\out-\(CONFIG\)\.txt\.rule">]])
    set(found_CustomBuild_out_CONFIG 1)
  endif()
  if(line MATCHES [[<CustomBuild Include=".*\\out-\(CONFIG\)-\(CONFIG\)\.txt\.rule">]])
    set(found_CustomBuild_out_CONFIG_CONFIG 1)
  endif()
  if(line MATCHES [[<CustomBuild Include=".*\\[0-9A-Fa-f]+\.rule">]])
    set(found_CustomBuild_out_HASH 1)
  endif()
endforeach()
if(NOT found_CustomBuild_out)
  string(APPEND RunCMake_TEST_FAILED "CustomBuild for out.txt.rule not found in\n  ${vcProjectFile}\n")
endif()
if(NOT found_CustomBuild_out_CONFIG)
  string(APPEND RunCMake_TEST_FAILED "CustomBuild for out-(CONFIG).txt.rule not found in\n  ${vcProjectFile}\n")
endif()
if(NOT found_CustomBuild_out_CONFIG_CONFIG)
  string(APPEND RunCMake_TEST_FAILED "CustomBuild for out-(CONFIG)-(CONFIG).txt.rule not found in\n  ${vcProjectFile}\n")
endif()
if(NOT found_CustomBuild_out_HASH)
  string(APPEND RunCMake_TEST_FAILED "CustomBuild for <hash>.rule not found in\n  ${vcProjectFile}\n")
endif()
