Import("projenv")
import subprocess

try:
    subprocess.check_call('echo \'unsigned long strlcat(char *dst, const char *src, unsigned long dsize); int main(){{ char *a = "hello "; char *b = "world"; strlcat(a, b, 5); }}\' |{} -o test -xc -'.format(projenv['CC']), shell = True)
    # add the flags if our test program ran without errors
    projenv.Append(CCFLAGS=["-DHAVE_STRLCPY", "-DHAVE_STRLCAT"])
    print("Compiler {} has STRLCPY".format(projenv['CC']))
except Exception as e:
    print("Compiler {} *does not* have STRLCPY\n{}".format(projenv['CC'], e))
