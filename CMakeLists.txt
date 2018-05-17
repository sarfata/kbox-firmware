cmake_minimum_required(VERSION 3.2)
project(kbox32-firmware)

include(CMakeListsPrivate.txt)

add_custom_target(
    PLATFORMIO_BUILD_ESP32 ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run -e esp32
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOAD_ESP32
    COMMAND ${PLATFORMIO_CMD} -f -c clion run -e esp32 -t upload
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
        PLATFORMIO_UPLOAD_ESP32_OTA
        COMMAND ${PLATFORMIO_CMD} -f -c clion run -e esp32 -t upload --upload-port kbox32.local
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
        PLATFORMIO_RUN_ESP32 ${PLATFORMIO_CMD} -f -c clion run -e esp32 -t upload
        COMMAND ${PLATFORMIO_CMD} device monitor
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_REBUILD_PROJECT_INDEX ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion init --ide clion
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
