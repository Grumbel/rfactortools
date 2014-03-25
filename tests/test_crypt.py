#!/usr/bin/env python3

# rfactortools test cases
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


import unittest
import rfactortools.crypt


class CryptTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_encrypt(self):
        with self.assertRaises(RuntimeError):
            # test for wrong skip value
            rfactortools.crypt.encrypt_data(b"My Data", 0xdeadbeaf, 0x4b1dca9f960524e8, 6)

    def test_decrypt(self):
        data_in = b"My Data"

        # TODO: With unencrypted data skip value is ignored, even when
        # wrong, is that ok?
        rfactortools.crypt.decrypt_data(data_in, 6)

        # short data is assumed to be unencrypted, skip value is ignored
        self.assertEqual(rfactortools.crypt.decrypt_data(data_in, 4), data_in)

    def test_encrypt_roundtrip(self):
        data_in = b"My Data"
        expected_out = b"G\x9a\xa8H\x9f\xca\x1dK\xaf\xbe\xad\xde\x00\x00\x00\x00\x93\xa7\xfe\x9a\xbf\xaa\xbf"
        data_out = rfactortools.crypt.encrypt_data(data_in, 0xdeadbeaf, 0x4b1dca9f960524e8, 0)
        self.assertEqual(data_out, expected_out)
        self.assertEqual(rfactortools.crypt.decrypt_data(data_out, 0), data_in)


if __name__ == '__main__':
    unittest.main()


# EOF #
