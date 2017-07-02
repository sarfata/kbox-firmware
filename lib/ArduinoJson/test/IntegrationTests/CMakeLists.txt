# Copyright Benoit Blanchon 2014-2017
# MIT License
# 
# Arduino JSON library
# https://bblanchon.github.io/ArduinoJson/
# If you like this project, please add a star!

add_executable(IntegrationTests 
	gbathree.cpp
	round_trip.cpp
)

target_link_libraries(IntegrationTests catch)
add_test(IntegrationTests IntegrationTests)
