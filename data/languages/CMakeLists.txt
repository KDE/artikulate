FILE(GLOB languages *.xml)
set(languages
    ba.xml
    ca.xml
    de.xml
    du.xml
    en_BE.xml
    en_US.xml
    fr.xml
    gr.xml
    it.xml
    mr.xml
    sp.xml
)

install(FILES ${languages} DESTINATION ${DATA_INSTALL_DIR}/artikulate/languages)

