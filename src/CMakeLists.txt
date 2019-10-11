project(delphibackup)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( delphibackup delphibackup delphibackup.cpp )
target_include_directories( delphibackup PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( delphibackup ${CMAKE_SOURCE_DIR}/../ricardian )