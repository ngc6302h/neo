/*
Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Test.h"
#include <Array.h>
#include <MultidimensionalView.h>
#include <Generator.h>

int main()
{
    Array<long, 3*3*2*3*3> matrix_storage {0};
    neo::Generators::integer_sequence<long>(1).generate_into(matrix_storage.begin(), matrix_storage.end());
    MultidimensionalView matrix(matrix_storage, 3, 3, 2, 3, 3);
    int k = 1;
    for(int x = 0; x < 3; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            for(int z = 0; z < 2; z++)
            {
                for(int w = 0; w < 3; w++)
                {
                    for(int j = 0; j < 3; j++)
                    {
                        __builtin_printf("matrix[%d, %d, %d, %d, %d] = %ld (should be %d)\n", x, y, z, w, j, matrix[x, y, z, w, j], k);
                        TEST_EQUAL((matrix[x, y, z, w, j]), k)
                        ++k;
                    }
                }
            }
        }
    }
    
    return 0;
}