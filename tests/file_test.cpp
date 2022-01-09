/*
 * Copyright (C) 2021  Iori Torres (shortanemoia@protonmail.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Test.h"
#include <File.h>

int main(int, char** argv)
{
    printf("%s\n", get_current_dir_name());
    auto maybe_error = File::exists(argv[0]);
    TEST_FALSE(maybe_error.has_value());
    auto file_or_error = File::open(argv[0], "rb");
    TEST(file_or_error.has_result());
    File f = move(file_or_error.result());
    File file(move(f));
    TEST_FALSE(file.has_error());
    TEST_FALSE(file.eof());
    auto pos_or_error = file.getpos();
    TEST(pos_or_error.has_result());
    TEST_EQUAL(pos_or_error.result(), 0);
    auto char_or_error = file.read_byte();
    TEST(char_or_error.has_result());
    TEST_EQUAL(char_or_error.result(), 0x7f);
    auto pos_or_error2 = file.getpos();
    TEST(pos_or_error2.has_result());
    TEST_EQUAL(pos_or_error2.result(), 1);
    auto filesize_or_error = file.size();
    TEST(filesize_or_error.has_result());
    maybe_error = file.seek(SeekMode::Start, 0);
    Vector<u8> buf((size_t)file.size().result(), true);
    auto bytes_read_or_error = file.read(buf.span(), buf.size());
    TEST_FALSE(bytes_read_or_error.has_error());
    TEST_EQUAL(bytes_read_or_error.result(), (size_t)filesize_or_error.result());
    auto new_file_or_error = File::open("file_test_copy", "w+");
    TEST(new_file_or_error.has_result());
    File copy = move(new_file_or_error.result());
    auto bytes_written_or_error = copy.write(buf.span().as_readonly(), buf.size());
    TEST(bytes_written_or_error.has_result());
    Vector<u8> buf2(bytes_written_or_error.result(), true);
    [[maybe_unused]] auto _ = copy.seek(SeekMode::Start, 0);
    auto bytes_read_or_error2 = copy.read(buf2.span(), bytes_written_or_error.result());
    TEST_EQUAL(__builtin_memcmp(buf.data(), buf2.data(), bytes_read_or_error2.result()), 0);

    auto file_or_error2 = File::open("__idonotexist__", "r");
    TEST(file_or_error2.has_error());
    TEST_EQUAL(file_or_error2.error(), neo::Error::NoSuchEntity);
    return 0;
}
