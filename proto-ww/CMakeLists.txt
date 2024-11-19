if(WIN32)
    otr_module(proto-wibblewobble)
    target_link_libraries(opentrack-proto-wibblewobble opentrack-csv)
endif()
