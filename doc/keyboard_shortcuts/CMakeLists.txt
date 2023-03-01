FIND_PACKAGE(LATEX)

message(CHECK_START "Search for LaTeX tools")
IF(LATEX_FOUND AND LATEX_PDFLATEX_FOUND)
    MESSAGE(CHECK_PASS "LaTeX tools found!")
    message(STATUS "LaTeX compiler:       ${LATEX_COMPILER}")
    message(STATUS "PdfLaTeX compiler:    ${PDFLATEX_COMPILER}")

    # The directory where the final build data will be stored.
    SET(OUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    # Latex source file.
    SET(MAIN_DOC keyboard_shortcuts)
    SET(MAIN_TEX ${MAIN_DOC}.tex)

	# First pass.
	ADD_CUSTOM_TARGET( latex-prebuild
			COMMAND ${PDFLATEX_COMPILER}  -output-directory ${OUT_DIRECTORY} -draftmode -interaction=nonstopmode ${MAIN_TEX}
			COMMENT "Starting Prebuild."
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			DEPENDS ${MAIN_TEX})

	# Second pass - generate the final pdf.
	ADD_CUSTOM_TARGET( latex-pdf	
				COMMAND ${PDFLATEX_COMPILER} -output-directory ${OUT_DIRECTORY}  ${MAIN_TEX}
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				COMMENT "Assembling the final pdf file."
				DEPENDS  ${MAIN_TEX})

	ADD_CUSTOM_TARGET(keyboard_shortcuts ALL) # Entry point of execution.
	ADD_DEPENDENCIES(keyboard_shortcuts latex-pdf)

	ADD_CUSTOM_COMMAND(TARGET keyboard_shortcuts POST_BUILD 
	                   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${MAIN_DOC}.pdf" ${CMAKE_SOURCE_DIR}/doc/build
					   COMMENT "Copying keyboard_shortcuts.pdf to the doc/build folder")
ELSE()
	MESSAGE(CHECK_FAIL "No latex tools found!")
ENDIF()
