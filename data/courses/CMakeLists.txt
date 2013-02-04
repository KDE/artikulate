FILE(GLOB courses *.xml)
set(courses
    de.xml
    fr.xml
)

install(FILES ${courses} DESTINATION ${DATA_INSTALL_DIR}/artikulate/courses)

