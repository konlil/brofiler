SEPARATE_ARGUMENTS( FILES_LIST )

FOREACH( ENTRY ${FILES_LIST} )
    MESSAGE( "copying: ${ENTRY} to ${DESTINATION}" )
    FILE( COPY ${ENTRY} DESTINATION ${DESTINATION} PATTERN "${EXCLUDE_EXT}" EXCLUDE )
ENDFOREACH( ENTRY )