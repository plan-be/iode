# https://doc.qt.io/qt-6.2/cmake-get-started.html

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message(CHECK_START "Search for Qt6 package")
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set(CMAKE_PREFIX_PATH "C:\\Qt\\6.2.1\\msvc2019_64")
endif()
find_package(Qt6 COMPONENTS Core Widgets Gui)

if(Qt6_FOUND)
message(CHECK_PASS "found")

set(GUI_UTILS   
    super.h super.cpp 
    wrapper_classes.h 
    utils.h 
    util/double_validator.h
)

set(GUI_SETTINGS    
    settings.h  settings.cpp
)

set(GUI_CUSTOM_WIDGETS  
    custom_widgets/qfilechooser.h 
    custom_widgets/qsampleedit.h
)

set(GUI_EDIT_OBJECT
                                        iode_objs/edit/popup_view.h
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
    iode_objs/delegates/popup_delegate.h
    iode_objs/delegates/abstract_delegate.h
    iode_objs/delegates/text_delegate.h
    iode_objs/delegates/comments_delegate.h
    iode_objs/delegates/equations_delegate.h
    iode_objs/delegates/identities_delegate.h
    iode_objs/delegates/lists_delegate.h
    iode_objs/delegates/scalars_delegate.h
    iode_objs/delegates/variables_delegate.h
    iode_objs/delegates/tables_delegate.h
)

set(GUI_OBJS_VIEWS
    iode_objs/views/abstract_table_view.h
    iode_objs/views/comments_view.h         iode_objs/views/comments_view.cpp
    iode_objs/views/equations_view.h        iode_objs/views/equations_view.cpp
    iode_objs/views/identities_view.h       iode_objs/views/identities_view.cpp
    iode_objs/views/lists_view.h            iode_objs/views/lists_view.cpp
    iode_objs/views/scalars_view.h          iode_objs/views/scalars_view.cpp
    iode_objs/views/tables_view.h           iode_objs/views/tables_view.cpp
    iode_objs/views/variables_view.h        iode_objs/views/variables_view.cpp
)

set(GUI_IODE_OBJS 
    ${GUI_NEW_OBJECT} ${GUI_EDIT_OBJECT} 
    ${GUI_OBJS_MODELS} ${GUI_OBJS_DELEGATES} ${GUI_OBJS_VIEWS}
)

set(GUI_MENU_FILE
    menu/file/file_import_comments.ui           menu/file/file_import_comments.h        menu/file/file_import_comments.cpp
    menu/file/file_import_variables.ui          menu/file/file_import_variables.h       menu/file/file_import_variables.cpp
)

set(GUI_MENU_WORKSPACE
    menu/workspace/workspace_load.ui            menu/workspace/workspace_load.h         menu/workspace/workspace_load.cpp
    menu/workspace/workspace_save.ui            menu/workspace/workspace_save.h         menu/workspace/workspace_save.cpp
    menu/workspace/workspace_clear.ui           menu/workspace/workspace_clear.h        menu/workspace/workspace_clear.cpp
    menu/workspace/workspace_high_to_low.ui     menu/workspace/workspace_high_to_low.h  menu/workspace/workspace_high_to_low.cpp
    menu/workspace/workspace_low_to_high.ui     menu/workspace/workspace_low_to_high.h  menu/workspace/workspace_low_to_high.cpp
)

set(GUI_MENU_COMPUTE
    menu/compute/compute_identities.ui          menu/compute/compute_identities.h           menu/compute/compute_identities.cpp
    menu/compute/compute_simulation.ui          menu/compute/compute_simulation.h           menu/compute/compute_simulation.cpp
    menu/compute/compute_model.ui               menu/compute/compute_model.h                menu/compute/compute_model.cpp
    menu/compute/compute_scc_decomposition.ui   menu/compute/compute_scc_decomposition.h    menu/compute/compute_scc_decomposition.cpp
    menu/compute/compute_scc_simulation.ui      menu/compute/compute_scc_simulation.h       menu/compute/compute_scc_simulation.cpp
)

set(GUI_MENUS ${GUI_MENU_FILE} ${GUI_MENU_WORKSPACE} ${GUI_MENU_COMPUTE})

set(GUI_MAIN    
    main_window.ui  main_window.h   main_window.cpp 
    main.cpp
) 

set(PROJECT_SOURCES     ${GUI_UTILS} ${GUI_SETTINGS} ${GUI_CUSTOM_WIDGETS} 
                        ${GUI_IODE_OBJS} ${GUI_EDIT_OBJECT}  ${GUI_MENUS}
                        ${GUI_MAIN}
)

qt_add_executable(iode_gui MANUAL_FINALIZATION ${PROJECT_SOURCES})
target_link_libraries(iode_gui PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui iodecppapi)
# By calling target_include_directories, we make sure that the absolute path to 
# the gui directory is automatically added as an include path to the iode_gui target.
# Required to use the custom widgets in UI files
target_include_directories(iode_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# disable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(iode_gui PRIVATE /W0)
endif()

set_target_properties(iode_gui PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

qt_finalize_executable(iode_gui)

if(WIN32)
    # If Windows: Need to copy Qt DLLs in the same directory as generated executable
    # See https://doc.qt.io/qt-6.2/windows-deployment.html
    # and https://stackoverflow.com/a/66736820

    # Retrieve the absolute path to qmake and then use that path to find
    # the windeployqt executable
    get_target_property(QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)

    find_program(WINDEPLOYQT_ENV_SETUP qtenv2.bat HINTS "${QT_BIN_DIR}")
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

    # Run windeployqt immediately after build
    add_custom_command(TARGET iode_gui POST_BUILD
        COMMAND "${WINDEPLOYQT_ENV_SETUP}" && "${WINDEPLOYQT_EXECUTABLE}" --list target --verbose 1 "$<TARGET_FILE:iode_gui>"
        COMMENT "Running windeployqt..."
    )
endif()

else()

message(CHECK_FAIL "not found")

endif()
