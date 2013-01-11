FILE(GLOB languages *.xml)
set(languages
    de.xml
    fr.xml
)

install(FILES ${languages} DESTINATION ${DATA_INSTALL_DIR}/artikulate/languages)
