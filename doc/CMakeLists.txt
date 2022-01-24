# Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)
  doxygen_add_docs(digital_curling__doc
    ../digital_curling/include/
    doc.md
  )
endif()
