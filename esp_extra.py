from SCons.Script import DefaultEnvironment
import os

env = DefaultEnvironment()

LOCAL_UPLOADER=os.path.join("$PROJECT_DIR","tools/esptool.py")

env.Replace(
    LOCAL_UPLOADERFLAGS=[
        "-p", "$UPLOAD_PORT",
        "-b", "115200",
        "write_flash", "0x0",
    ],
    UPLOADER=LOCAL_UPLOADER,
    UPLOADCMD='$UPLOADER $LOCAL_UPLOADERFLAGS $SOURCE',
)
