env = Environment(CFLAGS=["-O3", "-g"])
env.Program("rfactordec", ["rfactordec.c"])

py_env = Environment(SHLIBPREFIX = '',
                     CCFLAGS=['-Wall', '-Werror', '-O3', '-g'])

py_env.ParseConfig("python3-config --cflags --ldflags | sed -e 's/-Werror=declaration-after-statement//' -e 's/-Wstrict-prototypes//'")
py_env.SharedLibrary('rfactortools/_crypt', ['rfactortools_crypt.cpp'])

sources = Glob("*.py", strings=True) + Glob("rfactortools/*.py", strings=True) + Glob("tests/*.py", strings=True)
flake_check = Command(None, sources,
                "flake8 --max-line-length=120 $SOURCES")

scripts = [
    "aiwtool.py",
    "dirtool.py",
    "gentool.py",
    "gmttool.py",
    "gtr2-to-gsc2013.py",
    "imgtool.py",
    "maspack.py",
    "masunpack.py",
    "minised-gui.py",
    "minised.py",
    "race07-ids.py",
    "rfactorcrypt.py",
    "rfactor-to-gsc2013.py",
    "rfactortools-gui.py",
    "sfxtool.py",
    "vehtool.py",
]

AlwaysBuild(Alias("test", [], "python3 -m unittest discover"))

for i in scripts + Glob("rfactortools/*.py", strings=True) + Glob("tests/*.py", strings=True):
    Alias("pylint", Command(i + ".pylint", i, "epylint $SOURCE"))
    # Alias("pylint", [], "epylint %s" % i)
    # AlwaysBuild("pylint")

Default(flake_check)

Alias("all", [flake_check, "pylint", "test"])

# EOF #
