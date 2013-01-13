FILE(GLOB courses *.xml)
set(courses
    fr.xml
)

install(FILES ${courses} DESTINATION ${DATA_INSTALL_DIR}/artikulate/courses)

