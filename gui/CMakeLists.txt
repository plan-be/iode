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

set(GUI_POPUP
    tabs/popup_view.h

    tabs/popup/edit_equation.ui         tabs/popup/edit_equation.h          tabs/popup/edit_equation.cpp
                                        tabs/popup/edit_table_view.h        tabs/popup/edit_table_view.cpp
                                        tabs/popup/edit_table_model.h       tabs/popup/edit_table_model.cpp
    tabs/popup/edit_table.ui            tabs/popup/edit_table.h             tabs/popup/edit_table.cpp
    tabs/popup/edit_vars_sample.ui      tabs/popup/edit_vars_sample.h       tabs/popup/edit_vars_sample.cpp
                                        tabs/popup/edit_table_delegate.h    tabs/popup/edit_table_delegate.cpp

    tabs/popup/add_object.ui 
                                        tabs/popup/add_comment.h            tabs/popup/add_comment.cpp
                                        tabs/popup/add_identity.h           tabs/popup/add_identity.cpp
                                        tabs/popup/add_list.h               tabs/popup/add_list.cpp
    tabs/popup/add_scalar.ui            tabs/popup/add_scalar.h             tabs/popup/add_scalar.cpp
    tabs/popup/add_table.ui             tabs/popup/add_table.h              tabs/popup/add_table.cpp
                                        tabs/popup/add_variable.h           tabs/popup/add_variable.cpp 
)

set(GUI_TABS_MODELS     
    tabs/abstract_table_model.h tabs/abstract_table_model.cpp
    tabs/comments_model.h       tabs/comments_model.cpp
    tabs/equations_model.h      tabs/equations_model.cpp
    tabs/identities_model.h     tabs/identities_model.cpp
    tabs/lists_model.h          tabs/lists_model.cpp
    tabs/scalars_model.h        tabs/scalars_model.cpp
    tabs/tables_model.h         tabs/tables_model.cpp
    tabs/variables_model.h      tabs/variables_model.cpp
)

set(GUI_TABS_DELEGATES
    tabs/abstract_delegate.h
    tabs/popup_delegate.h
    tabs/text_delegate.h
    tabs/comments_delegate.h
    tabs/equations_delegate.h
    tabs/identities_delegate.h
    tabs/lists_delegate.h
    tabs/scalars_delegate.h
    tabs/variables_delegate.h
    tabs/tables_delegate.h
)

set(GUI_TABS_VIEWS
    tabs/abstract_table_view.h
    tabs/comments_view.h    tabs/comments_view.cpp
    tabs/equations_view.h   tabs/equations_view.cpp
    tabs/identities_view.h  tabs/identities_view.cpp
    tabs/lists_view.h       tabs/lists_view.cpp
    tabs/scalars_view.h     tabs/scalars_view.cpp
    tabs/tables_view.h      tabs/tables_view.cpp
    tabs/variables_view.h   tabs/variables_view.cpp
)

set(GUI_TABS ${GUI_TABS_DELEGATES} ${GUI_TABS_MODELS} ${GUI_TABS_VIEWS})

set(GUI_MENU_WORKSPACE
    menu/workspace/workspace_load.ui    menu/workspace/workspace_load.h     menu/workspace/workspace_load.cpp
    menu/workspace/workspace_save.ui    menu/workspace/workspace_save.h     menu/workspace/workspace_save.cpp
    menu/workspace/workspace_clear.ui   menu/workspace/workspace_clear.h    menu/workspace/workspace_clear.cpp
)

set(GUI_MENU_COMPUTE
    menu/compute/compute_identities.ui  menu/compute/compute_identities.h   menu/compute/compute_identities.cpp
    menu/compute/compute_simulation.ui  menu/compute/compute_simulation.h   menu/compute/compute_simulation.cpp
)

set(GUI_MENUS ${GUI_MENU_WORKSPACE} ${GUI_MENU_COMPUTE})

set(GUI_MAIN    
    main_window.ui  main_window.h   main_window.cpp 
    main.cpp
) 

set(PROJECT_SOURCES     ${GUI_UTILS} ${GUI_SETTINGS} ${GUI_CUSTOM_WIDGETS} 
                        ${GUI_POPUP} ${GUI_TABS} ${GUI_MENUS}
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
