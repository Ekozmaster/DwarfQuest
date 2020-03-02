target_sources(DwarfQuest
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/GameObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Transform.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/GameObject.h
        ${CMAKE_CURRENT_LIST_DIR}/Transform.h
)