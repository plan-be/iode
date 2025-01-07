
message(STATUS "\n---- GUI (QT) ----\n")

file(INSTALL ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

set(RESOURCE_FILENAME "iode_resource")
set(RC_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/resources/${RESOURCE_FILENAME}.rc")
set(QRC_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/resources/${RESOURCE_FILENAME}.qrc")
set(PY_RESOURCE_FILENAME "${RESOURCE_FILENAME}_rc")
set(PY_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/${PY_RESOURCE_FILENAME}.py")

# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/__init__.py.in ${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/__init__.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/utils.py.in ${CMAKE_CURRENT_SOURCE_DIR}/iode_gui/utils.py)

if(SKBUILD)
	message(STATUS "Skip target ${IODE_CPP_GUI_TARGET}\n")
else()

# https://doc.qt.io/qt-6.2/cmake-get-started.html

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(Qt6_FOUND)
set(GUI_UTILS   
    super.h     super.cpp 
    wrapper_classes.h 
    utils.h 
    util/widgets/file_chooser.h
    util/widgets/sample_edit.h
    util/double_validator.h
    util/system_item.h
)

set(GUI_SETTINGS    
    settings.h  settings.cpp
)

set(GUI_PRINT 
                                        print/print.h                       print/print.cpp
    print/print_file_dialog.ui          print/print_file_dialog.h           print/print_file_dialog.cpp
    print/file_print_preferences.ui     print/file_print_preferences.h      print/file_print_preferences.cpp
)

set(GUI_TEXT_EDIT 
                                            text_edit/completer.h                   text_edit/completer.cpp
                                            text_edit/highlighter.h                 text_edit/highlighter.cpp
                                            text_edit/complete_line_edit.h          text_edit/complete_line_edit.cpp
                                            text_edit/complete_text_edit.h          text_edit/complete_text_edit.cpp
                                            text_edit/text_editor.h                 text_edit/text_editor.cpp
                                            text_edit/report_editor.h               text_edit/report_editor.cpp
    text_edit/find_and_replace_dialog.ui    text_edit/find_and_replace_dialog.h     text_edit/find_and_replace_dialog.cpp
)

set(GUI_TABS
                            tabs/tab_abstract.h
                            tabs/iode_objs/tab_database_abstract.h
                            tabs/iode_objs/tab_numerical_values.h
                            tabs/iode_objs/tab_computed_table.h
                            tabs/iode_objs/tab_database.h
                            tabs/tab_text_abstract.h                tabs/tab_text_abstract.cpp
    tabs/tab_text.ui        tabs/tab_text.h                         tabs/tab_text.cpp
    tabs/tab_report.ui      tabs/tab_report.h                       tabs/tab_report.cpp
)

set(GUI_MAIN_WIDGETS  
    main_widgets/file_explorer/file_delegate.h            main_widgets/file_explorer/file_delegate.cpp
    main_widgets/file_explorer/file_explorer_proxy.h
    main_widgets/file_explorer/file_explorer.h            main_widgets/file_explorer/file_explorer.cpp
    main_widgets/tab_widget/tab_widget_abstract.h         main_widgets/tab_widget/tab_widget_abstract.cpp
    main_widgets/tab_widget/tab_widget.h                  main_widgets/tab_widget/tab_widget.cpp
    main_widgets/iode_command.h                           main_widgets/iode_command.cpp 
)

set(GUI_PLOT
    plot/plot.h         plot/plot.cpp
    plot/plot_vars.h    plot/plot_vars.cpp
    plot/plot_table.h   plot/plot_table.cpp
)

set(GUI_EDIT_OBJECT
    iode_objs/edit/edit_iode_obj.ui     iode_objs/edit/edit_iode_obj.h          iode_objs/edit/edit_iode_obj.cpp
                                        iode_objs/edit/edit_comment.h
    iode_objs/edit/edit_equation.ui     iode_objs/edit/edit_equation.h          iode_objs/edit/edit_equation.cpp
                                        iode_objs/edit/edit_identity.h
                                        iode_objs/edit/edit_list.h
                                        iode_objs/edit/edit_table_view.h        iode_objs/edit/edit_table_view.cpp
                                        iode_objs/edit/edit_table_model.h       iode_objs/edit/edit_table_model.cpp
                                        iode_objs/edit/edit_table_delegate.h    iode_objs/edit/edit_table_delegate.cpp
    iode_objs/edit/edit_table.ui        iode_objs/edit/edit_table.h             iode_objs/edit/edit_table.cpp
    iode_objs/edit/edit_vars_sample.ui  iode_objs/edit/edit_vars_sample.h       iode_objs/edit/edit_vars_sample.cpp
)

set(GUI_NEW_OBJECT
    iode_objs/new/add_object.ui 
                                    iode_objs/new/add_comment.h     iode_objs/new/add_comment.cpp
                                    iode_objs/new/add_identity.h    iode_objs/new/add_identity.cpp
                                    iode_objs/new/add_list.h        iode_objs/new/add_list.cpp
    iode_objs/new/add_scalar.ui     iode_objs/new/add_scalar.h      iode_objs/new/add_scalar.cpp
    iode_objs/new/add_table.ui      iode_objs/new/add_table.h       iode_objs/new/add_table.cpp
                                    iode_objs/new/add_variable.h    iode_objs/new/add_variable.cpp 
)

set(GUI_OBJS_MODELS     
    iode_objs/models/abstract_table_model.h     iode_objs/models/abstract_table_model.cpp
    iode_objs/models/numerical_table_model.h    
    iode_objs/models/comments_model.h           iode_objs/models/comments_model.cpp
    iode_objs/models/equations_model.h          iode_objs/models/equations_model.cpp
    iode_objs/models/identities_model.h         iode_objs/models/identities_model.cpp
    iode_objs/models/lists_model.h              iode_objs/models/lists_model.cpp
    iode_objs/models/scalars_model.h            iode_objs/models/scalars_model.cpp
    iode_objs/models/tables_model.h             iode_objs/models/tables_model.cpp
    iode_objs/models/variables_model.h          iode_objs/models/variables_model.cpp
    iode_objs/models/computed_table_model.h     iode_objs/models/computed_table_model.cpp    
)

set(GUI_OBJS_DELEGATES
    iode_objs/delegates/base_delegate.h
    iode_objs/delegates/comments_delegate.h
    iode_objs/delegates/equations_delegate.h
    iode_objs/delegates/identities_delegate.h
    iode_objs/delegates/lists_delegate.h
    iode_objs/delegates/scalars_delegate.h
    iode_objs/delegates/variables_delegate.h
    iode_objs/delegates/tables_delegate.h
)

set(GUI_OBJS_VIEWS
    iode_objs/views/abstract_table_view.h   iode_objs/views/abstract_table_view.cpp
    iode_objs/views/numerical_view.h
    iode_objs/views/computed_table_view.h
    iode_objs/views/comments_view.h
    iode_objs/views/equations_view.h
    iode_objs/views/identities_view.h       iode_objs/views/identities_view.cpp
    iode_objs/views/lists_view.h
    iode_objs/views/scalars_view.h
    iode_objs/views/tables_view.h           iode_objs/views/tables_view.cpp
    iode_objs/views/variables_view.h        iode_objs/views/variables_view.cpp
)

set(GUI_IODE_MISC
    iode_objs/misc/unit_root.ui         iode_objs/misc/unit_root.h          iode_objs/misc/unit_root.cpp
    iode_objs/misc/dyn_adjustment.ui    iode_objs/misc/dyn_adjustment.h     iode_objs/misc/dyn_adjustment.cpp
)

set(GUI_IODE_ESTIMATION
    iode_objs/estimation/estimation_coefs.ui    iode_objs/estimation/estimation_coefs.h
                                                iode_objs/estimation/correlation_matrix_model.h     iode_objs/estimation/correlation_matrix_model.cpp
                                                iode_objs/estimation/tests_eqs_model.h              iode_objs/estimation/tests_eqs_model.cpp        
    iode_objs/estimation/estimation_results.ui  iode_objs/estimation/estimation_results.h           iode_objs/estimation/estimation_results.cpp
)

set(GUI_IODE_OBJS 
    ${GUI_NEW_OBJECT} ${GUI_EDIT_OBJECT} ${GUI_OBJS_WIDGET}
    ${GUI_OBJS_MODELS} ${GUI_OBJS_DELEGATES} ${GUI_OBJS_VIEWS}
    ${GUI_IODE_MISC} ${GUI_IODE_ESTIMATION}
)

set(GUI_MENU_FILE
    menu/file/file_new_project.ui               menu/file/file_new_project.h            menu/file/file_new_project.cpp
    menu/file/file_import_comments.ui           menu/file/file_import_comments.h        menu/file/file_import_comments.cpp
    menu/file/file_import_variables.ui          menu/file/file_import_variables.h       menu/file/file_import_variables.cpp
    menu/file/file_export.ui                    menu/file/file_export.h                 menu/file/file_export.cpp
    menu/file/file_settings.ui               menu/file/file_settings.h            menu/file/file_settings.cpp
)

set(GUI_MENU_WORKSPACE
    menu/workspace/workspace_copy_into.ui               menu/workspace/workspace_copy_into.h                menu/workspace/workspace_copy_into.cpp
    menu/workspace/workspace_merge_into.ui              menu/workspace/workspace_merge_into.h               menu/workspace/workspace_merge_into.cpp
    menu/workspace/workspace_describe.ui                menu/workspace/workspace_describe.h                 menu/workspace/workspace_describe.cpp
    menu/workspace/workspace_extrapolate_variables.ui   menu/workspace/workspace_extrapolate_variables.h    menu/workspace/workspace_extrapolate_variables.cpp
    menu/workspace/workspace_high_to_low.ui             menu/workspace/workspace_high_to_low.h              menu/workspace/workspace_high_to_low.cpp
    menu/workspace/workspace_low_to_high.ui             menu/workspace/workspace_low_to_high.h              menu/workspace/workspace_low_to_high.cpp
    menu/workspace/workspace_seasonal_adjustment.ui     menu/workspace/workspace_seasonal_adjustment.h      menu/workspace/workspace_seasonal_adjustment.cpp
    menu/workspace/workspace_trend_correction.ui        menu/workspace/workspace_trend_correction.h         menu/workspace/workspace_trend_correction.cpp
)

set(GUI_MENU_DATA 
    menu/data/sort_expand_list.ui       menu/data/sort_expand_list.h        menu/data/sort_expand_list.cpp
    menu/data/list_calculus.ui          menu/data/list_calculus.h           menu/data/list_calculus.cpp
    menu/data/search_text.ui            menu/data/search_text.h             menu/data/search_text.cpp
    menu/data/scan_objects.ui           menu/data/scan_objects.h            menu/data/scan_objects.cpp
    menu/data/file_contents.ui          menu/data/file_contents.h           menu/data/file_contents.cpp
    menu/data/file_compare.ui           menu/data/file_compare.h            menu/data/file_compare.cpp
    menu/data/duplicate.ui              menu/data/duplicate.h               menu/data/duplicate.cpp
    menu/data/edit_tables.ui            menu/data/edit_tables.h             menu/data/edit_tables.cpp
)

set(GUI_MENU_COMPUTE
    menu/compute/compute_identities.ui          menu/compute/compute_identities.h           menu/compute/compute_identities.cpp
    menu/compute/compute_simulation.ui          menu/compute/compute_simulation.h           menu/compute/compute_simulation.cpp
    menu/compute/compute_model.ui               menu/compute/compute_model.h                menu/compute/compute_model.cpp
    menu/compute/compute_scc_decomposition.ui   menu/compute/compute_scc_decomposition.h    menu/compute/compute_scc_decomposition.cpp
    menu/compute/compute_scc_simulation.ui      menu/compute/compute_scc_simulation.h       menu/compute/compute_scc_simulation.cpp
)

set(GUI_MENU_PRINT_GRAPH
                                            menu/print_graph/print_abstract.h           menu/print_graph/print_abstract.cpp    
menu/print_graph/print_tables.ui            menu/print_graph/print_tables.h             menu/print_graph/print_tables.cpp
menu/print_graph/print_variables.ui         menu/print_graph/print_variables.h          menu/print_graph/print_variables.cpp
menu/print_graph/graph_tables.ui            menu/print_graph/graph_tables.h             menu/print_graph/graph_tables.cpp
menu/print_graph/graph_variables.ui         menu/print_graph/graph_variables.h          menu/print_graph/graph_variables.cpp
)

set(GUI_MENUS ${GUI_MENU_FILE} ${GUI_MENU_WORKSPACE} ${GUI_MENU_DATA} ${GUI_MENU_COMPUTE} ${GUI_MENU_PRINT_GRAPH})

set(GUI_MAIN    
                        main_window_abstract.h      main_window_abstract.cpp
    main_window.ui      main_window.h               main_window.cpp 
                                                    main.cpp
) 

set(PROJECT_SOURCES     ${GUI_UTILS} ${GUI_SETTINGS} ${GUI_PRINT} ${GUI_TEXT_EDIT} 
                        ${GUI_PLOT} ${GUI_IODE_OBJS} ${GUI_EDIT_OBJECT} ${GUI_TABS} 
                        ${GUI_MAIN_WIDGETS} ${GUI_MENUS} ${GUI_MAIN}
)

qt_add_executable(${IODE_CPP_GUI_TARGET} ${QRC_RESOURCE_FILE} ${RC_RESOURCE_FILE} MANUAL_FINALIZATION ${PROJECT_SOURCES})
target_link_libraries(${IODE_CPP_GUI_TARGET} PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::PrintSupport iode_cpp_api)
# By calling target_include_directories, we make sure that the absolute path to 
# the gui directory is automatically added as an include path to the ${IODE_CPP_GUI_TARGET} target.
# Required to use the custom widgets in UI files
target_include_directories(${IODE_CPP_GUI_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# disable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${IODE_CPP_GUI_TARGET} PRIVATE /W0)
endif()

set_target_properties(${IODE_CPP_GUI_TARGET} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

qt_finalize_executable(${IODE_CPP_GUI_TARGET})

# copy user documentation to the final build directory
add_custom_command(TARGET ${IODE_CPP_GUI_TARGET} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/doc/build $<TARGET_FILE_DIR:${IODE_CPP_GUI_TARGET}>/doc)
# copy iode.msg to the final build directory
add_custom_command(TARGET ${IODE_CPP_GUI_TARGET} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/iode.msg $<TARGET_FILE_DIR:${IODE_CPP_GUI_TARGET}>/iode.msg)

if(WIN32)
    # If Windows: Need to copy Qt DLLs in the same directory as generated executable
    # See https://doc.qt.io/qt-6/windows-deployment.html
    # and https://stackoverflow.com/a/66736820

    # Retrieve the absolute path to qmake and then use that path to find
    # the windeployqt executable
    get_target_property(QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)

    find_program(WINDEPLOYQT_ENV_SETUP qtenv2.bat HINTS "${QT_BIN_DIR}")
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

    # Run windeployqt immediately after build
    add_custom_command(TARGET ${IODE_CPP_GUI_TARGET} POST_BUILD
        COMMAND "${WINDEPLOYQT_ENV_SETUP}" && "${WINDEPLOYQT_EXECUTABLE}" 
            --list target 
            --no-quick-import
            --no-translations
            --verbose 1 
            "$<TARGET_FILE:${IODE_CPP_GUI_TARGET}>"
        COMMENT "Running windeployqt..."
    )

message(STATUS "New target ${IODE_CPP_GUI_TARGET} enabled")

endif()

else()

message(WARNING "Qt not found")

endif()

endif()
