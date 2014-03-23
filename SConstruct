env = Environment(CFLAGS=["-O3", "-g"])
env.Program("rfactordec", ["rfactordec.c"])

py_env = Environment(SHLIBPREFIX = '',
                     CCFLAGS=['-Wall', '-Werror', '-O3', '-g'])

py_env.ParseConfig("python3-config --cflags --ldflags | sed -e 's/-Werror=declaration-after-statement//' -e 's/-Wstrict-prototypes//'")
py_env.SharedLibrary('rfactortools/_crypt', ['rfactortools_crypt.cpp'])

flake_check = Command(None, Glob("*.py") + Glob("rfactortools/*.py") + Glob("tests/*.py"),
                "flake8 --max-line-length=120 $SOURCES")

AlwaysBuild(Alias("test", [], "python3 -m unittest discover"))
Default("test", flake_check)

# EOF #
