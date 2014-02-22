env = Environment(CFLAGS=["-O3", "-g"])
env.Program("rfactordec", ["rfactordec.c"])

py_env = Environment(SHLIBPREFIX = '',
                     CCFLAGS=['-Wall', '-Werror', '-std=c99', '-O3', '-g'])

py_env.ParseConfig("python3-config --cflags --ldflags | sed 's/-Werror=declaration-after-statement//'")
py_env.SharedLibrary('rfactorcrypt', ['rfactorcrypt.c'])

# EOF #
