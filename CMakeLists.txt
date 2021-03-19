# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

cmake_minimum_required(VERSION 3.13)

set(SOURCE_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BINARY_TOP_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1)

set(out_hex_dir ${CMAKE_CURRENT_BINARY_DIR}/hex)
set(out_lib_dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(out_inc_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
set(tools_dir ${CMAKE_CURRENT_SOURCE_DIR}/tools)

set(core_stub_o ${CMAKE_CURRENT_SOURCE_DIR}/components/lib/core_stub.o)
set(flash_ldscript ${SOURCE_TOP_DIR}/ldscripts/app_flashimg.ld)
set(file_ldscript ${SOURCE_TOP_DIR}/ldscripts/app_fileimg.ld)
set(cmd_mkappimg dtools mkappimg)
set(pacgen_py ${tools_dir}/pacgen.py)

include(cmake/core_config.cmake)
include(cmake/toolchain-gcc.cmake)
set(BUILD_SHARED_LIBS OFF)

project(core C CXX ASM)

if(CONFIG_APPIMG_LOAD_FLASH_EYBOND)
ADD_DEFINITIONS(-D_PLATFORM_L610_=\"EYBOND_L610\")
include_directories(components/include components/newlib/include)
include_directories(eybondapp/lib)
include_directories(eybondapp/public)
include_directories(eybondapp/app)
include_directories(eybondapp/net)
include_directories(eybondapp/device)
include_directories(eybondapp/device/Manufactor)
include_directories(eybondapp/server/Eybond)
include_directories(eybondapp/server/CommonServer)
include_directories(eybondapp/server/StateGrid)
include_directories(eybondapp/fota)
else()
include_directories(components/include components/newlib/include app/include)
endif()

function(cpp_only target file)
    add_library(${target} OBJECT ${file})
    set_source_files_properties(${file} PROPERTIES LANGUAGE C)
    target_compile_options(${target} PRIVATE -E -P -x c)
endfunction()

function(add_appimg target ldscript)
    set(gen_ldscript ${target}_ldscript)
    set(target_map_file ${out_hex_dir}/${target}.map)
    set(target_img_file ${out_hex_dir}/${target}.bin)
    cpp_only(${gen_ldscript} ${ldscript})
    add_executable(${target} ${ARGN} ${core_stub_o})
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${out_hex_dir})
    target_link_libraries(${target} PRIVATE -T $<TARGET_OBJECTS:${gen_ldscript}>)
    target_link_libraries(${target} PRIVATE -Wl,-Map=${target_map_file} -nostdlib -Wl,--gc-sections)

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${cmd_mkappimg} $<TARGET_FILE:${target}> ${target_img_file}
        BYPRODUCTS ${target_img_file} ${target_map_file}
    )
endfunction()

configure_file(${SOURCE_TOP_DIR}/components/lib/fdl1.img ${out_hex_dir}/fdl1.img COPYONLY)
configure_file(${SOURCE_TOP_DIR}/components/lib/fdl2.img ${out_hex_dir}/fdl2.img COPYONLY)
macro(pac_init_fdl cmd)
    set(${cmd}
        cfg-init --pname UIX8910_MODEM --palias "APPIMG"
            --pversion "8910 MODULE" --version "BP_R1.0.0"
            --flashtype 1
        cfg-host-fdl -a ${CONFIG_FDL1_IMAGE_START} -s ${CONFIG_FDL1_IMAGE_SIZE}
            -p ${out_hex_dir}/fdl1.img
        cfg-fdl2 -a ${CONFIG_FDL2_IMAGE_START} -s ${CONFIG_FDL2_IMAGE_SIZE}
            -p ${out_hex_dir}/fdl2.img
    )
endmacro()

if(CONFIG_APPIMG_LOAD_FLASH)
    set(target appimg_flash_delete)
    set(pac_file ${out_hex_dir}/${target}.pac)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-erase-flash -i ERASE_APPIMG
                -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                -s ${CONFIG_APPIMG_FLASH_SIZE}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()

if(CONFIG_APPIMG_LOAD_FILE)
    set(target appimg_file_delete)
    set(pac_file ${out_hex_dir}/${target}.pac)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-del-appimg -i DEL_APPIMG
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()
string(TIMESTAMP VERSION "%Y%m%d%H%M%S")
string(TIMESTAMP LONGDATE "%y%m%d")
string(TIMESTAMP LONGTIME "%H%M%S")
if(CONFIG_APPIMG_LOAD_FLASH)    
if(CONFIG_APPIMG_LOAD_FLASH_EYBOND)
    set(CUSTOMER_ID 000)
    set(PLATFORM ${CUSTOMER_ID}_4G04_02_)
    set(MEMORY BP_V)
    set(APPVERSION 6.1.2.0)
    set(target ${PLATFORM}${MEMORY}${APPVERSION}_${VERSION})
    set(__FWVERSION__ ${PLATFORM}${MEMORY}${APPVERSION})
    set(_BUILDINFO_ EybondFWG)
    ADD_DEFINITIONS(-Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function)
    ADD_DEFINITIONS(-DMAKE_TIME=\"${VERSION}\")
    ADD_DEFINITIONS(-DMAKE_DATE=\"${LONGDATE}\")
    ADD_DEFINITIONS(-DMAKE_TIME2=\"${LONGTIME}\")
    ADD_DEFINITIONS(-DFWVERSION=\"${__FWVERSION__}\")
    ADD_DEFINITIONS(-D__APPVERSION__=\"${APPVERSION}\")
    ADD_DEFINITIONS(-DCUSTOMER=\"${CUSTOMER_ID}\")
    ADD_DEFINITIONS(-DBUILDINFO=\"${_BUILDINFO_}\")
    file(GLOB MAIN_FILES eybondapp/main/*.c)
    file(GLOB APP_FILES eybondapp/app/*.c)
    file(GLOB NET_FILES eybondapp/net/*.c)
    file(GLOB DEVICES_FILES eybondapp/device/*.c)
    file(GLOB DEVICES_MANUFACTOR_FILES eybondapp/device/Manufactor/*.c)
    file(GLOB SERVER_EYBOND_FILES eybondapp/server/Eybond/*.c)
    file(GLOB SERVER_COMMON_FILES eybondapp/server/CommonServer/*.c)
    file(GLOB SERVER_STATEGRID_FILES eybondapp/server/StateGrid/*.c)
    file(GLOB FOTA_FILES eybondapp/fota/*.c)
    file(GLOB LIB_FILES eybondapp/lib/*.c)
    file(GLOB PUB_FILES eybondapp/public/*.c)
    add_appimg(${target} ${flash_ldscript}
    ${MAIN_FILES}
    ${APP_FILES}
    ${NET_FILES}
    ${DEVICES_FILES}
    ${DEVICES_MANUFACTOR_FILES}
    ${SERVER_EYBOND_FILES}
    ${SERVER_COMMON_FILES}
    ${SERVER_STATEGRID_FILES}
    ${FOTA_FILES}
    ${LIB_FILES}
    ${PUB_FILES}
    )
 else()
    set(target 000_4G04_01_BP_V6.0.0.0_${VERSION})
    add_appimg(${target} ${flash_ldscript}
    app/src/app_enter.c
    app/src/net_task.c 
    app/src/elog.c
    app/src/elog_async.c
    app/src/elog_buf.c
    app/src/elog_port.c
    app/src/elog_utils.c
    app/src/uart_operate.c
    app/src/common_data.c
    app/src/data_collector_parameter_table.c
    app/src/eybond_modbus_tcp_protocol.c
    app/src/big_little_endian_swap.c
    app/src/debug_cmd.c
    app/src/parameter_number_and_value.c
    app/src/md5_test.c
    app/src/modbus_crc.c
    app/src/modbus_md5.c
    app/src/hextostr.c
    app/src/watch_dog.c
    app/src/run_log.c
    app/src/data_block_burning_record_table.c
    app/src/xmodem_server.c
    app/src/gpio_operate.c
    app/src/device_access_information.c
    )
endif()

    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})

    set(pac_file ${out_hex_dir}/${target}.pac)
    set(target_img_file ${out_hex_dir}/${target}.bin)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-image -i APPIMG -a ${CONFIG_APPIMG_FLASH_ADDRESS}
                -s ${CONFIG_APPIMG_FLASH_SIZE}
                -p ${target_img_file}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py} ${target_img_file}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()

if(CONFIG_APPIMG_LOAD_FILE)
    set(target hello_file)
    add_appimg(${target} ${file_ldscript} hello_world.c)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})

    set(pac_file ${out_hex_dir}/${target}.pac)
    set(target_img_file ${out_hex_dir}/${target}.img)
    pac_init_fdl(init_fdl)
    add_custom_command(OUTPUT ${pac_file}
        COMMAND python3 ${pacgen_py} ${init_fdl}
            cfg-pack-file -i APPIMG -p ${target_img_file}
                -n ${CONFIG_APPIMG_LOAD_FILE_NAME}
            pac-gen ${pac_file}
        DEPENDS ${pacgen_py} ${target_img_file}
        WORKING_DIRECTORY ${SOURCE_TOP_DIR}
    )
    add_custom_target(${target}_pacgen ALL DEPENDS ${pac_file})
endif()
