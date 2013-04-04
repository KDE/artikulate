FILE(GLOB languages *.xml)
set(languages
    ca.xml
    de.xml
    du.xml
    en_US.xml
    fr.xml
    gr.xml
    it.xml
    sp.xml
)

install(FILES ${languages} DESTINATION ${DATA_INSTALL_DIR}/artikulate/languages)
