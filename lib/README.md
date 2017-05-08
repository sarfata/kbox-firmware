# About the libraries included in this project

Some libraries are included in the form of Git subtrees. If you are not changing
them, then you do not need to worry about this, a copy is included in this
repository.

To pull or push to the subtrees, start by defining remotes for all the origins:

    git remote add pedvide-adc git@github.com:pedvide/ADC.git
    git remote add ttlappalainen-nmea2000 git@github.com:ttlappalainen/NMEA2000.git
    git remote add sarfata-nmea2000teensy git@github.com:sarfata/NMEA2000_teensy.git
    git remote add teachop-flexcan git@github.com:teachop/FlexCAN_Library.git
    git remote add paul-ili9341 git@github.com:PaulStoffregen/ILI9341_t3.git
    git remote add paul-time git@github.com:PaulStoffregen/Time.git
    git remote add espasynctcp git@github.com:me-no-dev/ESPAsyncTCP.git
    git remote add adafruit-ina219 git@github.com:adafruit/Adafruit_INA219.git
    git remote add espasyncwebserver git@github.com:me-no-dev/ESPAsyncWebServer.git
    git remote add bblanchon-arduinojson git@github.com:bblanchon/ArduinoJson.git

To add a new subtree:

    git subtree add --prefix=lib/ESPAsyncTCP espasynctcp/master

To pull changes from a subtree:

    git fetch <remote>
    git subtree pull --prefix=lib/<lib> <remote> master --squash

For more information, I recommend [reading
this](https://hpc.uni.lu/blog/2014/understanding-git-subtree/).
