# rFactorTools
# Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


env = Environment(CFLAGS=["-O3", "-g"])
env.Program("rfactordec", ["rfactordec.c"])

py_env = Environment(SHLIBPREFIX = '',
                     CCFLAGS=['-Wall', '-Werror', '-O3', '-g'])

py_env.ParseConfig("python3-config --cflags --ldflags | sed -e 's/-Werror=declaration-after-statement//' -e 's/-Wstrict-prototypes//'")
py_env.SharedLibrary('rfactortools/_crypt', ['rfactortools_crypt.cpp'])

sources = Glob("*.py", strings=True) + \
          Glob("rfactortools/*.py", strings=True) + \
          Glob("rfactortools/gui/*.py", strings=True) + \
          Glob("rfactortools/gui/*.pyw", strings=True) + \
          Glob("tests/*.py", strings=True)

flake_check = Command(None, sources,
                "python3 -m flake8.run --max-line-length=120 $SOURCES")

AlwaysBuild(Alias("test", [], "python3 -m unittest discover"))

for i in sources:
    Alias("pylint", Command(i + ".pylint", i, "epylint $SOURCE"))
    # Alias("pylint", [], "epylint %s" % i)
    # AlwaysBuild("pylint")

Default(flake_check)

Alias("all", [flake_check, "pylint", "test"])


# EOF #
