target_sources(DwarfQuest
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Logger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Testing.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Logger.h
        ${CMAKE_CURRENT_LIST_DIR}/Testing.h
)