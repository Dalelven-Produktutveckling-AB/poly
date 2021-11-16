set(POLY_BUNDLE_ETL OFF)

if (NOT POLY_ETL_INCLUDE_DIR)
    set(POLY_ETL_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/etl/include)
    set(POLY_BUNDLE_ETL ON)
endif()

add_library(poly-etl INTERFACE)
target_include_directories(poly-etl INTERFACE
                            $<BUILD_INTERFACE:${POLY_ETL_INCLUDE_DIR}>
                            )

if (POLY_BUNDLE_ETL)
    install(DIRECTORY etl/include/etl
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/poly/_external)
    target_include_directories(poly-etl INTERFACE
                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/poly/_external>
                            )
endif()

add_library(poly::etl ALIAS poly-etl)

