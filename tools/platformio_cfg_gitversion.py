Import("env")
import subprocess
from datetime import datetime

version = "git-cmd-not-available"

try:
    version = subprocess.check_output(["git", "describe"]).strip()
except:
    pass

env.Append(CCFLAGS=["-DKBOX_VERSION=\"\\\"{}\\\"\"".format(version)])
