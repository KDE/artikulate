FILE(GLOB courses *.xml)
set(courses
    de.xml
    fr.xml
)
install(FILES ${courses} DESTINATION ${DATA_INSTALL_DIR}/artikulate/courses)

set(audioTestfiles
    de_01.ogg
    de_02.ogg
)
install(FILES ${audioTestfiles} DESTINATION ${DATA_INSTALL_DIR}/artikulate/courses)
