target_sources(DwarfQuest
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/TestBehaviour.cpp
        ${CMAKE_CURRENT_LIST_DIR}/TestBehaviourB.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Renderer.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/TestBehaviour.h
        ${CMAKE_CURRENT_LIST_DIR}/TestBehaviourB.h
        ${CMAKE_CURRENT_LIST_DIR}/Renderer.h
        
)