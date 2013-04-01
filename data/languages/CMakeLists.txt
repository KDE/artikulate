FILE(GLOB languages *.xml)
set(languages
    de.xml
    fr.xml
    en_US.xml
    du.xml
    it.xml
)

install(FILES ${languages} DESTINATION ${DATA_INSTALL_DIR}/artikulate/languages)
