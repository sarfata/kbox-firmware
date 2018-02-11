import os
Import("env")

LOCAL_UPLOADER=os.path.join("$PROJECT_DIR","tools/esptool.py")

env.Replace(
    LOCAL_UPLOADERFLAGS=[
        "-p", "$UPLOAD_PORT",
        "-b", "2000000",
        "write_flash", "--flash_mode", "dio", "--flash_size", "16m",
    ],
   UPLOADER=LOCAL_UPLOADER,
   UPLOADCMD='$UPLOADER $LOCAL_UPLOADERFLAGS $LOCAL_UPLOAD_ADDR $SOURCE',
)

if "uploadfs" not in COMMAND_LINE_TARGETS:
    env.Replace(
        LOCAL_UPLOAD_ADDR="0x0"
    )
else:
    env.Replace(
        LOCAL_UPLOAD_ADDR="$SPIFFS_START"
    )

