file(GLOB HEADER_FILES *.hpp)

add_executable(DH-wad
   ${HEADER_FILES}

   main.cpp

   Except.cpp
   io.cpp
   Lump.cpp
   LumpData.cpp
   LumpFile.cpp
   LumpNull.cpp
   LumpWad.cpp
   opt_dir.cpp
   opt_map.cpp
   opt_spec.cpp
   opt_wad.cpp
   option.cpp
   Wad.cpp
)

