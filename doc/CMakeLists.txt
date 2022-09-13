# Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)

  # Doxyfile options
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen-awesome.css)
  set(DOXYGEN_PROJECT_BRIEF "A curling simulation system for curling AIs")
  set(DOXYGEN_PROJECT_LOGO "logo.svg")

  # configure doxygen
  doxygen_add_docs(digital_curling__doc
    ${DIGITAL_CURLING_INCLUDE_FILES}
    mainpage.md
    making_ai.md
    server.md
    protocol.md
    players.md
    simulators.md
    coordinate.md
    json.md
    extend_library.md
  )

  # copy logo.svg
  add_custom_command(TARGET digital_curling__doc PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/logo.svg ${CMAKE_CURRENT_BINARY_DIR}
  )

  # copy figures to binary directory
  add_custom_command(TARGET digital_curling__doc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/fig ${CMAKE_CURRENT_BINARY_DIR}/html/fig
  )
endif()
