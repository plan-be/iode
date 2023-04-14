# https://doc.qt.io/qt-6.2/cmake-get-started.html

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message(CHECK_START "Search for Qt6 package")
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    list(APPEND CMAKE_PREFIX_PATH "C:\\Qt\\6.4.0\\msvc2019_64;C:\\Qt\\6.4.2\\msvc2019_64")
endif()
find_package(Qt6 COMPONENTS Core Widgets Gui Charts PrintSupport)

if(Qt6_FOUND)
message(CHECK_PASS "found")

set(GUI_UTILS   
    super.h     super.cpp 
    wrapper_classes.h 
    utils.h 
    util/double_validator.h
    util/system_item.h
    util/find_and_replace_dialog.ui     util/find_and_replace_dialog.h      util/find_and_replace_dialog.cpp
)

set(GUI_SETTINGS    
    settings.h  settings.cpp
)

set(GUI_TABS
    tabs/qiode_tab_abstract.h
    tabs/qiode_tab_database.h
    tabs/qiode_tab_text.h           tabs/qiode_tab_text.cpp
    tabs/qiode_tab_report.h         tabs/qiode_tab_report.cpp
)

set(GUI_CUSTOM_WIDGETS  
    custom_widgets/qfilechooser.h 
    custom_widgets/qsampleedit.h
    custom_widgets/qiode_completer.h            custom_widgets/qiode_completer.cpp
    custom_widgets/qiode_highlighter.h          custom_widgets/qiode_highlighter.cpp
    custom_widgets/qiode_command.h              custom_widgets/qiode_command.cpp 
    custom_widgets/text_editor.h                custom_widgets/text_editor.cpp
    custom_widgets/report_editor.h              custom_widgets/report_editor.cpp
    custom_widgets/file_delegate.h              custom_widgets/file_delegate.cpp
    custom_widgets/qiode_tab_widget.h           custom_widgets/qiode_tab_widget.cpp
    custom_widgets/qiode_file_explorer_proxy.h
    custom_widgets/qiode_file_explorer.h        custom_widgets/qiode_file_explorer.cpp
)

set(GUI_PLOT
    plot/plot.h     plot/plot.cpp
)

set(GUI_EDIT_OBJECT
    iode_objs/edit/edit_equation.ui     iode_objs/edit/edit_equation.h          iode_objs/edit/edit_equation.cpp
                                        iode_objs/edit/edit_table_view.h        iode_objs/edit/edit_table_view.cpp
                                        iode_objs/edit/edit_table_model.h       iode_objs/edit/edit_table_model.cpp
    iode_objs/edit/edit_table.ui        iode_objs/edit/edit_table.h             iode_objs/edit/edit_table.cpp
                                        iode_objs/edit/edit_table_delegate.h    iode_objs/edit/edit_table_delegate.cpp
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
    iode_objs/models/comments_model.h           iode_objs/models/comments_model.cpp
    iode_objs/models/equations_model.h          iode_objs/models/equations_model.cpp
    iode_objs/models/identities_model.h         iode_objs/models/identities_model.cpp
    iode_objs/models/lists_model.h              iode_objs/models/lists_model.cpp
    iode_objs/models/scalars_model.h            iode_objs/models/scalars_model.cpp
    iode_objs/models/tables_model.h             iode_objs/models/tables_model.cpp
    iode_objs/models/variables_model.h          iode_objs/models/variables_model.cpp
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
    iode_objs/views/comments_view.h         iode_objs/views/comments_view.cpp
    iode_objs/views/equations_view.h        iode_objs/views/equations_view.cpp
    iode_objs/views/identities_view.h       iode_objs/views/identities_view.cpp
    iode_objs/views/lists_view.h            iode_objs/views/lists_view.cpp
    iode_objs/views/scalars_view.h          iode_objs/views/scalars_view.cpp
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

set(GUI_MENU_COMPUTE
    menu/compute/compute_identities.ui          menu/compute/compute_identities.h           menu/compute/compute_identities.cpp
    menu/compute/compute_simulation.ui          menu/compute/compute_simulation.h           menu/compute/compute_simulation.cpp
    menu/compute/compute_model.ui               menu/compute/compute_model.h                menu/compute/compute_model.cpp
    menu/compute/compute_scc_decomposition.ui   menu/compute/compute_scc_decomposition.h    menu/compute/compute_scc_decomposition.cpp
    menu/compute/compute_scc_simulation.ui      menu/compute/compute_scc_simulation.h       menu/compute/compute_scc_simulation.cpp
)

set(GUI_MENU_PRINT_GRAPH
    menu/print_graph/graph_variables.ui         menu/print_graph/graph_variables.h          menu/print_graph/graph_variables.cpp
)

set(GUI_MENUS ${GUI_MENU_FILE} ${GUI_MENU_WORKSPACE} ${GUI_MENU_COMPUTE} ${GUI_MENU_PRINT_GRAPH})

set(GUI_MAIN    
                        main_window_plot.h      main_window_plot.cpp
    main_window.ui      main_window.h           main_window.cpp 
                                                main.cpp
) 

set(PROJECT_SOURCES     ${GUI_UTILS} ${GUI_SETTINGS} ${GUI_CUSTOM_WIDGETS} ${GUI_PLOT}
                        ${GUI_IODE_OBJS} ${GUI_EDIT_OBJECT} ${GUI_TABS} ${GUI_MENUS}
                        ${GUI_MAIN}
)

set(APP_ICON_WINDOWS "${IODE_RESOUCES_DIR}/iode.rc")

qt_add_executable(${IODE_GUI_TARGET} ${IODE_RESOUCES_DIR}/iode_resource.qrc ${APP_ICON_WINDOWS} MANUAL_FINALIZATION ${PROJECT_SOURCES})
target_link_libraries(${IODE_GUI_TARGET} PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::PrintSupport iodecppapi)
# By calling target_include_directories, we make sure that the absolute path to 
# the gui directory is automatically added as an include path to the ${IODE_GUI_TARGET} target.
# Required to use the custom widgets in UI files
target_include_directories(${IODE_GUI_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# disable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(${IODE_GUI_TARGET} PRIVATE /W0)
endif()

set_target_properties(${IODE_GUI_TARGET} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

qt_finalize_executable(${IODE_GUI_TARGET})

# copy user documentation to the final build directory
add_custom_command(TARGET ${IODE_GUI_TARGET} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/doc/build $<TARGET_FILE_DIR:${IODE_GUI_TARGET}>/doc)
# copy iode.msg to the final build directory
add_custom_command(TARGET ${IODE_GUI_TARGET} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/iode.msg $<TARGET_FILE_DIR:${IODE_GUI_TARGET}>/iode.msg)

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
    add_custom_command(TARGET ${IODE_GUI_TARGET} POST_BUILD
        COMMAND "${WINDEPLOYQT_ENV_SETUP}" && "${WINDEPLOYQT_EXECUTABLE}" 
            --list target 
            --no-quick-import
            --no-translations
            --verbose 1 
            "$<TARGET_FILE:${IODE_GUI_TARGET}>"
        COMMENT "Running windeployqt..."
    )
endif()

else()

message(CHECK_FAIL "not found")

endif()
