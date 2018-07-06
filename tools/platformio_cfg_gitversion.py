Import("projenv")
import subprocess

version = "git-cmd-not-available"

try:
    version = subprocess.check_output(["git", "describe"]).strip()
except:
    pass

projenv.Append(CCFLAGS=["-DKBOX_VERSION=\"\\\"{}\\\"\"".format(version)])
