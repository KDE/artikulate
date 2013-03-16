FILE(GLOB skeletons *.xml)
set(skeletons
    basic.xml
)
install(FILES ${skeletons} DESTINATION ${DATA_INSTALL_DIR}/artikulate/skeletons)
